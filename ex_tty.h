/*
 * Ex - a text editor
 * Bill Joy UCB September 1977
 */

#if 0
int	TTY;
#endif

extern
char	*CLEAR, *NDSPACE, *UPLINE, *HSTR, *CE;
extern
char	*CM;

extern
char	CA, AM, BS, OS, PT, NOCR;
extern
int	LINES, COLUMNS;

extern
char	UPPERCASE;

extern
char	ttytype[];

extern
int	outcol, outline, destcol, destline;

/*
 * As yet unused capabilities...
 *
char	*AL, *DL, *CD;
char	IT;
int	MODES[2];
 */
