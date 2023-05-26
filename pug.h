//
// Created by sbancuz on 4/4/23.
//

#ifndef SBL_PUG_H
#define SBL_PUG_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "macro_builder.h"

#define PUG_MAX_HINTS_FOR_COMMAND 5
#define PUG_MAX_COMMANDS 64
typedef struct pug_hints_T {
	uint8_t count;
	uint8_t received;
	char *names[PUG_MAX_HINTS_FOR_COMMAND];
	char *v[PUG_MAX_HINTS_FOR_COMMAND];
} PugHints;

typedef struct pug_flags_T {
	PugHints hints;
	char s_comm[4];
	char *l_comm;
	char *descr;
} PugFlags;

typedef struct pug_state_T {
	char *program_name;
	uint8_t version_major;
	uint8_t version_minor;
	uint8_t descr_padding;
	uint8_t indent;
	uint8_t subcommands_count;
	struct {
		size_t flags;
		uint8_t count;
		bool selected;
		char *name;
		char *descr;
		PugFlags flags_handler[PUG_MAX_COMMANDS];
		PugHints hints;
	} subcommands[PUG_MAX_HINTS_FOR_COMMAND];
} PugState;

inline void pug_register_subcommand(PugState *ps, int val, char *name, char *desc, PugHints ph);
inline void pug_register_flag(PugState *ps, int subcommand, int val, PugFlags pf);
inline void pug_register_help(PugState *ps);
inline void pug_print_help(PugState *ps);
inline int pug_match_flag(PugState *ps, int sub, const char *c);
inline void pug_read_args(PugState *ps, int argc, char **argv);

// WARNING: use these macros only inside of pug_iter_subcommands macro
#define pug_any_of(ps, ...) pug_any_of_impl((ps), (sub), (pug_to_flags(__VA_ARGS__)))
#define pug_all_of(ps, ...) pug_all_of_impl((ps), (sub), (pug_to_flags(__VA_ARGS__)))
inline bool pug_any_of_impl(PugState *ps, int sub, size_t any_of_flags);
inline bool pug_all_of_impl(PugState *ps, int sub, size_t all_of_flags);

#define pug_to_flags(...) \
	FOR_EACH(pug_to_flag, __VA_ARGS__) 0
#define pug_to_flag(x) (1UL << x) |

#define pug_iter_subcommands_s(state, sub) (int (sub) = 0; (sub) < (state).subcommands_count; (sub)++)
#define pug_iter_subcommands_m(state, sub) (int (sub) = 0; (sub) < (state)->subcommands_count; (sub)++)

#define pug_iter_flags_s(state, sub, flag) (int (flag) = 0; (flag) < (state).subcommands[sub].count; (flag)++)
#define pug_iter_flags_m(state, sub, flag) (int (flag) = 0; (flag) < (state)->subcommands[sub].count; (flag)++)

#define pug_switch_on_subcommand_s(state, subhints,  body)          \
    do {                                                            \
        for pug_iter_subcommands_s((state), _pug_sub) {                  \
            if (!(state).subcommands[(_pug_sub)].selected)               \
                continue;                                           \
			char **subhints = (state).subcommands[(_pug_sub)].hints.v; 	\
            switch (_pug_sub) {                                          \
				body                                                \
			}                                                       \
                                                                    \
        }                                                           \
    } while(false)                                                  \

#define pug_switch_on_flag_s(state, flaghints, body)                                        \
    do {                                                                                    \
		for pug_iter_flags_s(state, _pug_sub, flag) {                                            \
			if (!(((state).subcommands[_pug_sub].flags & (1 << flag)) != 0))                     \
				continue;                                                                   \
			char **flaghints = (state).subcommands[(_pug_sub)].flags_handler[flag].hints.v;		\
			switch(flag) {                                                                  \
                body;                                                                       \
			}															                    \
		}                                                                                   \
    } while(false);                                                                         \

#endif //SBL_PUG_H
