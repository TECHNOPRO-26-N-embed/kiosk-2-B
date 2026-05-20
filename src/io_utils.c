#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io_utils.h"

/* 文字列を厳密に整数へ変換する（余分な文字は不許可）。 */
int parse_int_strict(const char *s, int *out) {
    char *end;
    long v;

    /* 空文字列やNULLは数値として扱わない。 */
    if (s == NULL || *s == '\0' || out == NULL) {
        return 0;
    }

    /* strtol で変換し、変換後に余剰文字がないか検証する。 */
    v = strtol(s, &end, 10);
    while (*end != '\0') {
        if (!isspace((unsigned char)*end)) {
            return 0;
        }
        end++;
    }

    /* int 範囲外の値は失敗扱い。 */
    if (v < -2147483647L - 1L || v > 2147483647L) {
        return 0;
    }

    *out = (int)v;
    return 1;
}

/* 文字列を厳密に実数へ変換する（余分な文字は不許可）。 */
int parse_double_strict(const char *s, double *out) {
    char *end;
    double v;

    /* 空文字列やNULLは数値として扱わない。 */
    if (s == NULL || *s == '\0' || out == NULL) {
        return 0;
    }

    /* strtod で変換し、余分な文字が残る入力を排除する。 */
    v = strtod(s, &end);
    while (*end != '\0') {
        if (!isspace((unsigned char)*end)) {
            return 0;
        }
        end++;
    }

    *out = v;
    return 1;
}

/* fgets で残る改行コードを末尾から除去する。 */
void trim_newline(char *s) {
    int len;
    if (s == NULL) {
        return;
    }
    len = (int)strlen(s);
    /* 末尾に連続する CR/LF をすべて削る。 */
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[len - 1] = '\0';
        len--;
    }
}

/* 標準入力から1行取得し、末尾改行を除去して返す。 */
int read_line(char *buf, int size) {
    /* 呼び出し側のバッファ不正を先に弾く。 */
    if (buf == NULL || size <= 0) {
        return 0;
    }
    if (fgets(buf, size, stdin) == NULL) {
        return 0;
    }
    /* 入力後に改行を消して、後段の比較を簡単にする。 */
    trim_newline(buf);
    return 1;
}

/* 数値入力プロンプトを表示し、正しい整数が入るまで再入力させる。 */
int prompt_int(const char *prompt, int *out_value) {
    char buf[64];
    /* 正しい整数が入力されるまでこの場で再入力を促す。 */
    while (1) {
        if (prompt != NULL) {
            printf("%s", prompt);
        }
        if (!read_line(buf, sizeof(buf))) {
            return 0;
        }
        if (parse_int_strict(buf, out_value)) {
            return 1;
        }
        printf("数値を入力してください。\n");
    }
}
