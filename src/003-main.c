#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

static void error(char *fmt, ...) {
	va_list va;
	va_start(va, fmt);
	vfprintf(stderr, fmt, va);
	fprintf(stderr, "\n");
	va_end(va);
	exit(1);
}

static bool equal(token *tok, char *str) {
	return memcmp(tok -> loc, str, tok -> len) == 0 && str[tok -> len] == '\0';
}

static token *skip(token *tok, char *str) {
	if (!equal(tok, str))
		error("expect '%s'", str);
	return tok -> next;
}

static int getnumber(token *tok) {
	if (tok -> kind != TK_NUM)
		error("expect a number");
	return tok -> val;
}

static token *newtoken(tokenkind kind, char *start, char *end) {
	token *tok = calloc(1, sizeof(token));
	tok -> kind = kind;
	tok -> loc = start;
	tok -> len = end - start;
	return tok;
}

static token *tokenize(char *p) {
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
			cur -> val = strtoul(p, &p, 10);
			cur -> len = p - oldptr;
			continue;
		}
		if (*p == '-' || *p == '+') {
			cur -> next = newtoken(TK_PUNCT, p, p + 1);
			cur = cur -> next;
			p++;
			continue;
		}
		error("invalid token: %c", *p);
	}
	cur -> next = newtoken(TK_EOF, p, p);
	return head.next;
}

int main(int argc, char **argv) {
	if (argc != 2) {
		error("%s: invalid number of arguments", argv[0]);
	}
	token *tok = tokenize(argv[1]);
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
