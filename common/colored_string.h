#ifndef __COLORED_STRING
#define __COLORED_STRING

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __linux__
#include <strings.h>
#else
#endif

#define MAX_LEN 1024

#define ATTR_DEFLT "0"
#define ATTR_HILGT "1"
#define ATTR_NULL "2"
#define ATTR_UNDRL "4"

#define F_RED "31"
#define F_GREEN "32"
#define F_YELLOW "33"
#define F_BLUE "34"
#define F_BLACK "30"
#define F_WHITE "37"

#define B_RED "41"
#define B_GREEN "42"
#define B_YELLOW "43"
#define B_BLUE "44"
#define B_BLACK "40"
#define B_WHITE "47"

#define ANSI_START "\033["
#define ANSI_END "\033[0m"

#define PBAR_LEN 10

char *colored(char *format, const char *text_attr, const char *fcolor,
              const char *bcolor);
char *red(char *format);
char *green(char *format);
char *yellow(char *format);
char *blue(char *format);
char *gray(char *format);

#endif