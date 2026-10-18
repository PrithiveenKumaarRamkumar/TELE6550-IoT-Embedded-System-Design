/*
 * HW5 - Garage counter FSM
 *
 * Formal description: (States, Inputs, Outputs, update, initialState)
 *
 *   States       = {counting} x {0, ..., M}   (one mode, so equivalent to {0, ..., M})
 *   Inputs       = ({up, down} -> {present, absent})
 *   Outputs      = ({count} -> {0, ..., M, absent})
 *   initialState = (counting, c = 0)
 *
 *   update((counting, c), i) =
 *       ((counting, c + 1), count = c + 1)  if i(up) = present  and i(down) = absent  and c < M
 *       ((counting, c - 1), count = c - 1)  if i(down) = present and i(up) = absent   and c > 0
 *       ((counting, c),     count = absent) otherwise (implicit self-loop / stutter)
 */

#include <stdio.h>
#include <stdbool.h>

#define M 10 /* garage capacity */

typedef enum { ABSENT = 0, PRESENT = 1 } pure_t;

typedef enum { COUNTING } mode_t;

typedef struct {
    mode_t mode;
    int c; /* c in {0, ..., M} */
} state_t;

typedef struct {
    pure_t up;
    pure_t down;
} inputs_t;

typedef struct {
    bool present; /* count is absent unless a transition fires */
    int value;    /* count in {0, ..., M} when present */
} outputs_t;

static const state_t initial_state = { .mode = COUNTING, .c = 0 };

/* update: States x Inputs -> States x Outputs */
static void update(const state_t *s, inputs_t in, state_t *next, outputs_t *out)
{
    *next = *s;
    out->present = false;
    out->value = 0;

    if (in.up == PRESENT && in.down == ABSENT && s->c < M) {
        next->c = s->c + 1;
        out->present = true;
        out->value = next->c;
    } else if (in.down == PRESENT && in.up == ABSENT && s->c > 0) {
        next->c = s->c - 1;
        out->present = true;
        out->value = next->c;
    }
}

static const char *pure_str(pure_t p)
{
    return p == PRESENT ? "present" : "absent";
}

static void print_reaction(int step, const char *event, const state_t *s,
                           inputs_t in, const state_t *next, const outputs_t *out)
{
    char count_str[12];
    if (out->present) {
        snprintf(count_str, sizeof(count_str), "%d", out->value);
    } else {
        snprintf(count_str, sizeof(count_str), "absent");
    }
    printf("%-4d %-10s | up=%-7s down=%-7s | state=(counting, c=%2d) | count=%-6s | next=(counting, c=%2d)\n",
           step, event, pure_str(in.up), pure_str(in.down), s->c, count_str, next->c);
}

void app_main(void)
{
    const inputs_t car_in  = { .up = PRESENT, .down = ABSENT };
    const inputs_t car_out = { .up = ABSENT,  .down = PRESENT };

    state_t s = initial_state;
    state_t next;
    outputs_t out;
    int step = 1;

    printf("\nHW5 garage counter FSM (M = %d)\n", M);
    printf("Initial state: (counting, c=%d) - empty garage\n\n", s.c);

    for (int i = 0; i < 6; i++) {
        update(&s, car_in, &next, &out);
        print_reaction(step++, "car in", &s, car_in, &next, &out);
        s = next;
    }
    for (int i = 0; i < 2; i++) {
        update(&s, car_out, &next, &out);
        print_reaction(step++, "car out", &s, car_out, &next, &out);
        s = next;
    }

    printf("\nFinal state: (counting, c=%d) cars in garage\n", s.c);
}
