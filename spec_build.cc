#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "specs.h"

static char* 			input;
static char* 			output;
static char*			tmpsrc;
static int* 			word_begin;
static int* 			word_end;
static unsigned int		slen;
static unsigned int		optr, omaxptr;
static int				linecnt;

extern time_t			specs_start_time;

#define READ_NOREAD		0
#define READ_NOPARSE	1
#define READ_OK			2

int read_a_line(char* src = NULL);

bool append_buf(char* src, unsigned int len, unsigned int tlen, bool pad)
{
	if (len>=tlen) {
		memcpy(output+optr, src, tlen);
		optr+=tlen;
		if (omaxptr<optr)
			omaxptr = optr;
		return true;
	}

	memcpy(output+optr, src, len);
	optr+=len;
	if (omaxptr<optr)
		omaxptr = optr;

	if (!pad)
		return true;

	memset(output+optr, space, tlen-len);
	optr+=(tlen-len);
	if (omaxptr<optr)
		omaxptr = optr;

	return true;
}

bool copy_buf(char* src, unsigned int len, unsigned int pos, unsigned int tlen, bool pad, unsigned char align)
{
	unsigned int diff;

	if (pos==POS_NEXT)
		return append_buf(src, len, tlen, pad);

	if (pos==POS_NEXTWORD) {
		output[optr++] = space;
		if (omaxptr<optr)
			omaxptr = optr;
		return append_buf(src, len, tlen, pad);
	}

	/* fill empty space before this buffer */
	if (pos>omaxptr) {
		memset(output+omaxptr, space, pos-omaxptr);
		optr = omaxptr = pos;
	}
	else optr = pos;

	if (len>=tlen) {
		diff = len-tlen;
		if (align==ALIGN_RIGHT)
			src += diff;
		else if (align==ALIGN_CENTER)
			src += (diff/2);
		memcpy(output+optr, src, tlen);
		optr+= tlen;
		if (omaxptr<optr)
			omaxptr = optr;
		return true;
	}

	if (!pad) {
		memcpy(output+optr, src, len);
		optr+= len;
		if (omaxptr<optr)
			omaxptr = optr;
		return true;
	}

	/* this is the case where we do pad */
	diff = tlen - len;
	if (align==ALIGN_RIGHT) {
		memset(output+optr, space, diff);
		optr+=diff;
		diff = 0;
	} else if (align==ALIGN_CENTER) {
		memset(output+optr, space, diff/2);
		optr+=(diff/2);
		diff -= (diff/2);
	}
	memcpy(output+optr, src, len);
	optr+=len;
	memset(output+optr, space, diff);
	optr+= diff;
	if (omaxptr<optr)
		omaxptr = optr;
	return true;
}

unsigned int fill_temp(char* src, unsigned int offset, unsigned int len, bool pad)
{
	int length_to_copy = (((int)slen) - ((int)offset));

	if (length_to_copy > 0)
		memcpy(tmpsrc, src+offset, length_to_copy);
	else
		length_to_copy = 0;

	if (!pad) {
		tmpsrc[length_to_copy] = 0;
		return length_to_copy;
	}
	else {
		memset(tmpsrc+length_to_copy, space, len-length_to_copy);
		tmpsrc[len] = 0;
		return len;
	}
}

