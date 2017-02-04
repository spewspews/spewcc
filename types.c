#include <u.h>
#include <libc.h>
#include <avl.h>
#include "cc.h"

Avltree *types;

int
typcmp(Avl *a1, Avl *a2)
{
	Typ *t1, *t2, **c1, **c2;

	t1 = (Typ*)a1;
	t2 = (Typ*)a2;

	if(t1->t != t2->t)
		return t1->t < t2->t;

	if(t1->q != t2->q)
		return t1->q < t2->q;

	if(t1->n != t2->n)
		return t1->n < t2->n;

	c1 = t1->c;
	c2 = t2->c;
	while(c1 < t1->c + t1->n) {
		if(c1++ != c2++)
			return (uintptr)*c1 < (uintptr)*c2;
	}

	return 0;
}

void
tinit(void)
{
	types = avlcreate(typcmp);
}

Typ*
typ(uchar t, uchar q, uchar n, Typ **ta)
{
	Typ k, *type;

	k.t = t;
	k.q = q;
	k.n = n;
	k.c = ta;

	type = (Typ*)avllookup(types, &k);
	if(type != nil)
		return type;

	type = emallocz(sizeof(Typ) + n*sizeof(Typ*));
	type->t = t;
	type->q = q;
	type->n = n;
	type->c = (Typ**)(type+1);
	memcpy(type->c, ta, sizeof(Typ*)*n);
	return type;
}

Typ*
pointer(int q, Typ *t)
{
	return typ(TPTR, q, 1, &t); 
}

Typ*
record(int t, int q, int n, Typ **ta, Typ **tags)
{
	Typ *type;

	type = typ(t, q, n, ta);
	type->tags = ecalloc(n, sizeof(char*));
	memcpy(type->tags, tags, sizeof(char*)*n);
	return type;
}

Typ*
function(uchar q, Typ *p, Typ *r)
{
	Typ *ta[2];

	ta[0] = p;
	ta[1] = r;
	return typ(TFUNC, q, 2, ta);
}

Typ*
basic(int t, uchar q)
{
	return typ(t, q, 0, nil);
}
