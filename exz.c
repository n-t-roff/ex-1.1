#include "ex.h"
#include "ex_tty.h"
/*
 * Ex - a text editor
 * Bill Joy UCB June 1977
 */

static void zop2(int, int);
static void splitit(void);

char	znoclear, zhadpr, zweight;

void
zop(char hadpr)
{
	register int c, lines, op;

	zhadpr = hadpr;
	notempty();
	znoclear = 0;
	zweight = 0;
	switch(c = op = ex_getchar()) {
		case '^':
			zweight = 1;
		case '-':
		case '+':
			while (peekchar() == op) {
				ex_getchar();
				zweight++;
			}
		case '=':
		case '.':
			c = ex_getchar();
			break;
		case EOF:
			znoclear++;
			break;
		default:
			op = 0;
			break;
	}
	if (digit(c)) {
		lines = c - '0';
		for(;;) {
			c = ex_getchar();
			if (!digit(c))
				break;
			lines *= 10;
			lines += c - '0';
		}
		if (lines < value(WINDOW))
			znoclear++;
		if (op == '=')
			lines += 2;
	} else
		lines = op == EOF ? value(SCROLL) : value(WINDOW);
	if (c != EOF) {
		ungetchar(c);
		ex_newline();
	}
	addr1 = addr2;
	setdot();
	zop2(lines, op);
}

static void
zop2(int lines, int op)
{
	register int *split;

	split = NIL;
	switch(op) {
		case EOF:
			if (addr2 == dol)
				error("\nAt EOF");
		case '+':
			if (addr2 == dol)
				error("At EOF");
			addr2 += lines * zweight;
			if (addr2 > dol)
				error("Hit BOTTOM");
			addr2++;
		default:
			addr1 = addr2;
			addr2 += lines-1;
			dot = addr2;
			break;
		case '=':
		case '.':
			lines--;
			lines >>= 1;
			if (op == '=')
				lines--;
			addr1 = addr2-lines;
			if (op == '=')
				dot = split = addr2;
			addr2 += lines;
			if (op == '.') {
				markDOT();
				dot = addr2;
			}
			break;
		case '^':
		case '-':
			addr2 -= lines * zweight;
			if (addr2 < one)
				error("Hit TOP");
			lines--;
			addr1 = addr2 - lines;
			dot = addr2;
			break;
	}
	if (addr1 <= zero)
		addr1 = one;
	if (addr2 > dol)
		addr2 = dol;
	if (dot > dol)
		dot = dol;
	if (addr1 > addr2)
		return;
	if (addr2 - addr1 > 1 /* && really wanna do this */)
		pstart();
	if (op == EOF && zhadpr) {
		ex_getline(*addr1);
		ex_putchar('\r' | QUOTE);
		shudclob = 1;
	} else if (znoclear == 0 && CLEAR)
		ex_putchar('\032' | QUOTE);
	if (split) {
		plines(addr1, split - 1, 0);
		splitit();
		plines(split, split, 0);
		splitit();
		addr1 = split + 1;
	}
	plines(addr1, addr2, 0);
}

static void
splitit(void)
{
	register int l;
	register char *lp;

	for (l = COLUMNS == 1000 ? 72 : COLUMNS, lp = linebuf; l > 0; l--)
		*lp++ = '-';
	*lp = 0;
	ex_printf("%s\n", linebuf);
}

void
zeq(int cnt)
{

	znoclear = 0;
	zop2(cnt, '=');
	putNFL();
}
