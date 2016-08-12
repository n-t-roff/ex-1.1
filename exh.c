#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "ex.h"
/*
 * Ex - a text editor
 * Bill Joy UCB June 1977
 */

void
helpinit(void)
{

#if 0
	erfile = open(erpath, 0);
	if (erfile < 0)
		flush();
	else
		pstop();
#endif
}

void
error(char *fmt, ...)
{
	va_list ap;

	if (pfast)
		pfast = 1;
	flush();
	resetflav();
	if (laste) {
		laste = 0;
		ex_sync();
	}
	dingdong();
	if (inopen) {
		termreset();
		putnl();
	}
	inopen = 0;
	inconf = 0;
	fmt = mesg(fmt);
	va_start(ap, fmt);
	lvprintf(fmt, ap);
	putNFL();
	if (die)
		exit(1);
	undiddle();
	lseek(0, 0, SEEK_END);
	if (inglobal)
		setlastchar('\n');
	inglobal = 0;
	globp = 0;
	while (lastchar() != '\n' && lastchar() != EOF)
		ex_getchar();
	ungetchar(0);
	endline = 1;
	if (io > 0) {
		close(io);
		io = -1;
	}
	reset();
}

void
dingdong(void)
{

	if (value(ERRBELLS))
		ex_printf("\207\207");
}

/*
 * Mesg decodes the terse/verbose strings. Thus
 *	'xxx@yyy' -> 'xxx' if terse, else 'xxx yyy'
 *	'xxx|yyy' -> 'xxx' if terse, else 'yyy'
 * All others map to themselves.
 */
char *
mesg(char *str)
{
	register char *cp;

	str = strcpy(genbuf, str);
	for (cp = str; *cp; cp++)
		switch (*cp) {
			case '@':
				if (value(TERSE))
					*cp = 0;
				else
					*cp = ' ';
				break;
			case '|':
				if (value(TERSE) == 0)
					return (cp + 1);
				*cp = 0;
				break;
		}
	return (str);
}

void
normal(void)
{

	if (normtty) {
		gTTY(1);
		tty = normf;
		sTTY(1);
	}
	normtty = 0;
}

void
helpthem(void)
{
	register char *cp, *icp;
	char buff[513];
	register int buffcnt;

	strcpy(buff, SHAREDIR "/how_ex/");
	icp = strend(buff);
	skipwh();
	if (endcmd(peekchar()))
		strcat(buff, "help");
	else {
		cp = icp;
		while (!white(peekchar()) && !endcmd(peekchar()))
			*cp++ = ex_getchar();
		*cp++ = 0;
		skipwh();
		if (!endcmd(peekchar()))
			error("Help takes one keyword only as argument@- \"help index\" gives a list of keywords");
	}
	eol();
	io = open(buff, O_RDONLY);
	if (io < 0)
		error("Don't know anything about %s@- \"help index\" gives a list of known subjects", icp);
	setnorm();
	for(;;) {
		buffcnt = read(io, buff, 512);
		if (buffcnt <= 0)
			break;
		buff[buffcnt] = 0;
		ex_printf("%s", buff);
	}
	flush();
}
