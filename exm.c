#include "ex.h"
/*
 * Ex - a text editor
 * Bill Joy UCB June/August 1977
 */

static int getcopy(void);
static int getPUT(void);

void
move(void)
{
	register int *adt;

	if (Command[0] == 'M') {
		setdot1();
		markpr(addr2 == dot ? addr1 - 1 : addr2 + 1);
	} else
		setdot();
	nonzero();
	adt = address();
	if (adt == 0)
		error("%s where?|%s requires a trailing address", Command);
	ex_newline();
	move1(Command[0] != 'M', adt);
	killed();
}

int	*tad1;

void
move1(char cflag, int *addrt)
{
	register int *adt, *ad1, *ad2;
	int lines;

	adt = addrt;
	lines = (addr2 - addr1) + 1;
	if (cflag) {
		tad1 = addr1;
		ad1 = dol;
		append(getcopy, ad1++);
		ad2 = dol;
	} else {
		ad2 = addr2;
		for (ad1 = addr1; ad1 <= ad2;)
			*ad1++ &= ~01;
		ad1 = addr1;
	}
	ad2++;
	if (adt < ad1) {
		if (adt + 1 == ad1 && !cflag)
			error("That move would do nothing!");
		dot = adt + (ad2 - ad1);
		if (++adt != ad1) {
			reverse(adt, ad1);
			reverse(ad1, ad2);
			reverse(adt, ad2);
		}
	} else if (adt >= ad2) {
		dot = adt++;
		reverse(ad1, ad2);
		reverse(ad2, adt);
		reverse(ad1, adt);
	} else
		error("Move to a moved line");
	change();
	if (!inglobal) {
		if (cflag) {
			undap1 = addrt + 1;
			undap2 = undap1 + lines;
			deletenone();
		} else {
			undkind = UNDMOVE;
			undap1 = addr1;
			undap2 = addr2;
			unddel = addrt;
			unddol = dol;
		}
	}
}

void
reverse(int *a1, int *a2)
{
	register int t;

	for (;;) {
		t = *--a2;
		if (a2 <= a1)
			return;
		*a2 = *a1;
		*a1++ = t;
	}
}

static int
getcopy(void)
{
	if (tad1 > addr2)
		return (EOF);
	ex_getline(*tad1++);
	return (0);
}

static int
getPUT(void)
{
	if (tad1 > unddol)
		return (EOF);
	ex_getline(*tad1++);
	tad1++;
	return (0);
}

void
yank(void)
{

	save12();
	undkind = UNDNONE;
	killed();
}

void
PUT(void)
{
	register int cnt;

	cnt = unddol - dol;
	tad1 = dol + 1;
	append(getPUT, addr2);
	netchange(cnt);
}
