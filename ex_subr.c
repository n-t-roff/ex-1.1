/*
 * Ex - a text editor
 * Bill Joy UCB September 1977
 */

#include "ex.h"

int
skipwh(void)
{
	register int wh;

	wh = 0;
	while (white(peekchar())) {
		wh++;
		getchar();
	}
	return (wh);
}

#if 0
strcmp(left, right)
	register char *left, *right;
{

	while (*left == *right++)
		if (*left++ == 0)
			return (0);
	return (*left - *--right);
}
#endif

int
letter(int c)
{

	if (c >= 'a' && c <= 'z')
		return (c);
	if (c >= 'A' && c <= 'Z')
		return (c + 'a' - 'A');
	return (0);
}

#if 0
strlen(cp)
	register char *cp;
{
	register int i;

	i = 0;
	while (*cp++)
		i++;
	return (i);
}
#endif

int
white(int c)
{

	return (c == ' ' || c == '\t');
}

int
digit(int c)
{
	return (c >= '0' && c <= '9');
}

int
ucletter(int c)
{
	return (c >= 'A' && c <= 'Z');
}