bool apply_spec_unit(spec_unit& u)
{
	char*			start;
	unsigned int 	len, clen;
	char*			cstring;
	time_t			t;
	char			timestamp[32];
	int				rc;
	unsigned int	actual_last;

	switch (u.action) {
		case ACTION_WRITE:
			output[omaxptr]=0;
			printf("%s\n", output+1);
			optr = omaxptr = 0;
			return true;
		case ACTION_WRITE_ERR:
			output[omaxptr]=0;
			fprintf(stderr, "%s\n", output+1);
			optr = omaxptr = 0;
			return true;
		case ACTION_READ:
			rc = read_a_line();
			if (rc!=READ_OK)
				return false;
			else return true;
		case ACTION_REDO:
			output[omaxptr]=0;
			rc = read_a_line(output);
			if (rc!=READ_OK)
				return false;
			optr = omaxptr = 0;
			return true;
		case ACTION_COPY_LITERAL:
			cstring = string_convert(u.literal, u.lit_len, u.conversion, &clen);
			return copy_buf(cstring, clen, u.pos, u.tlen, u.pad_output, u.align);
		case ACTION_COPY_CHARS:
			actual_last = u.last ? u.last : 0x7fffffff;
			if (slen>=actual_last) {
				cstring = string_convert(input+u.first-1, actual_last-u.first+1, u.conversion, &clen);
				return copy_buf(cstring, clen, u.pos, u.tlen, u.pad_output, u.align);
			}
			len = fill_temp(input, u.first-1, actual_last-u.first, u.pad_input);
			cstring = string_convert(tmpsrc, len, u.conversion, &clen);
			return copy_buf(cstring, clen, u.pos, u.tlen, u.pad_output, u.align);
		case ACTION_COPY_WORDS:
			if (word_begin[u.first]<0)
				return true;
			start = input + word_begin[u.first];
			len = (u.last ? word_end[u.last] : strlen(input)) - word_begin[u.first];
			cstring = string_convert(start, len, u.conversion, &clen);
			return copy_buf(cstring, clen, u.pos, u.tlen, u.pad_output, u.align);
		case ACTION_COPY_TIMESTAMP:
			t = time(0);
			sprintf(timestamp, "[%s", ctime(&t));
			timestamp[25]=']';
			cstring = string_convert(timestamp, strlen(timestamp), u.conversion, &clen);
			return copy_buf(cstring, clen, u.pos, u.tlen, u.pad_output, u.align);
		case ACTION_COPY_TIMEDIFF:
			t = time(0) - specs_start_time;
			sprintf(timestamp, "%08u", (unsigned int)t);
			cstring = string_convert(timestamp, strlen(timestamp), u.conversion, &clen);
			return copy_buf(cstring, clen, u.pos, u.tlen, u.pad_output, u.align);
		case ACTION_COPY_LINENO:
			sprintf(timestamp, "%u", linecnt);
			cstring = string_convert(timestamp, strlen(timestamp), u.conversion, &clen);
			return copy_buf(cstring, clen, u.pos, u.tlen, u.pad_output, u.align);
		default:
			return false;
	};
}

#define IS_WS(x)	(ws_tab[((unsigned char)(x))])
bool parse_words()
{
	int i;
	char* ptr = input;

	for (i=1; i<=last_word_to_scan; i++)
		word_begin[i]=-1;

	/* skip over leading ws */
	while (IS_WS(*ptr) && *ptr)
		ptr++;
	if (!*ptr)
		return false;

	/* Now starts the first word - let's iterate */
	i = 1;
	while (i<=last_word_to_scan) {
		word_begin[i] = ptr - input;
		while (!IS_WS(*ptr) && *ptr)
			ptr++;
		word_end[i] = ptr - input;
		if (i<last_word_to_scan) {
			while (IS_WS(*ptr) && *ptr)
				ptr++;
			if (!*ptr)
				return false;
		}
		i++;
	}
	return true;
}


int read_a_line(char* src)
{
	if (!src) {
		if (!fgets(input, MAX_INPUT_LINE, stdin))
			return READ_NOREAD;
		linecnt++;
	} else {
		strcpy(input, src);
	}
	input[MAX_INPUT_LINE] = 0;
	slen = strlen(input);
	if (slen && input[slen-1]=='\n')
		input[slen--] = 0;
	if (last_word_to_scan && !parse_words()) {
		gprintf(stderr, "%s: Error with wordcount line %u\n", program_name, linecnt);
		if (!there_is_a_read)
			return READ_NOPARSE;
	}
	return READ_OK;
}

void specs_execute()
{
	int i;
	input = (char*)malloc(MAX_INPUT_LINE+1);
	output = (char*)malloc(MAX_OUTPUT_LINE+1);
	tmpsrc = (char*)malloc(max_src_len+10);
	converted_string = (char*)malloc(max_src_len+10);

	if (last_word_to_scan) {
		word_begin = (int*)malloc(sizeof(unsigned int)*(last_word_to_scan+1));
		word_end = (int*)malloc(sizeof(unsigned int)*(last_word_to_scan+1));
		if (!word_begin || !word_end) {
			gprintf(stderr, "%s: memory allocation failure.\n", program_name);
			return;
		}
	}

	if (!input || !output) {
		gprintf(stderr, "%s: memory allocation failure.\n", program_name);
		return;
	}

	linecnt = 0;

	while (!feof(stdin)) {
		int rc = read_a_line();
		if (rc==READ_NOREAD)
			return;
		if (rc==READ_NOPARSE)
			continue;
		optr = omaxptr = 0;
		for (i=0; i<spec_unit_count; i++)
			if (!apply_spec_unit(specs[i])) {
				gprintf(stderr, "%s: Error rendering line %u\n", program_name, linecnt);
				return;
			}
		output[omaxptr]=0;
		printf("%s\n", output+1);
	}
}
