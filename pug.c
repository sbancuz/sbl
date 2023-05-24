#include "pug.h"

void pug_register_subcommand(PugState *ps, int val, char *name, char *desc, PugHints ph) {
	ps->subcommands_count++;
	ps->subcommands[val].name = name;
	ps->subcommands[val].descr = desc;
	ps->subcommands[val].hints = ph;
}

void pug_register_flag(PugState *ps, int subcommand, int val, PugFlags pf) {
	ps->subcommands[subcommand].count++;
	ps->subcommands[subcommand].flags_handler[val] = pf;
}

void pug_register_help(PugState *ps) {
	pug_register_flag(ps, ps->subcommands_count, 0,
	                  (PugFlags){.s_comm = "h", "help", "Prints the help for the program"});
}

void pug_print_flags(PugState *ps, int sub) {
	for pug_iter_flags_m(ps, sub, flag) {
		int padding = 0;
		padding += printf("%*s%*s -%s --%s ", ps->indent, " ", ps->indent, " ",
						  ps->subcommands[sub].flags_handler[flag].s_comm,
						  ps->subcommands[sub].flags_handler[flag].l_comm);
		if (ps->subcommands[sub].flags_handler[flag].hints.count > 0) {
			for (int i = 0; i < ps->subcommands[sub].flags_handler[flag].hints.count; ++i) {
				padding += printf("<%s> ", ps->subcommands[sub].flags_handler[flag].hints.names[i]);
			}
		}
		printf("%*s%s\n", ps->descr_padding - padding ," ", ps->subcommands[sub].flags_handler[flag].descr);
	}
}

void pug_print_help(PugState *ps) {
	printf("%s V%d.%d help screen\n", ps->program_name, ps->version_major, ps->version_minor);
	pug_print_flags(ps, ps->subcommands_count);
	printf("\n");

	if (ps->subcommands_count > 0) {
		for pug_iter_subcommands_m(ps, sub) {
			int padding = 0;
			padding += printf("%*s %s ", ps->indent, " ", ps->subcommands[sub].name);
			if (ps->subcommands[sub].hints.count > 0) {
				for (int i = 0; i < ps->subcommands[sub].hints.count; ++i) {
					padding += printf("<%s> ", ps->subcommands[sub].hints.names[i]);
				}
			}
			printf("%*s%s\n", ps->descr_padding - padding ," ", ps->subcommands[sub].descr);
			pug_print_flags(ps, sub);
			printf("\n");
		}
	} else {
		pug_print_flags(ps, 0);
		printf("\n");
	}
}

int pug_match_flag(PugState *ps, int sub, const char *c) {
	for pug_iter_flags_m(ps, sub, flag) {
		if (strcmp(ps->subcommands[sub].flags_handler[flag].s_comm, &c[1]) == 0) return flag;
		if (strcmp(ps->subcommands[sub].flags_handler[flag].l_comm, &c[2]) == 0) return flag;
	}

	return -1;
}

void pug_advance(PugState *ps, int sub, const int *argc, int *count,  char **copy) {
	ps->subcommands[sub].selected = true;

	// if found, then search if it needs flags or hints
	if (ps->subcommands[sub].count == 0 && ps->subcommands[sub].hints.count == 0) {
		return;
	}

	*count += 1;
	if (*count >= *argc) {
		printf("Not enough arguments\n");
		pug_print_help(ps);
		exit(1);
	}
	copy++;

	while (*count < *argc) {
		if (*copy[0] == '-') {
			int f;
			if ((f = pug_match_flag(ps, sub, *copy)) != -1) {
				ps->subcommands[sub].flags |= (1UL << f);
				if (ps->subcommands[sub].flags_handler[f].hints.received ==
				    ps->subcommands[sub].flags_handler[f].hints.count) {
					*count += 1;
					if (*count >= *argc) {
						return;
					}
					copy++;
					continue;
				}

				for (int i = 0; i < ps->subcommands[sub].flags_handler[f].hints.count; ++i) {
					*count += 1;
					if (*count >= *argc) {
						printf("Not enough arguments\n");
						pug_print_help(ps);
						exit(1);
					}
					copy++;

					ps->subcommands[sub].flags_handler[f].hints.v[ps->subcommands[sub].flags_handler[f].hints.received] = *copy;
					ps->subcommands[sub].flags_handler[f].hints.received++;
				}

				*count += 1;
				if (*count >= *argc) {
					return;
				}
				copy++;
			} else {
				printf("Invalid flag for subcommand '%s %s'\n", ps->subcommands[sub].name, *copy);
				pug_print_help(ps);
				exit(1);
			}
		} else {
			// Can't have hints before matching ANY flags
			for (int i = 0; i < ps->subcommands[sub].count; ++i) {
				if (ps->subcommands[sub].hints.received == ps->subcommands[sub].hints.count) {
					break;
				}

				ps->subcommands[sub].hints.v[ps->subcommands[sub].hints.received] = *copy;
				ps->subcommands[sub].hints.received++;

				*count += 1;
				if (*count >= *argc) {
					printf("Not enough hints\n");
					pug_print_help(ps);
					exit(1);
				}
				copy++;
			}
		}
	}
}

void pug_read_args(PugState *ps, int argc, char **argv) {
	char **copy = argv;
	int count = 1;
	copy++;

	// If the program doesn't require any input the just return
	if (ps->subcommands_count == 0 && ps->subcommands[0].count == 0)
		return;

	// If there isn't any input than just return
	if (count == argc  && (ps->subcommands_count == 0 || ps->subcommands[0].count == 0))
		return;

	// Search for help flag
	while (count < argc) {
		if (strcmp(*copy, "-h") == 0 || strcmp(*copy, "--help") == 0) {
			pug_print_help(ps);
			exit(0);
		}

		count++;
		copy++;
	}

	copy = argv + 1;
	count = 1;

	if (ps->subcommands_count > 0) {
		if (count == argc) {
			printf("Not enough arguments\n");
			pug_print_help(ps);
			exit(1);
		}
		for pug_iter_subcommands_m(ps, sub) {
			// Search for a valid subcommand
			if (strcmp(ps->subcommands[sub].name, *copy) != 0) {
				continue;
			}
			pug_advance(ps, sub, &argc, &count, copy);
			return ;
		}
		// This should be en error
		printf("Can't find the provided subcommand\n");
		pug_print_help(ps);
		exit(1);
	} else {
		pug_advance(ps, 0, &argc, &count, copy);
		return ;
	}
}

bool pug_any_of_impl(PugState *ps, int sub, size_t any_of_flags) {
	return (ps->subcommands[sub].flags & any_of_flags) != 0;
}

bool pug_all_of_impl(PugState *ps,  int sub, size_t all_of_flags) {
	return (ps->subcommands[sub].flags & all_of_flags) == all_of_flags;
}
