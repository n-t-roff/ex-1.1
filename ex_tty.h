/*
 * Ex - a text editor
 * Bill Joy UCB September 1977
 */

#if 0
int	TTY;
#endif

char	*CLEAR, *NDSPACE, *UPLINE, *HSTR, *CE;
char	*CM;

char	CA, AM, BS, OS, PT, NOCR;
extern	int LINES, COLUMNS;

char	UPPERCASE;

extern char	ttytype[];

int	outcol, outline, destcol, destline;

/*
 * As yet unused capabilities...
 *
char	*AL, *DL, *CD;
char	IT;
int	MODES[2];
 */

void setterm(char *);
void Setterm(char *, int);
