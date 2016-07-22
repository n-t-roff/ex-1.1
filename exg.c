#include "ex.h"
#include "ex_re.h"
/*
 * Ex - a text editor
 * Bill Joy UCB June 1977
 *
 * Should get a < n square algorithm here.
 */

#define	GBSIZE	256

static void save(int *, int *);

void
global(int k)
{
	register char *gp;
	int c;
	register int *a1;
	char globuf[GBSIZE], *Cwas;
	int i;

	i = dol - zero;
	Cwas = Command;
	if (inglobal)
		error("Global within global@not allowed");
	markDOT();
	setall();
	nonzero();
	skipwh();
	c = ex_getchar();
	if (endcmd(c))
		error("Global needs re|Missing regular expression for global");
	compile(c, 0);
	savere(&scanre);
	gp = globuf;
	while ((c = ex_getchar()) != '\n') {
		switch (c) {
			case EOF:
				error("EOF reading global");
			case '\\':
				c = ex_getchar();
				switch (c) {
					case '\\':
						ungetchar(c);
						break;
					case '\n':
						break;
					default:
						*gp++ = '\\';
						break;
				}
				break;
		}
		*gp++ = c;
		if (gp >= &globuf[GBSIZE - 2])
			error("Global command too long@- limit 254 characters");
	}
	ungetchar(c);
	ex_newline();
	*gp++ = c;
	*gp++ = 0;
	inglobal = 1;
	for (a1 = one; a1 <= dol; a1++) {
		*a1 &= ~01;
		if (a1 >= addr1 && a1 <= addr2 && execute(0, a1) == k)
			*a1 |= 01;
	}
	saveall();
	for (a1 = one; a1 <= dol; a1++) {
		if (*a1 & 01) {
			*a1 &= ~01;
			dot = a1;
			globp = globuf;
			commands(1, 1);
			a1 = zero;
		}
	}
	inglobal = 0;
	endline = 1;
	Command = Cwas;
	netchHAD(i);
	setlastchar(EOF);
}

static void
save(int *a1, int *a2)
{
	register int *addr, *dest;

	undkind = UNDNONE;
	undadot = dot;
	for (dest = dol + 1, addr = a1; addr <= a2; *dest++ = *addr++)
		if (dest >= endcore) {
#ifdef UNIX_SBRK
			if (sbrk(1024) == -1)
#endif
				error("Out of memory@saving lines for undo - try using ed or re)");
#ifdef UNIX_SBRK
			endcore += 1024;
#endif
		}
	undkind = UNDALL;
	unddel = a1 - 1;
	undap1 = a1;
	undap2 = a2 + 1;
	unddol = dest - 1;
}

void
save12(void)
{

	save(addr1, addr2);
}

void
saveall(void)
{

	save(one, dol);
}
