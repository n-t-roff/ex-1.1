#include <unistd.h>
#include "ex.h"
#ifdef VISUAL
#include "ex_tty.h"
#include "ex_vis.h"
/*
 * Ex - a text editor
 * Bill Joy UCB September 1977
 */

static int getbr(void);

char	Peekkey;

void
ungetkey(int c)
{

	Peekkey = c;
}

int
getkey(void)
{
	int c;

	do
		c = getbr();
	while (c == 0);
	return (c);
}

int
peekbr(void)
{

	Peekkey = getbr();
	return (Peekkey == 0);
}

static int
getbr(void)
{
	int c;

	if (Peekkey) {
		c = Peekkey;
		Peekkey = 0;
		return (c);
	}
	if (vglobp) {
		if (*vglobp)
			return (*vglobp++);
		return (ESCAPE);
	}
	flusho();
	if (read(0, &c, 1) != 1)
		error("Input read error");
	c &= 0177;
	return (c);
}

int
getesc(void)
{
	register int c;

	c = getkey();
	switch (c) {
		case DELETE:
		case FS:
			ungetkey(c);
		case ESCAPE:
			return (0);
	}
	return (c);
}

int
peekkey(void)
{

	Peekkey = getkey();
	return (Peekkey);
}
#endif
