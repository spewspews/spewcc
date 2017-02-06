#include <u.h>
#include <libc.h>
#include <avl.h>
#include "cc.h"

Avltree *types;

int
typcmp(Avl *a1, Avl *a2)
{
	Type *t1, *t2, **p1, **p2;
	int cmp;

	t1 = (Type*)a1;
	t2 = (Type*)a2;

	if((cmp = memcmp(t1, t2, sizeof(Type))) != 0)
		return cmp;

	p1 = t1->c;
	p2 = t2->c;
	for(;;) {
		t1 = *p1++;
		t2 = *p2++;
		if(t1 != t2) {
			if((uintptr)t1 < (uintptr)t2)
				return -1;
			return 1;
		}
		if(t1 == nil)
			return 0;
	}
}

void
tinit(void)
{
	types = avlcreate(typcmp);
}

Type*
type(uchar t, uchar q, uvlong a, Sym *tag, Type **ta)
{
	static Type *k;
	Type *typ;
	int n;

	if(k == nil)
		k = emallocz(sizeof(Type) + sizeof(Type*)*256);

	k->t = t;
	k->q = q;
	k->a = a;
	k->tag = tag;
	for(n = 0; ta[n] != nil; n++) {
		if(n == 256)
			yyerror("type: too many children");
		k->c[n] = ta[n];
	}
	k->c[n] = nil;

	typ = (Type*)avllookup(types, k);
	if(typ != nil)
		return typ;

	typ = emallocz(sizeof(Type) + n*sizeof(Type*));
	typ->t = t;
	typ->q = q;
	typ->a = a;
	typ->tag = tag;
	memcpy(typ->c, ta, sizeof(Type*)*(n+1));
	return typ;
}

Type*
tagtype(uchar t, uchar q, Sym *tag)
{
	static Type *ta[1];

	return type(t, q, 0, tag, ta);
}

Type*
array(int q, uvlong a, Type *t)
{
	Type *ta[2];

	ta[0] = t;
	ta[1] = nil;
	return type(TARRAY, q, a, nil, ta);
}
	
Type*
pointer(int q, Type *t)
{
	Type *ta[2];

	ta[0] = t;
	ta[1] = nil;
	return type(TPTR, q, 0, nil, ta); 
}

Type*
basictype(uchar t, uchar q)
{
	static Type *ta[1];

	return type(t, q, 0, nil, ta);
}

Type*
record(uchar t, uchar q, Type **ta)
{
	return type(t, q, 0, nil, ta);
}
