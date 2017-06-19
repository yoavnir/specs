#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "specs.h"

static bool	ws_init(const char *ws_str)
{
	unsigned char i;
	memset(ws_tab, 0, sizeof(ws_tab));
	for (i=0; ws_str[i]; i++) {
		if (ws_str[i]=='\\') {
			i++;
			if (ws_str[i]==0) {
				ws_tab['\\'] = true;
				return false;
			}
			else if (ws_str[i]=='t')
				ws_tab['\t'] = true;
			else if (ws_str[i]=='s')
				ws_tab[' '] = true;
			else if (ws_str[i]=='n')
				ws_tab['\n'] = true;
			else if (ws_str[i]=='r')
				ws_tab['\r'] = true;
			else if (ws_str[i]=='w') {
				ws_tab['\r'] = true;
				ws_tab['\n'] = true;
				ws_tab['\t'] = true;
			}
			else if (ws_str[i]=='W') {
				ws_tab['\r'] = true;
				ws_tab['\n'] = true;
				ws_tab['\t'] = true;
				ws_tab[' '] = true;
			}
			else {
				ws_tab[ws_str[i]] = true;
			}
		}
		else ws_tab[ws_str[i]]=true;
	}
	
	return true;
}

bool is_write(char* arg)
{
	if (0==strcasecmp(arg, "w")	|| 0==strcasecmp(arg, "write"))
		return true;
	else return false;
			
}

bool is_read(char* arg)
{
	if (0==strcasecmp(arg, "r")	|| 0==strcasecmp(arg, "read"))
		return true;
	else return false;
			
}

bool is_redo(char* arg)
{
	if (0==strcasecmp(arg, "redo"))
		return true;
	else return false;
			
}

#define IS_DIGIT(c)	(((c)>='0') && ((c)<='9'))
bool is_single_number(char* arg, unsigned int* first, unsigned int* last)
{
	/* format must be a string of digits followed by a hyphen and another string of digits */
	char* p = arg;
	if (!IS_DIGIT(*p))
		return false;
	p++;
	while (IS_DIGIT(*p))
		p++;
	if (*p!=0)
		return false;
	
	unsigned int idx = strtoul(arg, NULL, 10);
	if (!idx)
		return false;
	
	*first = *last = idx;
	return true;
}

bool is_hyphenated_range(char* arg, unsigned int* first, unsigned int* last)
{
	/* format must be a string of digits followed by a hyphen and another string of digits */
	char* p = arg;
	if (!IS_DIGIT(*p))
		return false;
	p++;
	while (IS_DIGIT(*p))
		p++;
	if (*p!='-')
		return false;
	p++;
	if (!IS_DIGIT(*p) && *p!='x')
		return false;
	char* plast = p;
	p++;
	while (IS_DIGIT(*p))
		p++;
	if (*p!=0)
		return false;
		
	/* OK, the format is OK. Let's try to get the first and last */
	unsigned int _first, _last;
	
	_first = strtoul(arg, NULL, 10);
	_last = strtoul(plast, NULL, 10);
	
	if (!_last && *plast=='x')
		_last = 0x7fffffff;
	
	if (!_first || !_last)
		return false;
		
	if (_last<_first)
		return false;
		
	*first = _first;
	if (_last==0x7fffffff)
		*last = 0;
	else *last = _last;
	return true;
}

bool is_dotted_range(char* arg, unsigned int* first, unsigned int* len)
{
	/* format must be a string of digits followed by a dot and another string of digits */
	char* p = arg;
	if (!IS_DIGIT(*p))
		return false;
	p++;
	while (IS_DIGIT(*p))
		p++;
	if (*p!='.')
		return false;
	p++;
	if (!IS_DIGIT(*p))
		return false;
	char* plen = p;
	p++;
	while (IS_DIGIT(*p))
		p++;
	if (*p!=0)
		return false;
		
	/* OK, the format is OK. Let's try to get the first and last */
	unsigned int _first, _len;
	
	_first = strtoul(arg, NULL, 10);
	_len = strtoul(plen, NULL, 10);
	
	if (!_first || !_len)
		return false;
				
	*first = _first;
	*len = _len;
	return true;
	
}

