#ifndef __SPECS__H
#define __SPECS__H

#define MAX_INPUT_LINE		1000000
#define MAX_OUTPUT_LINE		1000000
#define MAX_WORD_INDEX		  65536
#define MAX_SPEC_UNITS		    256
#define MAX_LITERAL_LENGTH	   1024
#define UNLIMITED_LENGTH	0xffffffff
#define POS_NEXT			0xffffffff
#define POS_NEXTWORD		0xfffffffe

enum spec_action_t {
	ACTION_INVALID,
	ACTION_COPY_WORDS,
	ACTION_COPY_CHARS,
	ACTION_COPY_LITERAL,
	ACTION_WRITE,
	ACTION_WRITE_ERR,
	ACTION_READ,
	ACTION_REDO,
	ACTION_COPY_TIMESTAMP,
	ACTION_COPY_TIMEDIFF,
	ACTION_COPY_LINENO
};

enum spec_align_t {
	ALIGN_LEFT,
	ALIGN_CENTER,
	ALIGN_RIGHT
};

#define CONVERSION(type,argstring,func)	type,
typedef enum  {
	CONVERT_NONE,
#include "conversion.h"
} cnv_tp;

struct  spec_unit {
	spec_action_t		action;
	spec_align_t		align;
	cnv_tp				conversion;
	bool				pad_input;
	bool				pad_output;
	unsigned int		first;
	unsigned int		last;
	unsigned int		pos;
	unsigned int		tlen;
	char*				literal;
	unsigned int		lit_len;
};

extern spec_unit		specs[MAX_SPEC_UNITS];
extern unsigned int		spec_unit_count;
extern bool				ws_tab[256];
extern char*			program_name;
extern unsigned int		last_word_to_scan;
extern bool				there_is_a_read;
extern char				space;
extern unsigned int		max_src_len;
extern bool				dodebugprt;
extern bool				dodebugprtx;
extern char*			converted_string;

bool					parse_arguments(int argc, char** argv);
void 					specs_execute();
char*					string_convert(char* string, unsigned int len, unsigned char conv, unsigned int* clen);

#define gprintf if (dodebugprt) fprintf
#define xprintf if (dodebugprtx) fprintf
#endif
