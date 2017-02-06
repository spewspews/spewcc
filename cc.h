typedef struct Type Type;
typedef struct Sym Sym;
typedef struct Nod Nod;

struct Type {
	Avl;
	uchar t, q;
	uvlong a;
	Sym *tag;
	Type *c[1];
};

struct Sym {
	Avl;
	Sym *link;	/* List for unions and structs */
	char *name;
	Type *typ;
	Sym *memb;	/* List of members for union and structs. This might go away */
	long lex;
	long off;	/* offset for union or struct or auto var in function? */
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
	TVOID,
	TCHAR,
	TSHORT,
	TINT,
	TLONG,
	TVLONG,
	TFLOAT,
	TDOUBLE,
	TUCHAR,
	TUSHORT,
	TUINT,
	TULONG,
	TUVLONG,
	NBTYP,
	TPTR = NBTYP,
	TFUNC,
	TARRAY,
	TSTRUCT,
	TUNION,
	TENUM,
	NTYP,
	TDOT,
	TAUTO,
	TEXTERN,
	TSTATIC,
	TTYPEDEF,
	TREGISTER,
	TCONST,
	TVOLATILE,
	TUNSIGNED,
	TSIGNED,

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
