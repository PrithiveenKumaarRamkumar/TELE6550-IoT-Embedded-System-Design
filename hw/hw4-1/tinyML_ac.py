#!/usr/bin/python3
# TinyML air-conditioner model.
#
# Sensors:  temperature 0..45 C, light 0..100 %, humidity 0..100 %
# Rule:     AC is ON when light > 70 % AND (temperature > 25 C OR humidity > 65 %)
#
# Builds a 100000-sample database, sweeps hidden-layer size and number of
# training iterations (epochs), keeps the best model, and writes its weights
# as a C header for the ESP32 project (esp32_ac/main/ac_model_weights.h).

import os
import numpy as np
import tensorflow as tf

from tensorflow.keras import layers
from tensorflow.keras import models

SEED = 42
tf.keras.utils.set_random_seed(SEED)

TEMP_MAX = 45.0
LIGHT_MAX = 100.0
HUM_MAX = 100.0

N = 100000

HIDDEN_NODES = [2, 4, 8, 16, 32, 64]
EPOCH_CHECKPOINTS = [5, 10, 25, 50, 100]   # "maximum number of iterations"
BATCH_SIZE = 256

# A smaller model is preferred when it is within this much of the best accuracy
# (fewer weights = less flash/RAM and faster inference on the ESP32).
TOLERANCE = 0.001

HERE = os.path.dirname(os.path.abspath(__file__))
HEADER_PATH = os.path.join(HERE, "esp32_ac", "main", "ac_model_weights.h")


def ac_rule(temp, light, hum):
    return (light > 70) & ((temp > 25) | (hum > 65))


def normalize(x):
    return x / np.array([TEMP_MAX, LIGHT_MAX, HUM_MAX], dtype=np.float32)


def build_model(hidden):
    model = models.Sequential()
    model.add(layers.Input(shape=(3,)))
    model.add(layers.Dense(hidden, activation='relu'))
    model.add(layers.Dense(1, activation='sigmoid'))
    model.compile(loss='binary_crossentropy', optimizer='adam', metrics=['accuracy'])
    return model


# ---------------------------------------------------------------- database
rng = np.random.default_rng(SEED)

temp = rng.uniform(0, TEMP_MAX, N)
light = rng.uniform(0, LIGHT_MAX, N)
hum = rng.uniform(0, HUM_MAX, N)

database_input = np.stack([temp, light, hum], axis=1).astype(np.float32)
database_output = ac_rule(temp, light, hum).astype(np.float32).reshape(-1, 1)

print(f"Database: {N} samples, AC ON in {database_output.mean() * 100:.1f}% of them")

# 80% train / 10% validation (model selection) / 10% test (final score)
x = normalize(database_input)
n_train, n_val = int(N * 0.8), int(N * 0.1)
x_train, y_train = x[:n_train], database_output[:n_train]
x_val, y_val = x[n_train:n_train + n_val], database_output[n_train:n_train + n_val]
x_test, y_test = x[n_train + n_val:], database_output[n_train + n_val:]

# ---------------------------------------------------------------- sweep
# Each hidden size is trained once for the maximum number of epochs; the
# validation accuracy after each checkpoint epoch is the score for that
# (hidden nodes, max iterations) pair.
results = {}
for hidden in HIDDEN_NODES:
    tf.keras.utils.set_random_seed(SEED)
    model = build_model(hidden)
    history = model.fit(x_train, y_train, epochs=max(EPOCH_CHECKPOINTS), batch_size=BATCH_SIZE,
                        validation_data=(x_val, y_val), verbose=0)
    for epochs in EPOCH_CHECKPOINTS:
        results[(hidden, epochs)] = history.history['val_accuracy'][epochs - 1]
    print(f"hidden={hidden:3d}  " + "  ".join(
        f"{e}ep={results[(hidden, e)] * 100:6.2f}%" for e in EPOCH_CHECKPOINTS))

print("\nValidation accuracy (%)  rows = hidden nodes, columns = epochs")
print("hidden " + "".join(f"{e:>9d}" for e in EPOCH_CHECKPOINTS))
for hidden in HIDDEN_NODES:
    print(f"{hidden:6d} " + "".join(f"{results[(hidden, e)] * 100:9.2f}" for e in EPOCH_CHECKPOINTS))

