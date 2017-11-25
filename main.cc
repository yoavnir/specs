#include <stdio.h>
#include <stdlib.h>
#include "specs.h"

static void dump_tospec(unsigned int pos, unsigned int tlen, bool pad)
{
	if (pos==POS_NEXT) {
		gprintf(stderr, " next.\n");
		return;
	}

	if (pos==POS_NEXTWORD) {
		gprintf(stderr, " next, leaving one space.\n");
		return;
	}

	gprintf(stderr, ", onto position %u", pos);

	if (tlen!=UNLIMITED_LENGTH) {
		if (pad) {
			gprintf(stderr, ", for a length of %u, padding if necessary.\n", tlen);
		} else {
			gprintf(stderr, ", for a length of up to %u.\n", tlen);
		}
	} else gprintf(stderr, ".\n");
}

static void dump_specs()
{
	int i;
	for (i=0; i<spec_unit_count; i++) {
		gprintf(stderr, "Spec Unit %u: ", i+1);
		switch(specs[i].action) {
			case ACTION_WRITE:
				gprintf(stderr, "Write\n");
				continue;
			case ACTION_WRITE_ERR:
				gprintf(stderr, "Write to stderr\n");
			case ACTION_READ:
				gprintf(stderr, "Read\n");
				continue;
			case ACTION_REDO:
				gprintf(stderr, "ReDo\n");
				continue;
			case ACTION_COPY_LITERAL:
				gprintf(stderr, "Copy literal <<%s>>", specs[i].literal);
				dump_tospec(specs[i].pos, specs[i].tlen, specs[i].pad_output);
				continue;
			case ACTION_COPY_CHARS:
				gprintf(stderr, "Copy chars in position %u-%u", specs[i].first, specs[i].last);
				if (specs[i].pad_input)
					gprintf(stderr, ", padding if necessary");
				dump_tospec(specs[i].pos, specs[i].tlen, specs[i].pad_output);
				continue;
			case ACTION_COPY_WORDS:
				gprintf(stderr, "Copy words in position %u-%u", specs[i].first, specs[i].last);
				dump_tospec(specs[i].pos, specs[i].tlen, specs[i].pad_output);
				continue;
			default:
				gprintf(stderr, "\n\n%s: Error. Spec units contain invalid entry.\n", program_name);
				exit(0);
		}
	}
}


int main(int argc, char** argv)
{
	bool	rc = parse_arguments(argc, argv);


	if (rc) {
		if (dodebugprt)
			dump_specs();
		gprintf(stderr, "%s: Will parse the first %u words.\n", program_name, last_word_to_scan);
		specs_execute();
		gprintf(stderr, "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\n");
		gprintf(stderr, "         1         2         3         4         5         6         7         8         9        10\n");
	}
	else
		gprintf(stderr, "%s: parse_arguments returned false\n", program_name);

	return 0;
}