bool is_word_range(char* arg, unsigned int* first, unsigned int* last, bool* pad)
{
	if (arg[0]!='w' && arg[0]!='W')
		return false;
	
	if (!is_hyphenated_range(arg+1, first, last) && !is_single_number(arg+1, first, last))
		return false;
		
	if (*last > MAX_WORD_INDEX)
		return false;
	
	*pad = false;
	return true;
}

bool is_char_range(char* arg, unsigned int* first, unsigned int* last, bool* pad)
{
	if (is_hyphenated_range(arg, first, last)) {
		*pad = false;
		return true;
	}
	
	unsigned int len;
	if (is_dotted_range(arg, first, &len)) {
		*last = *first+len;
		*pad = true;
		return true;
	}
	
	if (is_single_number(arg, first, last)) {
		*pad = true;
		return true;
	}
	
	return false;
}

bool is_valid_tospec(char* arg, unsigned int* first, unsigned int* len, bool* pad)
{
	unsigned int _last;
	
	if (0==strcasecmp(arg, "n")) {
		*len = UNLIMITED_LENGTH;
		*first = POS_NEXT;
		*pad = false;
		return true;
	}
	
	if (0==strcasecmp(arg, "nw")) {
		*len = UNLIMITED_LENGTH;
		*first = POS_NEXTWORD;
		*pad = false;
		return true;
	}
	
	if (is_hyphenated_range(arg, first, &_last)) {
		*len = _last - *first + 1;
		*pad = true;
		return true;
	}
	
	if (is_dotted_range(arg, first, len)) {
		*pad = true;
		return true;
	}
	
	if (is_single_number(arg, first, &_last)) {
		*len = UNLIMITED_LENGTH;
		*pad = false;
		return true;
	}
	
	return false;
}

static int
copy_literal(char* dst, char* src)
{
	int len = 0;
	
	while (*src) {
		if (*src=='\\') {
			src++;
			switch (*src) {
				case '\0':	return -1;
				case 't':	*dst++ = '\t';	break;
				case 'n':	*dst++ = '\n';	break;
				case 'r':	*dst++ = '\r';	break;
				case 's':	*dst++ = ' ';	break;
				default:	*dst++ = *src;
			}
			src++;
			len++;
		}
		else {
			*dst++ = *src++;
			len++;
		}
	}
	return len;
}

bool 	get_literal(char* arg, char** literal, unsigned int* len)
{
	unsigned int l = strlen(arg);
		
	if (l<=2 || arg[0]!='\'' || arg[l-1]!='\'') {
		char* _literal = (char*)malloc(l);
		*literal = _literal;
		*len = copy_literal(_literal, arg);
		return (*len>=0);
	}
	
	arg[l-1] = 0;
	*literal = arg+1;
	*len = l-2;
	return true;
}

bool	is_alignment_spec(char* arg, spec_align_t* alignment)
{
	if (0==strcmp(arg, "l")) {
		*alignment = ALIGN_LEFT;
		return true;
	} else if (0==strcmp(arg, "c")) {
		*alignment = ALIGN_CENTER;
		return true;
	} else if (0==strcmp(arg, "r")) {
		*alignment = ALIGN_RIGHT;
		return true;
	}
	else return false;
}

#ifdef CONVERSION
#undef CONVERSION
#endif
#define CONVERSION(type,argstring,func)		if (0==strcmp(arg,argstring)) { *conversion = type; return true; }
bool	is_valid_conversion(char* arg, cnv_tp* conversion)
{
#include "conversion.h"
	return false;
}

