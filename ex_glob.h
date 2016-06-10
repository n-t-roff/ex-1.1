/*
 * Ex - a text editor
 * Bill Joy UCB
 * Version 1.0 September, 1977
 */

struct	Glob {
	char	Ab[522];
	char	*Ava[200];
};
extern struct	Glob *G;

extern
char	**xargv0, **xargv, **argv;
extern
int	xargc0, xargc, argc;

extern
int	gargc;
