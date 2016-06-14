/*
 * Ex - a text editor
 * Bill Joy UCB
 * Version 1.0 September, 1977
 */

struct	Glob {
	char	Ab[522];
	char	*Ava[200];
} *G;

char	**xargv0, **xargv, **argv;
int	xargc0, xargc, argc;

int	gargc;

void glob(char **, struct Glob *);
void getone(void);
void filioerr(char *);
int any(int, char *);
