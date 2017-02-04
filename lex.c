#include <u.h>
#include <libc.h>
#include <ctype.h>
#include <avl.h>
#include "y.tab.h"
#include "ccc.h"

static void keywords(void);
static void usage(void);
static void kinit(void);

void
main(int argc, char **argv)
{
	ARGBEGIN {
	} ARGEND

	if(argc == 0)
		usage();
	sinit();
	kinit();
	yyparse();
}

static void
usage(void)
{
	fprint(2, "I wish");
	exits("usage");	
}

typedef struct Keyword Keyword;
struct Keyword {
	char *name;
	int lex;
}

Keyword keys[] = {		
	"auto",	LAUTO,
	"double",	LDOUBLE,
	"int",	LINT,
	"struct",	LSTRUCT,
	"break",	LBREAK,
	"else",	LELSE,
	"long",	LLONG,
	"switch",	LSWITCH,
	"case",	LCASE,
	"enum",	LENUM,
	"register",	LREGISTER,
	"typedef",	LTYPEDEF,
	"char",	LCHAR,
	"extern",	LEXTERN,
	"return",	LRETURN,
	"union",	LUNION,
	"const",	LCONST,
	"float",	LFLOAT,
	"short",	LSHORT,
	"unsigned",	LUNSIGNED,
	"continue",	LCONTINUE,
	"for",	LFOR,
	"signed",	LSIGNED,
	"void",	LVOID,
	"default",	LDEFAULT,
	"goto",	LGOTO,
	"sizeof",	LSIZEOF,
	"volatile",	LVOLATILE,
	"do",	LDO,
	"if",	LIF,
	"static",	LSTATIC,
	"while",	LWHILE,
}

/* Keep sorted */
Keyword optab[] = {
	"!=",	LNE,
	"%=",	LMODEQ,
	"&&",	LAND,
	"&=",	LANDEQ,
	"*=",	LMULEQ,
	"++",	LINC,
	"+=",	LPLEQ,
	"--",	LDEC,
	"-=",	LMIEQ,
	"->",	LARROW,
	"/=",	LDIVEQ,
	"<<",	LLSH,
	"<=",	LLE,
	"==",	LEQ,
	">=",	LGE,
	">>",	LRSH,
	"^=",	LXOREQ,
	"|=",	LOREQ,
	"||",	LOR,
};

char ops[128] = {
	['!']	'!',
	['%']	'%',
	['&']	'&',
	['|']	'|',
	['*']	'*',
	['+']	'+',
	['-']	'-',
	['/']	'/',
	['<']	'<',
	['>']	'>',
	['^']	'^',
};

static int
opcmp(char *a, char *b)
{
	int c;

	c = *a++ - *b++
	if(c == 0)
		return *a - *b;
	return c
}

long
opsearch(char *k)
{
	Keyword *m, *a;
	int c, n;

	n = nelem(optab);
	while(n > 0) {
		m = a + (n/2);
		c = opcmp(k, m->name);
		if(c == 0)
			return m->lex;

		if(n == 1)
			break;

		if(c < 0)
			n /= 2;
		else {
			a = m;
			n -= n/2;
		}
	}
	return -1;
}

static void
kinit(void)
{
	Sym *s;
	struct Keyword *k;

	for(k = keys; k < keys+nelem(keys); k++) {
		s = sym(k->name, NAMES);
		s->lex = k->lex;
		s = sym(k->name, TAGS);
		s->lex = k->lex;
	}
}

static int getsym(Rune);
static int getconst(Rune);
static int ns = NAMES;
static int nextns;

/*
 *	TODO: L'ρ'
 *	strings, rune strings
 */

int
skipws(void)
{
	int c;

	do {
		c = getc();
		if(c == '\n')
			curline.lineno++;
	} while(c < Runeself && isspace(c));
	return c;
}

int
yylex(void)
{
	Keyword k;
	int c, op[2];

	c = skipws();
	if(c == Beof)
		return -1;

	if(c >= Runeself || isalpha(c) || c == '_')
		return getsym(c);

	if(c == '"')
		return getstring();

	if(isdigit(c))
		return getconst(c);

	if(op[0] = oplook[c]) {
		op[1] = getc();
		if((k = opsearch(op)) != -1)
			return k->lex;
		ungetc();
		return c;
	}
}

#define isalnumr(c)	\
	((c) >= Runeself || isalnum(c) || (c) == '_')

static char*
symstr(int c)
{
	static char buf[100];
	char *p;

	p = buf;
Again:
	if(p >= buf+100)
		error("Symbol too long.");
	*p++ = c;
	c = getc();
	if(isalnumr(c))
		goto Again;
	ungetc();
	*p = '\0';
	return buf;
}

static int
getsym(int c)
{
	Sym *s;
	long lex;

	s = sym(symstr(c));
	lex = s->lex;
	if(lex == LSTRUCT || lex == LUNION || lex == LENUM) {
		c = skipws();
		if(c == '{') {
			ungetc();
			yylval.sym = anontag(lex);
		} else {
			if(!isalnumr(c))
				yyerror("getsym: bad tag");
			yylval.sym = tag(lex, symstr(c));
	} else
		yylval.sym = s;
	return lex;
}

static int
getconst(Rune r)
{
	static char buf[100];
	char *p;

	p = buf;
Again:
	*p++ = r;
	r = getr();
	if(isdigitr(r))
		goto Again;

	*p = '\0';
	ungetr();
	yylval.ival = strtol(buf, nil, 10);
	return LCONST;
}

void
yyerror(char *err)
{
	error(err);
}
