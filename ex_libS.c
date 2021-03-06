#
/*
 * Ex - a text editor
 * Bill Joy UCB September, 1977
 *
 * We use these routines here instead of libS.a because
 * we need a printf which prints through putchar().
 */

#include "ex.h"

#if 0
strcpy(oto, from)
	char *oto;
	register char *from;
{
	register char *to;

	to = oto;
	while (*to++ = *from++)
		continue;
	return (oto);
}

strcat(to, from)
	char *to, *from;
{

	strcpy(strend(to), from);
	return (to);
}
#endif

char *
strend(char *cp)
{

	while (*cp)
		cp++;
	return (cp);
}

char *
strcLIN(char *dp)
{

	return CP(linebuf, dp);
}