bool	parse_arguments(int argc, char** argv)
{
	program_name = argv[0];
	if (program_name[0]=='.' && program_name[1]=='/')
		program_name+=2;
	unsigned int original_argc = argc;
	
	/* skip over the program name */
	argc--;  argv++;

	/* first, take care of the switches */
	ws_init(" \t\r\n");
	bool no_more_switches = false;
	while (!no_more_switches && argc>0) {
		if (strcmp(argv[0], "-ws")==0 && argc>1) {
			argv++; argc--;
			if (!ws_init(argv[0])){
				gprintf(stderr, "%s: argument %u is an invalid white-space spec.\n", program_name, original_argc-1);
				return false;
			}
			argv++; argc--;
		}
		else if (strcmp(argv[0], "-spc")==0 && argc>1) {
			argv++; argc--;
			space = argv[0][0];
			argv++; argc--;
		}
		else if (strcmp(argv[0], "-d")==0) {
			dodebugprt = true;
			argv++; argc--;
		}
		else if (strcmp(argv[0], "-dd")==0) {
			dodebugprt = true;
			dodebugprtx = true;
			argv++; argc--;
		}
		else no_more_switches = true;
	}
	
	unsigned int spc_idx = 0;
	while (argc>0) {
		specs[spc_idx].conversion = CONVERT_NONE;
		if (is_write(argv[0])) {
			specs[spc_idx].action = ACTION_WRITE;
			argc--; argv++; spc_idx++;
		}
		if (is_read(argv[0])) {
			specs[spc_idx].action = ACTION_READ;
			there_is_a_read = true;
			argc--; argv++; spc_idx++;
		}
		if (is_redo(argv[0])) {
			specs[spc_idx].action = ACTION_REDO;
			there_is_a_read = true;
			argc--; argv++; spc_idx++;
		}
		else if (is_word_range(argv[0], &specs[spc_idx].first, &specs[spc_idx].last, &specs[spc_idx].pad_input)) {
			specs[spc_idx].action = ACTION_COPY_WORDS;
			specs[spc_idx].align = ALIGN_LEFT; /* the default */
			if (specs[spc_idx].last > last_word_to_scan)
				last_word_to_scan = specs[spc_idx].last;
			if (specs[spc_idx].first > last_word_to_scan)
				last_word_to_scan = specs[spc_idx].first;
			argc--; argv++;
			if (argc>0 && is_valid_conversion(argv[0], &specs[spc_idx].conversion)) {
				argc--; argv++;
			}
			if (argc==0) {
				gprintf(stderr, "%s: argument %u is missing a to_spec.\n", program_name, original_argc-1);
				return false;
			}
			if (!is_valid_tospec(argv[0], &specs[spc_idx].pos, &specs[spc_idx].tlen, &specs[spc_idx].pad_output)) {
				gprintf(stderr, "%s: argument %u is an invalid to_spec.\n", program_name, original_argc-argc);
				return false;
			}
			argc--; argv++; 
			if (argc>0 && is_alignment_spec(argv[0], &specs[spc_idx].align)) {
				argc--; argv++;
			}
			spc_idx++;
		}
		else if (is_char_range(argv[0], &specs[spc_idx].first, &specs[spc_idx].last, &specs[spc_idx].pad_input)) {
			specs[spc_idx].action = ACTION_COPY_CHARS;
			specs[spc_idx].align = ALIGN_LEFT;
			unsigned int lll = specs[spc_idx].last - specs[spc_idx].first;
			if (lll>max_src_len)
				max_src_len = lll;
			argc--; argv++;
			if (argc>0 && is_valid_conversion(argv[0], &specs[spc_idx].conversion)) {
				argc--; argv++;
			}
			if (argc==0) {
				gprintf(stderr, "%s: argument %u is missing a to_spec.\n", program_name, original_argc-1);
				return false;
			}
			if (!is_valid_tospec(argv[0], &specs[spc_idx].pos, &specs[spc_idx].tlen, &specs[spc_idx].pad_output)) {
				gprintf(stderr, "%s: argument %u is an invalid to_spec.\n", program_name, original_argc-argc);
				return false;
			}
			argc--; argv++; 
			if (argc>0 && is_alignment_spec(argv[0], &specs[spc_idx].align)) {
				argc--; argv++;
			}
			spc_idx++;
		}
		else if (0==strcmp(argv[0], "lineno")) {
			specs[spc_idx].action = ACTION_COPY_LINENO;
			specs[spc_idx].align = ALIGN_LEFT;
			argc--; argv++;
			if (argc>0 && is_valid_conversion(argv[0], &specs[spc_idx].conversion)) {
				argc--; argv++;
			}
			if (argc==0) {
				gprintf(stderr, "%s: argument %u is missing a to_spec.\n", program_name, original_argc-1);
				return false;
			}
			if (!is_valid_tospec(argv[0], &specs[spc_idx].pos, &specs[spc_idx].tlen, &specs[spc_idx].pad_output)) {
				gprintf(stderr, "%s: argument %u is an invalid to_spec.\n", program_name, original_argc-argc);
				return false;
			}
			argc--; argv++; 
			if (argc>0 && is_alignment_spec(argv[0], &specs[spc_idx].align)) {
				argc--; argv++;
			}
			spc_idx++;
		}
		else if (0==strcmp(argv[0], "timestamp")) {
			specs[spc_idx].action = ACTION_COPY_TIMESTAMP;
			specs[spc_idx].align = ALIGN_LEFT;
			argc--; argv++;
			if (argc>0 && is_valid_conversion(argv[0], &specs[spc_idx].conversion)) {
				argc--; argv++;
			}
			if (argc==0) {
				gprintf(stderr, "%s: argument %u is missing a to_spec.\n", program_name, original_argc-1);
				return false;
			}
			if (!is_valid_tospec(argv[0], &specs[spc_idx].pos, &specs[spc_idx].tlen, &specs[spc_idx].pad_output)) {
				gprintf(stderr, "%s: argument %u is an invalid to_spec.\n", program_name, original_argc-argc);
				return false;
			}
			argc--; argv++; 
			if (argc>0 && is_alignment_spec(argv[0], &specs[spc_idx].align)) {
				argc--; argv++;
			}
			spc_idx++;
		}
		else if (0==strcmp(argv[0], "timediff")) {
			specs[spc_idx].action = ACTION_COPY_TIMEDIFF;
			specs[spc_idx].align = ALIGN_LEFT;
			argc--; argv++;
			if (argc>0 && is_valid_conversion(argv[0], &specs[spc_idx].conversion)) {
				argc--; argv++;
			}
			if (argc==0) {
				gprintf(stderr, "%s: argument %u is missing a to_spec.\n", program_name, original_argc-1);
				return false;
			}
			if (!is_valid_tospec(argv[0], &specs[spc_idx].pos, &specs[spc_idx].tlen, &specs[spc_idx].pad_output)) {
				gprintf(stderr, "%s: argument %u is an invalid to_spec.\n", program_name, original_argc-argc);
				return false;
			}
			argc--; argv++; 
			if (argc>0 && is_alignment_spec(argv[0], &specs[spc_idx].align)) {
				argc--; argv++;
			}
			spc_idx++;
		}
		else {
			if (!get_literal(argv[0], &specs[spc_idx].literal, &specs[spc_idx].lit_len)) {
				gprintf(stderr, "%s: argument %u is an invalid literal.\n", program_name, original_argc-argc);
				return false;
			}
			specs[spc_idx].action = ACTION_COPY_LITERAL;
			specs[spc_idx].align = ALIGN_LEFT;
			argc--; argv++;
			if (argc>0 && is_valid_conversion(argv[0], &specs[spc_idx].conversion)) {
				argc--; argv++;
			}
			if (!is_valid_tospec(argv[0], &specs[spc_idx].pos, &specs[spc_idx].tlen, &specs[spc_idx].pad_output)) {
				gprintf(stderr, "%s: argument %u is an invalid to_spec.\n", program_name, original_argc-argc);
				return false;
			}
			argc--; argv++; 
			if (argc>0 && is_alignment_spec(argv[0], &specs[spc_idx].align)) {
				argc--; argv++;
			}
			spc_idx++;
		}
	}
	if (spc_idx==0) {
		gprintf(stderr, "%s: No specs were found\n", program_name);
		return false;
	}
	
	spec_unit_count = spc_idx;
	return true;
}
