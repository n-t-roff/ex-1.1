#
/*
 * Ex - a text editor
 * Bill Joy UCB September 1977
 */

#include <string.h>
#include "ex.h"
#include "ex_tty.h"

static char *longname(char *, char *);
static char *Tgetstr(char *);

char	tspace[1024], *Tspace;
#if 0
char	ttycap[];
#endif

int	COLUMNS	= 1000;
int	LINES	= 24;

void
setterm(char *type)
{

	Setterm(type, 1);
}

void
Setterm(char *type, int printerr)
{
	char buf[1024];
	register int unknown;

	if (type[0] == 0)
		type = "un";
	unknown = 0;
	if (tgetent(buf, type) != 1) {
		unknown = printerr;
		strcpy(buf, "un|unknown:0:0:?");
	}
#if 0
	TTY = buf[0] | (buf[1] << 8);
#endif
/*
	tgetmodes(MODES);
*/
	LINES = tgetnum("li");
	if (LINES <= 0)
		LINES = 24;
	varbls[WINDOW].vvalue = varbls[WINDOW].vdefault = LINES - 1;
	varbls[SCROLL].vvalue = varbls[SCROLL].vdefault = LINES / 2;
	COLUMNS = tgetnum("co");
	if (COLUMNS <= 20)
		COLUMNS = 1000;
/*
	IT = tgetflag("it");
*/
	CA = tgetflag("ca");
	Tspace = tspace;
	CM = Tgetstr("cm");
	if (!CM || cgoto()[0] == 'O')
		CA = 0, CM = 0;
	else
		CA = 1;
	AM = tgetflag("am");
	BS = tgetflag("bs");
	OS = tgetflag("os");
	CLEAR = Tgetstr("cl");
	NDSPACE = Tgetstr("nd");
	UPLINE = Tgetstr("up");
/*
	AL = Tgetstr("al");
	DL = Tgetstr"dl");
*/
	CE = Tgetstr("ce");
/*
	CD = Tgetstr("cd");
*/
	strcpy(ttytype, longname(buf, type));
	termreset();
	if (unknown)
		error("%s: Unknown terminal type@- do \"!ttytype -n\" for a list of valid types", type);
}

static char *
longname(char *bp, char *def)
{
	register char *cp;

	while (*bp && *bp != ':' && *bp != '|')
		bp++;
	if (*bp == '|') {
		bp++;
		cp = bp;
		while (*cp && *cp != ':' && *cp != '|')
			cp++;
		*cp = 0;
		return (bp);
	}
	return (def);
}

static char *
Tgetstr(char *cp)
{

	return tgetstr(cp, &Tspace);
}
