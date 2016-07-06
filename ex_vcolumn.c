#include "ex.h"
#include "ex_tty.h"
#include "ex_vis.h"
/*
 * Ex - a text editor
 * Bill Joy UCB October, 1977
 */

/*
 * Column returns the number of
 * columns occupied by printing the
 * characters through position cp of the
 * current line.
 */
int
column(char *cp)
{

	if (cp == 0)
		cp = &linebuf[LBSIZE - 2];
	return (qcolumn(cp, 0));
}

int
qcolumn(char *lim, char *gp)
{
	int x = 0;
	void (*OO)();

	OO = Outchar;
	Outchar = &qcount;
	vcntcol = 0;
	if (lim != NIL)
		x = lim[1], lim[1] = 0;
	pline(0);
	if (lim != NIL)
		lim[1] = x;
	if (gp)
		while (*gp)
			ex_putchar(*gp++);
	Outchar = OO;
	return (vcntcol);
}

void
qcount(int c)
{

	switch (c) {
		case '\t':
			vcntcol += 8;
			vcntcol &= ~07;
			break;
		default:
			vcntcol++;
	}
}
