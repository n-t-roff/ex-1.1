#
/*
 * Ex - a text editor
 * Bill Joy UCB September, 1977
 */

#include <unistd.h>
#include <sys/types.h>
#include "ex.h"
#include "ex_tty.h"
#include "ex_io.h"


#define	UCASE	04
#define	TABS	02
#define	CRFLG	020

char	TTYNAM[]	= "/dev/ttyx";

initoptions(f)
	char f;
{
	int ttyno, rcio;
	char Home[40];
	register char *H;

	intty = isatty(0);
	if (!intty && f == 0) {
unknown:
		setterm("u");
		return;
	}
	if (gTTY(1)) {
		if (f == 0 || gTTY(2))
			goto unknown;
		f = 2;
	} else
		f = 1;
#ifdef IUCLC
	UPPERCASE = (tty.c_iflag & IUCLC) != 0;
#endif
#ifdef TAB3
	PT = (tty.c_oflag & TABDLY) != TAB3;
#endif
	NOCR = (tty.c_oflag & ONLCR) == 0;
	H = getenv("HOME");
	strcpy(Home, H);
	strcat(Home, "/.exrc");
	source(Home, 1);
}

isetterm(type)
	char *type;
{
	char reenter;
	int osetexit[3];

	getexit(osetexit);
	reenter = 0;
	setexit();
	if (reenter == 0) {
		reenter++;
		Setterm(type, 0);
	}
	resexit(osetexit);
}

gettmode()
{
	struct stb stbuf;

	stat(TTYNAM, &stbuf);
	TMODE = stbuf.flags & 0777;
}
