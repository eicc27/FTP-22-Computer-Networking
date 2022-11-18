#include "colored_string.h"

char *colored(char *format, const char *text_attr, const char *fcolor,
              const char *bcolor) {
  const unsigned long len = strlen(format);
  char *fmt = calloc(MAX_LEN, sizeof(char));
  bzero(fmt, MAX_LEN * sizeof(char));
  strcat(fmt, ANSI_START);
  if (bcolor && !fcolor) {
    printf("\033[0;31mWith background color, front color must be "
           "specified!\033[0m\n");
    exit(1);
  }
  if (text_attr) {
    strcat(fmt, text_attr);
    if (bcolor || fcolor)
      strcat(fmt, ";");
  }
  if (bcolor) {
    strcat(fmt, bcolor);
    strcat(fmt, ";");
  }
  if (fcolor) {
    strcat(fmt, fcolor);
  }
  strcat(fmt, "m");
  strncat(fmt, format, len);
  strcat(fmt, ANSI_END);
  return fmt;
}

char *red(char *format) { return colored(format, NULL, F_RED, NULL); }
char *green(char *format) { return colored(format, NULL, F_GREEN, NULL); }
char *yellow(char *format) { return colored(format, NULL, F_YELLOW, NULL); }
char *blue(char *format) { return colored(format, NULL, F_BLUE, NULL); }
char *gray(char *format) { return colored(format, ATTR_NULL, F_BLACK, NULL); }