typedef struct Typ Typ;
typedef struct Sym Sym;
typedef struct Nod Nod;

struct Typ {
	Avl;
	uchar t, q, n;
	char **tags;
	Typ **c;
};

struct Sym {
	Avl;
	Sym *link;
	char *name;
	union {
		Typ *typ;	/* LNAME LTYPED */
		Sym *memb;	/* LTAG */
	};
	long lex;
	uchar blk;
	char cl;
	char buf[1];
};

struct Nod {
	union {
		struct {
			Nod *left, *right;	/* ACOMMA */
		};
		struct {
			Nod *down;	/* APTR AFUNC AARRAY*/
			union {
				Nod *alen;	/* AARRAY */
				Nod *fprm;	/* AFUNC */
			};
		};
		Sym *sym;	/* ASYM */
	};
	uchar t;
};

enum {
	ADCLR,
};

enum {
	TNOTYPE,
	TCHAR,
	TSHORT,
	TINT,
	TLONG,
	TVLONG,
	TFLOAT,
	TDOUBLE,
	TDOT,
	TPTR,
	TAUTO,
	TEXTERN,
	TSTATIC,
	TTYPEDEF,
	TREGISTER,
	TCONST,
	TVOLATILE,
	TUNSIGNED,
	TSIGNED,
	TUCHAR,
	TUSHORT,
	TUINT,
	TULONG,
	TUVLONG,
	TFUNC,
	TARRAY,
	TVOID,
	TSTRUCT,
	TUNION,
	TENUM,
	NTYP,

	KCONST = 0, /* at beginning since these are saved in type */
	KVOLATILE,
	KCHAR,
	KSHORT,
	KINT,
	KLONG,
	KFLOAT,
	KDOUBLE,
	KSIGNED,

	BCONST = 1<<KCONST,
	BVOLATILE = 1<<KVOLATILE,
	BCHAR = 1<<KCHAR,
	BSHORT = 1<<KSHORT,
	BINT = 1<<KINT,
	BLONG = 1<<KLONG,
	BFLOAT = 1<<KFLOAT,
	BDOUBLE = 1<<KDOUBLE,
	BSIGNED = 1<<KSIGNED,

	BQUAL = BCONST|BVOLATILE,
};

int yyparse(void);
int yylex(void);
void yyerror(char*);

Sym *sym(Avltree *t, char*, long);
void pushblk(void);
void popblk(void);

void *emallocz(ulong);
void *ecalloc(ulong, ulong);
