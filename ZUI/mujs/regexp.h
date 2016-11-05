#ifndef regexp_h
#define regexp_h

#define regcomp js_regcomp
#define regexec js_regexec
#define regfree js_regfree

typedef struct Reprog Reprog;
typedef struct Resub Resub;

Reprog *regcomp(const wchar_t *pattern, int cflags, const wchar_t **errorp);
int regexec(Reprog *prog, const wchar_t *string, Resub *sub, int eflags);
void regfree(Reprog *prog);

enum {
	/* regcomp flags */
	REG_ICASE = 1,
	REG_NEWLINE = 2,

	/* regexec flags */
	REG_NOTBOL = 4,

	/* limits */
	REG_MAXSUB = 16
};

struct Resub {
	int nsub;
	struct {
		const wchar_t *sp;
		const wchar_t *ep;
	} sub[REG_MAXSUB];
};

#endif
