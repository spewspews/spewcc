#include <u.h>
#include <libc.h>
#include <avl.h>
#include "ccc.h"

Nod*
nod(uchar t, ...)
{
	va_list va;
	Nod *n;

	n = emallocz(sizeof(*n));
	n->t = t;
	n->line = curline;
	va_start(va, t);
	switch(t) {
	case ACOMMA:
		n->left = va_arg(va, Nod*);
		n->right = va_arg(va, Nod*);
		break;
	case ADREF:
		n->down = va_arg(va, Nod*);
		break;
	case ASYM:
		n->sym = va_arg(va, Sym*);
		break;
	case AARRAY:
		n->atag = va_arg(va, Nod*);
		n->alen = va_arg(va, Nod*);
		break;
	case AFUNC:
		n->ftag = va_arg(va, Nod*);
		n->fprm = va_arg(va, Nod*);
		break;
	}
	va_end(va);
	return n;
}
