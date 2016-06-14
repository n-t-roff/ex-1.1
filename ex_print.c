/*
 * Ex - a text editor
 * Bill Joy UCB June 1977
 */

#include <stdarg.h>
#include "ex.h"
#include "ex_tty.h"
#include "ex_vis.h"

static void slobber(int);

/* Put in assembly language to save space */
int (*
setlist(void))()
{
	register int (*P)();

	P = Putchar;
	Putchar = &listchar;
	return (P);
}

int (*
setnorm(void))()
{
	register int (*P)();

	P = Putchar;
	Putchar = &normchar;
	return (P);
}

void
listchar(int c)
{

	c &= 0377;
	switch (c) {
		case '\t':
			c = '>';
			goto escit;
		case '\b':
			c = '<';
escit:
			if (!OS)
				break;
			outchar('-');
			outchar('\b');
			break;
		case '\n':
			break;
		case '\n' | QUOTE:
			outchar('$');
			break;
		default:
			if (c & QUOTE)
				break;
			if (c < ' ' && c != '\n' || c == 0177) {
				outchar('\\');
				if (c == 0177) {
					outchar('1');
					c = 077;
				}
				outchar((c >> 3) + '0');
				c = (c & 07) + '0';
			}
			break;
	}
	normchar(c);
}

void
normchar(int c)
{
	register char *colp;

	c &= 0377;
	if (c & QUOTE)
		switch (c) {
			case ' ' | QUOTE:
			case '\b' | QUOTE:
				break;
			case QUOTE:
				return;
			default:
				c &= 0177;
		}
	if (UPPERCASE) {
		if (ucletter(c)) {
			outchar('\\');
			c = letter(c);
		} else {
			colp = "({)}!|^~'`";
			while (*colp++)
				if (c == *colp++) {
					outchar('\\');
					c = colp[-2];
					break;
				}
		}
	}
	outchar(c);
}

void
numbline(int i)
{

	if (shudclob)
		slobber(' ');
	ex_printf("%6d  ", i);
	normline();
}

void
normline(void)
{

	if (shudclob)
		slobber(linebuf[0]);
	if (Putchar == &listchar || value(PRINTALL))
		ex_printf("%s", linebuf);
	else {
		if (value(INDICATEUL) && !inopen && !inconf)
			doulg();
		dographic();
		if (value(INDICATEUL) && !inopen && !inconf && genbuf[0]) {
			outchar('\n');
			if (Pline == numbline)
				outchar('\t');
			ex_printf("%s", genbuf);
		}
	}
	if (!inopen && !inconf)
		putchar('\n' | QUOTE);
}

static void
slobber(int c)
{

	shudclob = 0;
	switch (c) {
		case '\t':
			if (Putchar == &listchar)
				return;
			break;
		default:
			return;
		case ' ':
		case 0:
			break;
	}
	if (OS)
		return;
	ex_printf("\240\210");
}

void
setoutt(void)
{

	Outchar = &termchar;
}

void
lprintf(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	lvprintf(fmt, ap);
}

void
lvprintf(char *fmt, va_list ap)
{
	int (*P)();

	P = setlist();
	ex_vprintf(fmt, ap);
	Putchar = P;
}
