%{
#include <u.h>
#include <libc.h>
#include <avl.h>
#include "ccc.h"
%}

%union {
	Sym *sym;
	Typ *typ;
	Nod	*nod;
	int ival;
	u32int bits;
	struct {
		u32int bits;
		Typ *ty
		int cl;
	} tycl
}

%type	<typ>	type tag param dclspec
%type	<tycl>	dcl
%type	<bits>	bname cname qname
%type	<nod>	dclrs dclr dclr1 abdclr zexpr sblk sdcls
%type	<nod>	abdclr abdclr1 abdclr2 abdclr3

%token	<sym>	LTYPD LNAME LTAG
%token	<bits>	LCHAR LSHORT LINT LLONG LFLOAT LDOUBLE LDOT LAUTO LEXTERN
%token	<bits>	LSTATIC LTYPEDEF LREGISTER LCONST LVOLATILE LUNSIGNED LSIGNED LVOID
%token	LSTRUCT LUNION LENUM
%token	LCHAN

%left ','
%right '=' LPLEQ LMIEQ LMULEQ LDIVEQ LMODEQ LANDEQ LXOREQ LOREQ LLSHEQ LRSHEQ LSEND LRECV
%right '?' ':'
%left LOR
%left LAND
%left '|'
%left '^'
%left '&'
%left LEQ LNE
%left '<' '>' LLE LGE
%left LLSH LRSH
%left '+' '-'
%left '*' '/' '%'
%left LDEREF
%left LINC LDEC '!' '~'
%left '.' LARROW '[' '('

%%

unit:
|	unit dcl

dcl:
	btype ';'
|	btype dclrs ';' { dclrs($2, dcl($1.bits, $1.ty), $1.cl); }

btype:
	{ $$.cl = -1; $$.ty = nil; $$.bits = 0; }
|	dcl tname { $$.cl = $1.cl; $$.ty = $1.ty; $$.bits = typeor($1.tb, $2); }
|	dcl qname { $$.cl = $1.cl; $$.ty = $1.ty; $$.bits = typeor($1.tb, $2); }
|	dcl cname { $$.ty = $1.ty; $$.bits = $1.bits $$.cl = class($$.cl, $2); }
|	dcl dname { $$.cl = $1.cl; $$.bits = $1.bits; $$.ty = $2.ty; }

tname:
	LCHAR { $$ = BCHAR };
|	LSHORT { $$ = BSHORT };
|	LINT { $$ = BINT };
|	LLONG { $$ = BLONG };
|	LFLOAT { $$ = BFLOAT };
|	LDOUBLE { $$ = BDOUBLE };
|	LUNSIGNED { $$ = BUNSIGNED };
|	LSIGNED { $$ = BSIGNED };

cname:
	LAUTO { $$ = TAUTO };
|	LEXTERN { $$ = TEXTERN };
|	LSTATIC { $$ = TSTATIC };
|	LTYPEDEF { $$ = TTYPEDEF };
|	LREGISTER { $$ = TREGISTER };

qname:
	LCONST { $$ = BCONST };
|	LVOLATILE { $$ = BVOLATILE };

dname:
	LSTRUCT LTAG sblk
|	LSTRUCT sblk
|	LSTRUCT LTAG
|	LUNION LTAG sblk
|	LUNION sblk
|	LUNION LTAG
|	LENUM LTAG '{' enum '}'
|	LENUM '{' enum '}'
|	LENUM LTAG

sblk:
	{ pushblk(); } '{' sdcls '}' { popblk(); $$ = $3; }

sdcls:
	dcl dclrs ';'
|	sdcls ';' dcl dclrs ';'

dclrs:
	dclr
|	dclrs ',' dclr { nod(ACOMMA, $1, $3); }

dclr:
	dclr1
|	'*' dclr { $$ = node(ADREF, $2); }

dclr1:
	LNAME { $$ = nod(ASYM, symdcl($1, NAMES)); }
|	'(' dclr ')' { $$ = $2; }
|	dclr1 '[' zexpr ']' { $$ = nod(AARRAY, $1, $3); }
|	dclr1 '(' params ')' { $$ = nod(AFUNC, $1, $3); }

params:

abdclr:
	{ $$ = nil }
|	abdclr1

abdclr1:
	abdclr2
|	'*' { $$ = node(APTR, nil); }
|	'*' abdclr1 { $$ = node(APTR, $2); }

abdclr2:
	abdclr3
|	abdclr2 '(' params ')' { $$ = node(AFUNC, $3); }
|	abdclr2 '[' zexpr ']' { $$ = node(AARRAY, $1, $3); }

