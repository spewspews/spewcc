#include <u.h>
#include <libc.h>
#include <avl.h>
#include "ccc.h"

static Avltree *names;
static Avltree *tags;
static Sym *save;
static int blk;
static int symcmp(Avl*, Avl*);
static Sym *get(Avltree*, char*, long);

void
sinit(void)
{
	names = avlcreate(symcmp);
	tags = avlcreate(symcmp);
}

static int
symcmp(Avl *a, Avl *b)
{
	Sym *sa, *sb;
	char ca, cb;

	sa = (Sym*)a;
	sb = (Sym*)b;

	ca = sa->name[0];
	cb = sb->name[0];
	if(ca == cb)
		return strcmp(sa->name, sb->name);
	return ca > cb ? 1 : -1;
}

void
pushblk(void)
{
	static char buf[100];
	Sym *s;

	blk++;
	seprint(buf, buf+100, "$blk%d", blk);
	sy = sym(buf);
	sy->link = saved;
	saved = sy;
}

void
popblk(void)
{
	Sym *link;

	while(saved->blk != blk) {
		link = saved->link;
		saved->link = nil;
		if(saved->lex == LTAG)
			avlinsert(tags, saved);
		else
			avlinsert(names, saved);
		saved = link;
	}
	saved = saved->link;

	blk--;
}

Sym*
dclname(Sym *s)
{
	if(s->blk == blk)
		return s;
	return symcreate(names, s->name);
}

Sym*
sym(long ns, char *name)
{
	Avltree *t;
	Sym *s, l;
	long lex;

	if(ns != NAMES && ns != TAGS) {
		error("sym: Invalid namespace");
		ns = NAMES;
	}

	l.name = name;
	t = ns == NAMES ? names : tags;
	s = avllookup(t, &l);
	if(s != nil)
		return s;
	return symcreate(t, name);
}

static Sym*
symcreate(Avltree *t, char *name)
{
	Sym *s, *r;
	long lex;

	s = emallocz(sizeof(*s) + strlen(name));
	strcpy(s->buf, name);
	s->name = s->buf;
	s->blk = blk;
	s->lex = t = names ? LNAME : LTAG;
	if((r = avlinsert(t, s)) != nil) {
		assert(r->blk < blk);
		r->link = save;
		save = r;
	}
	return s;
}
