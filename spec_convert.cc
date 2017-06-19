#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "specs.h"

static char*			conv_rot13(char* string, unsigned int len, unsigned int* clen)
{
	unsigned int i;
	for (i=0; i<len; i++) {
		if (string[i]>='A' && string[i]<='M')
			converted_string[i] = string[i]+13;
		else if (string[i]>='N' && string[i]<='Z')
			converted_string[i] = string[i]-13;
		else if (string[i]>='a' && string[i]<='m')
			converted_string[i] = string[i]+13;
		else if (string[i]>='n' && string[i]<='z')
			converted_string[i] = string[i]-13;
		else converted_string[i] = string[i];
	}
	*clen = len;
	return converted_string;
}

static char*			conv_d2x(char* string, unsigned int len, unsigned int* clen)
{
	unsigned long long l;
	
	memcpy(converted_string, string, len);
	converted_string[len+1] = 0;
	l = strtoull(converted_string, NULL, 10);
	sprintf(converted_string, "%llx", l);
	*clen = strlen(converted_string);
	return converted_string;
}

static char*			conv_x2d(char* string, unsigned int len, unsigned int* clen)
{
	unsigned long long l;
	
	memcpy(converted_string, string, len);
	converted_string[len+1] = 0;
	l = strtoull(converted_string, NULL, 16);
	sprintf(converted_string, "%llu", l);
	*clen = strlen(converted_string);
	return converted_string;
}

static char*			conv_x2b(char* string, unsigned int len, unsigned int* clen)
{
	converted_string[0]=0;
	
	int i;
	for (i=0; i<len; i++) {
		switch(string[i]) {
			case '0':
				strcat(converted_string, "0000");
				break;
			case '1':
				strcat(converted_string, "0001");
				break;
			case '2':
				strcat(converted_string, "0010");
				break;
			case '3':
				strcat(converted_string, "0011");
				break;
			case '4':
				strcat(converted_string, "0100");
				break;
			case '5':
				strcat(converted_string, "0101");
				break;
			case '6':
				strcat(converted_string, "0110");
				break;
			case '7':
				strcat(converted_string, "0111");
				break;
			case '8':
				strcat(converted_string, "1000");
				break;
			case '9':
				strcat(converted_string, "1001");
				break;
			case 'a':
			case 'A':
				strcat(converted_string, "1010");
				break;
			case 'b':
			case 'B':
				strcat(converted_string, "1011");
				break;
			case 'c':
			case 'C':
				strcat(converted_string, "1100");
				break;
			case 'd':
			case 'D':
				strcat(converted_string, "1101");
				break;
			case 'e':
			case 'E':
				strcat(converted_string, "1110");
				break;
			case 'f':
			case 'F':
				strcat(converted_string, "1111");
				break;
		}
	}

	*clen = strlen(converted_string);
	return converted_string;
}

static char*			conv_x2bt(char* string, unsigned int len, unsigned int* clen)
{
	converted_string[0]=0;
	
	int i;
	bool already_written = false;
	for (i=0; i<len; i++) {
		switch(string[i]) {
			case '0':
				if (already_written)
					strcat(converted_string, "0000");
				break;
			case '1':
				if (already_written)
					strcat(converted_string, "0001");
				else strcat(converted_string, "1");
				break;
			case '2':
				if (already_written)
					strcat(converted_string, "0010");
				else strcat(converted_string, "10");
				break;
			case '3':
				if (already_written)
					strcat(converted_string, "0011");
				else strcat(converted_string, "11");
				break;
			case '4':
				if (already_written)
					strcat(converted_string, "0100");
				else strcat(converted_string, "100");
				break;
			case '5':
				if (already_written)
					strcat(converted_string, "0101");
				else strcat(converted_string, "101");
				break;
			case '6':
				if (already_written)
					strcat(converted_string, "0110");
				else strcat(converted_string, "110");
				break;
			case '7':
				if (already_written)
					strcat(converted_string, "0111");
				else strcat(converted_string, "111");
				break;
			case '8':
				strcat(converted_string, "1000");
				break;
			case '9':
				strcat(converted_string, "1001");
				break;
			case 'a':
			case 'A':
				strcat(converted_string, "1010");
				break;
			case 'b':
			case 'B':
				strcat(converted_string, "1011");
				break;
			case 'c':
			case 'C':
				strcat(converted_string, "1100");
				break;
			case 'd':
			case 'D':
				strcat(converted_string, "1101");
				break;
			case 'e':
			case 'E':
				strcat(converted_string, "1110");
				break;
			case 'f':
			case 'F':
				strcat(converted_string, "1111");
				break;
		}
	}

	*clen = strlen(converted_string);
	return converted_string;
}

static char*			conv_b2x(char* string, unsigned int len, unsigned int* clen)
{
	unsigned long long l;
	
	memcpy(converted_string, string, len);
	converted_string[len+1] = 0;
	l = strtoull(converted_string, NULL, 2);
	sprintf(converted_string, "%llx", l);
	*clen = strlen(converted_string);
	return converted_string;
}

static char*			conv_toupper(char* string, unsigned int len, unsigned int* clen)
{
	int i;
	for (i=0; i<len; i++)
		converted_string[i]=toupper(string[i]);
	*clen = len;
	return converted_string;
}

static char*			conv_tolower(char* string, unsigned int len, unsigned int* clen)
{
	int i;
	for (i=0; i<len; i++)
		converted_string[i]=tolower(string[i]);
	*clen = len;
	return converted_string;
}

#ifdef CONVERSION
#undef CONVERSION
#endif
#define CONVERSION(type,argstring,func)		case type: return func(string,len,clen);
char*					string_convert(char* string, unsigned int len, unsigned char conv, unsigned int* clen)
{
	switch (conv) {
		case CONVERT_NONE:
			*clen = len;
			return string;
#include "conversion.h"
		default:
			return NULL;
	}
}