abdclr3:
	'[' zexpr ']' { $$ = node(AARRAY, nil, $2); }
|	'(' abdclr1 ')' { $$ = $2; }

%%

u32int
typeor(u32int a, u32int b)
{
	u32int c;

	c = a | b;
	if(a & b)
		if((a & b) == BLONG)
			c |= BVLONG;		/* long long => vlong */
		else
			warn("once is enough: %Q", a & b);
	return c;
}

int
class(int oc, int nc)
{
	if(oc != -1)
		error("Storage class already specified.");
	return nc;
}

Typ*
chantyp(Typ *t)
{
	Typ *ct;

	ct = emallocz(sizeof(*ct));
	ct->ty = TCHAN;
	ct->chantyp = t;
	return ct;
}

Typ*
dcl(u32int b, Typ *typ)
{
	Typ *ntyp;
	u32int tbits;

	tbits = b & ~BQUAL;
	if(tbits && typ)
		error("Type mismatch %Q and %T", b, typ);

	if(typ)
		ntyp = typ;
	else switch(b) {
	case BCHAR:
	case BCHAR|BSIGNED:
		ntyp = typs[TCHAR];
		break;
	case BCHAR|BUNSIGNED:
		ntyp = typs[TUCHAR];
		break;
	case BSHORT:
	case BSHORT|BINT:
	case BSHORT|BSIGNED:
	case BSHORT|BINT|BSIGNED:
		ntyp = typs[TSHORT];
		break;
	case BUNSIGNED|BSHORT:
	case BUNSIGNED|BSHORT|BINT:
		ntyp = typs[TUSHORT];
		break;
	case 0:
	case BINT:
	case BINT|BSIGNED:
	case BSIGNED:
		ntyp = typs[TINT];
		break;
	case BUNSIGNED:
	case BUNSIGNED|BINT:
		ntyp = typs[TUINT];
		break;
	case BLONG:
	case BLONG|BINT:
	case BLONG|BSIGNED:
	case BLONG|BINT|BSIGNED:
		ntyp = typs[TLONG];
		break;
	case BUNSIGNED|BLONG:
	case BUNSIGNED|BLONG|BINT:
		ntyp = typs[TULONG];
		break;
	case BVLONG|BLONG:
	case BVLONG|BLONG|BINT:
	case BVLONG|BLONG|BSIGNED:
	case BVLONG|BLONG|BINT|BSIGNED:
		ntyp = typs[TVLONG];
		break;
	case BVLONG|BLONG|BUNSIGNED:
	case BVLONG|BLONG|BINT|BUNSIGNED:
		ntyp = typs[TUVLONG];
		break;
	case BFLOAT:
		ntyp = typs[TFLOAT];
		break;
	case BDOUBLE:
	case BDOUBLE|BLONG:
	case BFLOAT|BLONG:
		ntyp = typs[TDOUBLE];
		break;
	case BVOID:
		ntyp = typs[TVOID];
		break;
	}

	if(!(b & BQUAL))
		return ntyp;

	ntyp = copytyp(ntyp);
	ntyp->qual = b&BQUAL;
	return ntyp;
}

Nod*
invert(Nod *n)
{
	Nod *l;

	if(n->t != ACOMMA)
		return;
	while(n->left->t == ACOMMA) {
		l = n->left;
		n->left = l->right;
		l->right = n;
		n = l;
	}
	return n;
}

Typ*
dclr(Nod *d, Typ *typ, int cl, int prm)
{
	Sym *s;

	while(d->t != ASYM) switch(d->t) {
	default:
		error("dclr: can't handle %T", d->t);
		break;
	case ADREF:
		typ = typ(TIND, typ);
		d = d->down;
		break;
	case AFUNC:
		typ = typ(TFUNC, typ, d->fprm);
		d = d->down;
		break;
	case AARRAY:
		if(par){
			t = typ(TIND, typ);
			d = d->down;
			break;
		}
		if(d->alen != nil){
			d->alen = expfold(d->alen);
			if(d->alen->t != ACONST){
				error(n, "array size not an integer constant");
				d->alen = nil;
			}
		}
		t = typ(TARRAY, t, d->alen != nil ? n->alen->lval : 0);
		d = d->down;
		break;
	}
	s->typ = typ;
	s->cl = cl;
	return typ;
}

void
dclrs(Nod *d, Typ *ty, int cl)
{
	d = invert(d);
	while(d->t == ACOMMA) {
		dclr(d->left);
		d = d->right;
	}
	dclr(d, ty, cl);
}
