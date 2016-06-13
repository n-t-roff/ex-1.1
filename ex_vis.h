/*
 * Ex - a text editor
 * Bill Joy UCB June 1977
 */

#define	FS	CTRL('\\')
#define	NL	CTRL('j')
#define	CR	CTRL('m')

#define	INF	30000

#define	DELETE	0177
#define	ESCAPE	033

#ifdef VISUAL
char	visual;
char	*cursor, *wcursor, *tcursor;
int	dir;

/* ???
int	vlast;
*/

#define vputc(c)	putc(c, &obuf)

#define	TUBELINES	100
#define	TUBECOLS	300
#define	TUBESIZE	(TUBELINES * TUBECOLS)

int	vliny[TUBELINES + 2];
int	vcline, vcnt;

#define	ESCAPE	033

char	vch, *vtube[TUBELINES], *vtube0;

int	VLINES, VCOLUMNS;

#define	vlast	vliny[vcnt]

char vmoving;
char vmovcol;

#endif

void	vdelete(int);
int	(*Outchar)(), qcount();
void	vputchar(int);
void	vgoto(int, int);
void	vsync(int);
void	vclreol(void);
void	operate(int, int);
void	addtext(char *);
void	killglob(void);
void	setwork(char);
void	setLAST(void);

#ifdef VISUAL

char	vholdmove;

#define	beep	obeep

int	vcntcol;

char	holdat, splitw, hadup, *nextic;

#define	VNONE	0
#define	VCHNG	1
#define	VMANY	2

char	vundkind, *vutmp;
int	vulines[TUBELINES], vylines[TUBELINES], *vresaddr;
int	vrestore(), vrescnt, vresCNT, vdelcnt, vrescurs, vyancnt;
char	wdkind;
int	vSCROLL;

int	(*Pline)();
void termchar(int);
void numbline(int);
void normline(void);
#define	BUFSIZE	128

char	Xhadcnt;
int	Xcnt;

char	lasthad;
int	lastcnt;

char	lastcmd[5], *lastcp, workcmd[5];
char	DEL[BUFSIZE], INS[BUFSIZE];

#define	OVERBUF	CR

char	*vglobp;

char	HADUP, HADZERO;
int	CDCNT;

int	ZERO;

char	*ogcursor;

void beep(void);
void vgrabit(void);
void vyankit(void);
void vchange(void);
void vmove(void);
#endif
