#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdint-gcc.h>

// TODO: Support subcommands
//       Consider a dependency-tree and only print the "top-level" or the "scoped" commands
#define PUG_MAX_HINTS_FOR_COMMAND 5
#define PUG_MAX_COMMANDS 64

typedef struct pug_hints_T {
    uint8_t count;
    char *v[PUG_MAX_HINTS_FOR_COMMAND];
}PugHints;

typedef struct pug_flags_T {
    PugHints hints;
    char s_comm;
    char *l_comm;
    char *descr;
} PugFlags;

typedef struct pug_state_T {
    char *program_name;
    uint16_t version_major;
    uint16_t version_minor;
    int indent;
    uint8_t subcommands_count;
    struct {
        size_t flags;
        uint8_t count;
        char *name;
        char *descr;
        PugFlags flags_handler[PUG_MAX_COMMANDS];
        PugHints hints;
    } subcommands[PUG_MAX_HINTS_FOR_COMMAND];
} PugState;

// TODO: Maybe put hints
void pug_register_subcommand(PugState *ps, int val, char *name, char *desc, PugHints ph) {
    ps->subcommands_count++;
    ps->subcommands[val].name = name;
    ps->subcommands[val].descr = desc;
    ps->subcommands[val].hints = ph;
}

void pug_register_command(PugState *ps, int subcommand, int val, PugFlags pf) {

    ps->subcommands[subcommand].count++;
    ps->subcommands[subcommand].flags_handler[val] = pf;
}

void pug_register_help(PugState *ps, int val) {
    pug_register_command(ps, ps->subcommands_count, val,
                         (PugFlags){.s_comm = 'h', "help", "Prints the help for the program"});
}

#define pug_iter_subcommands_s(state, sub) (int (sub) = 0; (sub) < (state).subcommands_count; (sub)++)
#define pug_iter_subcommands_m(state, sub) (int (sub) = 0; (sub) < (state)->subcommands_count; (sub)++)


#define pug_iter_flags_s(state, sub, flag) (int (flag) = 0; (flag) < (state).subcommands[sub].count; (flag)++)
#define pug_iter_flags_m(state, sub, flag) (int (flag) = 0; (flag) < (state)->subcommands[sub].count; (flag)++)

void pug_print_help(PugState *ps) {
    printf("%s V%d.%d help screen\n", ps->program_name, ps->version_major, ps->version_minor);
    if (ps->subcommands_count > 0) {
        for pug_iter_subcommands_m(ps, sub) {
            printf("%*s %s ", ps->indent, " ", ps->subcommands[sub].name);
            if (ps->subcommands[sub].hints.count > 0) {
                for (int i = 0; i < ps->subcommands[sub].hints.count; ++i) {
                    printf("<%s> ", ps->subcommands[sub].hints.v[i]);
                }
            }
            printf("%s\n",  ps->subcommands[sub].descr);
            for pug_iter_flags_m(ps, sub, flag) {
                printf("%*s%*s -%c --%s ", ps->indent, " ", ps->indent, " ",
                       ps->subcommands[sub].flags_handler[flag].s_comm,
                       ps->subcommands[sub].flags_handler[flag].l_comm);
                if (ps->subcommands[sub].flags_handler[flag].hints.count > 0) {
                    for (int i = 0; i < ps->subcommands[sub].flags_handler[flag].hints.count; ++i) {
                        printf("<%s> ", ps->subcommands[sub].flags_handler[flag].hints.v[i]);
                    }
                }
                printf("%s\n", ps->subcommands[sub].flags_handler[flag].descr);
            }
        }
    } else {
        for pug_iter_flags_m(ps, 0, flag) {
            printf("%*s -%c --%s ", ps->indent, " ",
                   ps->subcommands[0].flags_handler[flag].s_comm,
                   ps->subcommands[0].flags_handler[flag].l_comm);
            if (ps->subcommands[0].flags_handler[flag].hints.count > 0) {
                for (int i = 0; i < ps->subcommands[0].flags_handler[flag].hints.count; ++i) {
                    printf("<%s> ", ps->subcommands[0].flags_handler[flag].hints.v[i]);
                }
            }
            printf("%s\n", ps->subcommands[0].flags_handler[flag].descr);
        }
    }
}

int pug_match_flag(PugState *ps, int sub, char *c) {
    for pug_iter_flags_m(ps, sub, flag) {
        if (ps->subcommands[sub].flags_handler[flag].s_comm == *c) return flag;
        if ((ps->subcommands[sub].flags_handler[flag].l_comm, c) == 0) return flag;
    }

    return -1;
}

void pug_read_args(PugState *ps, int argc, char **argv) {
    char **copy = argv;
    int count = 1;
    copy++;

    if (ps->subcommands_count > 0) {
        for pug_iter_subcommands_m(ps, sub) {
            // Search for a valid subcommand
            if (strcmp(ps->subcommands[sub].name, *copy) != 0)
                continue;

            // if found, then search if there is a valid flag
            if (ps->subcommands[sub].count == 0) {
                return;
            }

            // TODO
        }
    }
}

enum enumtest {
    VAL1,
    VAL2,
    VAL3,
    VAL4
};

enum sub {
    SUB1
};

int main(int argc, char **argv) {
    PugState state = {
            .indent = 2,
            .program_name = "test",
            .version_major = 1,
            .version_minor = 0,
            .subcommands_count = 0,
    };
    pug_register_subcommand(&state, SUB1, "sub1", "KEKW",
                            (PugHints){.count = 2, .v = {"HINT1", "HINT2"}});
    pug_register_command(&state, SUB1, VAL1,
                         (PugFlags){.s_comm = 't', .l_comm = "long-desc", .descr = "Description",
                                    .hints = {0}});
    pug_register_command(&state, SUB1, VAL2,
                         (PugFlags){.s_comm = 't', .l_comm = "long-desc", .descr = "Description",
                                    .hints = {0}});
    pug_register_command(&state, SUB1, VAL3,
                         (PugFlags){.s_comm = 't', .l_comm = "long-desc", .descr = "Description",
                                 .hints = (PugHints){.count = 2, .v = {"HINT1", "HINT2"}}});

    // TODO: Set indentations and layout
    pug_register_help(&state, VAL4);

    pug_print_help(&state);
    // pug_read_args(&state, argc, argv);

    // TODO: Iter only on set bits and if there are none, then prints help
    //       pug_switch_on_command(state, command, {body})
}