#include "ex.h"
/*
 * Ex - a text editor
 * Bill Joy UCB June 1977
 */

int
append(int (*f)(), int *a)
{
	register int *a1, *a2, *rdot;
	int nline;

	nline = 0;
	dot = a;
	if (!inglobal && !inopen && f != getsub) {
		undap1 = undap2 = dot + 1;
		undkind = UNDCHANGE;
	}
	while ((*f)() == 0) {
		if (unddol >= endcore) {
#ifdef UNIX_SBRK
			if (sbrk(1024) == -1) {
#endif
				if (!inglobal && f == getsub) {
					undap1 = addr1;
					undap2 = addr2 + 1;
				}
				error("Out of memory@- too many lines in file");
#ifdef UNIX_SBRK
			}
#endif
			endcore += 1024;
		}
		nline++;
		a1 = unddol + 1;
		a2 = a1 + 1;
		dot++;
		undap2++;
		dol++;
		unddol++;
		for (rdot = dot; a1 > rdot;)
			*--a2 = *--a1;
		*rdot = 0;
		putmark(rdot);
		if (f == &gettty) {
			dirtcnt++;
			TSYNC();
		}
	}
	return (nline);
}

void
appendnone(void)
{
	if (!inglobal) {
		undkind = UNDCHANGE;
		undap1 = undap2 = addr1;
	}
}
