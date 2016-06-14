#include <signal.h>
#include "ex.h"
/*
 * Ex - a text editor
 * Bill Joy UCB June 1977
 */

static void deleted(void);
static void deletem(void);

void
delete(void)
{

	nonzero();
	change();
	if (!inglobal && !inopen)
		deletem();
	else
		deleted();
	killed();
}

static void
deleted(void)
{
	register int *a1, *a2, *a3;

	a1 = addr1;
	a2 = addr2 + 1;
	a3 = unddol;
	unddol -= a2 - a1;
	undap2 -= a2 - a1;
	dol -= a2 - a1;
	do
		*a1++ = *a2++;
	while (a2 <= a3);
	a1 = addr1;
	if (a1 > dol)
		a1 = dol;
	dot = a1;
}

static void
deletem(void)
{
	register int *a1, *a2, ra;
	int dsavint;

	dsavint = signal(INTR, SIG_IGN);
	undkind = UNDCHANGE;
	a1 = addr1;
	unddol = dol;
	a2 = addr2;
	if (a2++ != dol) {
		reverse(a1, a2);
		reverse(a2, dol + 1);
		reverse(a1, dol + 1);
	}
	dol -= a2 - a1;
	unddel = a1 - 1;
	if (a1 > dol)
		a1 = dol;
	dot = a1;
	signal(INTR, dsavint);
}

void
deletenone(void)
{
	if (!inglobal) {
		undkind = UNDCHANGE;
		unddol = dol;
		unddel = addr1;
	}
}