best_acc = max(results.values())
# Smallest network first, then fewest epochs, that reaches best_acc - TOLERANCE
best_hidden, best_epochs = min((k for k, v in results.items() if v >= best_acc - TOLERANCE),
                               key=lambda k: (k[0], k[1]))
print(f"\nBest overall validation accuracy: {best_acc * 100:.2f}%")
print(f"Selected: {best_hidden} hidden nodes, {best_epochs} epochs "
      f"({results[(best_hidden, best_epochs)] * 100:.2f}%)")

# ---------------------------------------------------------------- final model
tf.keras.utils.set_random_seed(SEED)
model = build_model(best_hidden)
model.fit(x_train, y_train, epochs=best_epochs, batch_size=BATCH_SIZE, verbose=0)

print()
model.summary()

_, test_acc = model.evaluate(x_test, y_test, verbose=0)
print(f"Test accuracy (10000 unseen samples): {test_acc * 100:.2f}%")

w1, b1 = [w.numpy() for w in model.layers[0].weights]   # w1: (3, H), b1: (H,)
w2, b2 = [w.numpy() for w in model.layers[1].weights]   # w2: (H, 1), b2: (1,)

# Same math as predict() in the C code, on every integer input the ESP32 can get.
grid = np.array(np.meshgrid(np.arange(46), np.arange(101), np.arange(101), indexing='ij')).reshape(3, -1).T
hidden_out = np.maximum(0, normalize(grid.astype(np.float32)) @ w1 + b1)
c_pred = (hidden_out @ w2[:, 0] + b2[0]) > 0     # sigmoid(z) > 0.5  <=>  z > 0
grid_ok = (c_pred == ac_rule(grid[:, 0], grid[:, 1], grid[:, 2])).mean()
print(f"C-equivalent accuracy on every integer input ({len(grid)} combos): {grid_ok * 100:.2f}%")

testing_input = np.stack([rng.integers(0, 46, 10), rng.integers(0, 101, 10), rng.integers(0, 101, 10)], axis=1)
testing_output = model.predict(normalize(testing_input.astype(np.float32)), verbose=0)
print()
for (t, l, h), p in zip(testing_input, testing_output[:, 0]):
    print(f"Temp: {t:2d}C, Light: {l:3d}%, Humidity: {h:3d}%  ->  AC: {'ON ' if p > 0.5 else 'OFF'}"
          f"  (p={p:.3f}, rule: {'ON' if ac_rule(t, l, h) else 'OFF'})")

# ---------------------------------------------------------------- C header
def c_float(v):
    s = f"{float(v):.9g}"
    if not any(c in s for c in ".en"):   # "0" -> "0.0" so the f suffix is valid C
        s += ".0"
    return s + "f"


def c_floats(values):
    return ", ".join(c_float(v) for v in values)

line = "// Generated by tinyML_ac.py - do not edit by hand.\n"
line += f"// ANN 3 -> {best_hidden} (ReLU) -> 1 (sigmoid), trained {best_epochs} epochs\n"
line += f"// Test accuracy: {test_acc * 100:.2f}%\n\n"
line += "#pragma once\n\n"
line += f"#define N_INPUT 3\n#define N_HIDDEN {best_hidden}\n\n"
line += f"#define TEMP_MAX {TEMP_MAX:.1f}f\n#define LIGHT_MAX {LIGHT_MAX:.1f}f\n#define HUM_MAX {HUM_MAX:.1f}f\n\n"
line += "// w1[i][j]: weight from input i (0 = temperature, 1 = light, 2 = humidity) to hidden node j\n"
line += "static const float w1[N_INPUT][N_HIDDEN] = {\n"
line += "".join(f"\t{{ {c_floats(row)} }},\n" for row in w1)
line += "};\n\n"
line += f"static const float b1[N_HIDDEN] = {{ {c_floats(b1)} }};\n\n"
line += f"static const float w2[N_HIDDEN] = {{ {c_floats(w2[:, 0])} }};\n\n"
line += f"static const float b2 = {c_float(b2[0])};\n"

os.makedirs(os.path.dirname(HEADER_PATH), exist_ok=True)
with open(HEADER_PATH, "w") as f:
    f.write(line)

print("\n" + line)
print(f"Weights written to {HEADER_PATH}")
