#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../pug.h"

enum subcommands {
	SUB1,
	SUB2,
};

enum keygen_flags {
	// SETUP FLAGS
	F1SETUP1,
	F1SETUP2,

	// USEFUL FLAGS
	F1DO1,
	F1DO2,
};

enum encrypt_flags {
	// SETUP FLAGS
	F2SETUP1,

	// USEFUL FLAGS
	F2DO1,
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
int main(int argc, char **argv) {
	PugState state = {
	    .program_name = "pug_example",
	    .indent = 4,
	    .descr_padding = 60,
	    .version_major = 1,
	    .version_minor = 0,
	};

	pug_register_subcommand(
	    &state, SUB1, "sub1",
	    "The first subcommand",
	    (PugHints){0});
	pug_register_flag(&state, SUB1, F1SETUP1,
	                  (PugFlags){.s_comm = "s1",
	                             .l_comm = "setup1",
	                             .descr = "idk maybe ask for a file",
	                             .hints = (PugHints){.count = 1, .names = {"file_path"}}});
	pug_register_flag(&state, SUB1, F1SETUP2,
                    (PugFlags){.s_comm = "s2",
	                             .l_comm = "setup2",
	                             .descr = "idk maybe ask for another file",
	                             .hints = (PugHints){.count = 1, .names = {"file_path"}}});
	pug_register_flag(&state, SUB1, F1DO1,
	                  (PugFlags){.s_comm = "do1",
	                             .l_comm = "do_something",
	                             .descr = "do something 1"});
	pug_register_flag(&state, SUB1, F1DO2,
	                  (PugFlags){.s_comm = "do2",
	                             .l_comm = "do_somethin2",
	                             .descr = "do something 2"});

	pug_register_subcommand(
	    &state, SUB2, "sub2",
	    "The second subcommand",
	    (PugHints){.count = 1, .names = {"file_path"}});
	pug_register_flag(&state, SUB2, F2SETUP1,
	                  (PugFlags){.s_comm = "s2",
	                             .l_comm = "setup2",
	                             .descr = "idk maybe ask for an int",
	                             .hints = (PugHints){.count = 1, .names = {"some_int"}}});
	pug_register_flag(&state, SUB2, F2DO1,
                    (PugFlags){.s_comm = "d2",
	                             .l_comm = "do_something",
	                             .descr = "idk maybe ask for an output file",
	                             .hints = (PugHints){.count = 1, .names = {"file_path"}}});

	pug_register_help(&state);
	pug_read_args(&state, argc, argv);

	// NOLINTNEXTLINE
	pug_switch_on_subcommand_s(
	    state, subhints, // subhints is a char** that stores all the required input for the subcommand
	    case SUB1
	    : {
        // Make shure you check for enum vals only for the same subcommand
		    if (!pug_any_of(&state, F1SETUP1, F1SETUP2)) {
          // This here is used to check the exclusivity of one of these flags
			    pug_print_help(&state);
			    exit(0);
		    }
		    pug_switch_on_flag_s(
		        state, flaghints,
		        case F1SETUP1
		        : { char * f1 = flaghints[0]; } break;

		        case F1SETUP2
		        : { char * f2 = flaghints[0]; } break;

		        case F1DO1
		        : {
              foo();
            } break;

		        case F1DO2
		        : {
              bar();
            } break;
        )} break;
	    case SUB2
	    : {
      char *msg = subhints[0]; // take the required hints (see line 65 if you missed it)

      pug_switch_on_flag_s(state, flaghints,
        case F2SETUP1: {
          int i = atoi(flaghints[0]); // I know atoi isn't safe but works for demonstrative purposes
        } break;

        case F2DO1: {
          baz();
        } break;

      )} break;
      break;
	);
	return EXIT_SUCCESS;
}
#pragma clang diagnostic pop
