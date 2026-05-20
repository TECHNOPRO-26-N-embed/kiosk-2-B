#include <stdio.h>
#include <string.h>
#include "input.h"

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

int read_int_input(const char *prompt) {
    int val;
    printf("%s", prompt);
    fflush(stdout);
    if (scanf("%d", &val) != 1) {
        clear_input_buffer();
        return -1; /* 非数値入力 */
    }
    clear_input_buffer();
    return val;
}

void read_string_input(const char *prompt, char *buf, int max_len) {
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(buf, max_len, stdin) != NULL) {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
        }
    } else {
        buf[0] = '\0';
    }
}
