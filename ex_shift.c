#
/*
 * Ex - a text editor
 * Bill Joy UCB October 1977
 */
#include <string.h>
#include "ex.h"

void
shift(int c, int cnt)
{
	register int *addr;
	char *cp = genbuf;
	char *dp;
	register int i;

	if (!inglobal)
		save12(), undkind = UNDCHANGE;
	cnt *= value(SHIFTWIDTH);
	for (addr = addr1; addr <= addr2; addr++) {
		ex_getline(*addr);
		i = whitecnt(linebuf);
		switch (c) {
			case '>':
				if (linebuf[0] == 0)
					continue;
				cp = genindent(i + cnt);
				break;
			case '<':
				if (i == 0)
					continue;
				i -= cnt;
				cp = i > 0 ? genindent(i) : genbuf;
				break;
		}
		dot = addr;
		if (cp + strlen(dp = vpastwh(linebuf)) >= &genbuf[LBSIZE - 2])
			error("Line too long|Result line after shift would be too long");
		strcpy(cp, dp);
		strcLIN(genbuf);
		putmark(addr);
	}
}
