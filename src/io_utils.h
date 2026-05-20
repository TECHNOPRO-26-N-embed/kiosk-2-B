#ifndef IO_UTILS_H
#define IO_UTILS_H

int parse_int_strict(const char *s, int *out);
int parse_double_strict(const char *s, double *out);
void trim_newline(char *s);
int read_line(char *buf, int size);
int prompt_int(const char *prompt, int *out_value);

#endif
