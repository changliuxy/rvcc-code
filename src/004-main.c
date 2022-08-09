#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdbool.h>

typedef enum {
    TK_PUNCT,
    TK_NUM,
    TK_EOF,
} tokenkind;

typedef struct token token;

struct token {
    tokenkind kind;
    token *next;
    int val;
    char *loc;
    int len;
};

static char *currentinput;

static void error(char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    fprintf(stderr, "\n");
    va_end(va);
    exit(1);
}

static void verrorat(char *loc, char *fmt, va_list va) {
    fprintf(stderr, "%s\n", currentinput);
    int pos = loc - currentinput;
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, va);
    fprintf(stderr, "\n");
    va_end(va);
    exit(1);
}

static void errorat(char *loc, char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    verrorat(loc, fmt, va);
}

static void errortok(token *tok, char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    verrorat(tok -> loc, fmt, va);
}

static bool equal(token *tok, char *str) {
    //把存储区 str1 和存储区 str2 的前 n 个字节进行比较。
    return memcmp(tok -> loc, str, tok -> len) == 0 && str[tok -> len] == '\0';
}

static token *skip(token * tok, char *str) {
    if (!equal(tok, str))
        errortok(tok, "expect '%s'", str);
    return tok -> next;
}

static int *getnumber(token *tok) {
    if (tok -> kind != TK_NUM)
        errortok(tok, "expect a number");
    return tok -> val;
}

static token *newtoken(tokenkind kind, char *start, char *end) {
    token *tok = calloc(1, sizeof(token));
    tok -> kind = kind;
    tok -> loc = start;
    tok -> len = end - start;
    return tok;
}

static token *tokenize() {
    char *p = currentinput;
    token head = {};
    token *cur = &head;
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }
        if (isdigit(*p)) {
            cur -> next = newtoken(TK_NUM, p, p);
            cur = cur -> next;
            const char *oldptr = p;
            // 把参数 str 所指向的字符串根据给定的 base 转换为一个无符号长整数
            cur -> val = strtoul(p, &p, 10);
            cur -> len = p - oldptr;
            continue;
        }
        if (*p == '+' || *p == '-') {
            cur -> next = newtoken(TK_PUNCT, p, p + 1);
            cur = cur -> next;
            p++;
            continue;
        }
        errorat(p, "invalid token!");
    }
    cur -> next = newtoken(TK_EOF, p, p);
    return head.next;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        error("%s: invalid number of arguments", argv[0]);
    }
    currentinput = argv[1];
    token *tok = tokenize();
    printf("  .globl main\n");
    printf("main:\n");
    printf("  li a0, %d\n", getnumber(tok));
    tok = tok -> next;
    while (tok -> kind != TK_EOF) {
        if (equal(tok, "+")) {
            tok = tok -> next;
            printf("  addi a0, a0, %d\n", getnumber(tok));
            tok = tok -> next;
            continue;
        }
        tok = skip(tok, "-");
        printf("  addi a0, a0, -%d\n", getnumber(tok));
        tok = tok -> next;
    }
    printf("  ret\n");
    return 0;
}