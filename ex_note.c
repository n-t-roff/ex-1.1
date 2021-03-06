#
/*
 * Ex - a text editor
 * Bill Joy UCB September, 1977
 */

#include "ex.h"
#include "ex_glob.h"

static int
notable(int i)
{

	return (value(HUSH) == 0 && !inglobal && i > value(NOTIFY));
}

void
killed(void)
{
	register int cnt;

	cnt = addr2 - addr1 + 1;
	if (!notable(cnt))
		return;
	ex_printf("%d lines", cnt);
	if (value(TERSE) == 0) {
		ex_printf(" %c%s", Command[0] | ' ', Command + 1);
		if (Command[strlen(Command) - 1] != 'e')
			ex_putchar('e');
		ex_putchar('d');
	}
	putNFL();
}

void
netchHAD(int cnt)
{

	netchange((dol - zero) - cnt);
}

void
netchange(int i)
{
	register char *cp;

	if (i > 0)
		cp = "more";
	else
		cp = "less", i = -i;
	if (!notable(i))
		return;
	ex_printf(mesg("%d %s lines@in file after %s"), i, cp, Command);
	putNFL();
}

void
snote(int total, int lines)
{

	if (!notable(total))
		return;
	ex_printf(mesg("%d subs|%d substitutions"), total);
	if (lines != 1 && lines != total)
		ex_printf(" on %d lines", lines);
	putNFL();
}

void
noteargs(void)
{
	if (argc > 1) {
		ex_printf(mesg("%d files@to edit"), argc);
		putNFL();
	}
}
