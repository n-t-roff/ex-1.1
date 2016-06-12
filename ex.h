#include <setjmp.h>
#include <termios.h>
#define	VISUAL
/*
 * Ex - text editor
 * Bill Joy UCB June 1977
 *
 * Based on an earlier editor "ex" written by
 * William Joy and Charles Haley.
 *
 * And of course an inestimable debt to "ed"!
 */

#define	STATIC static

#define	NIL	0
#define	QUOTE	0200

#define	HUP	1
#define	INTR	2
#define	QUIT	3

#define	FNSIZE	64
#define	LBSIZE	512

#define	EOF	-1

char	ruptible, inglobal, inopen, inconf, listf, endline, laste, intty;
char	shudclob, diddle, die;

int	chngflag, xchngflag, tchngflag;

char	/* savedfile[FNS, */ file[FNSIZE], altfile[FNSIZE];
char	linebuf[LBSIZE], genbuf[LBSIZE];

int	*address(), *addr1, *addr2;
int	*zero, *one, *dot, *dol, *unddol, *endcore, *fendcore;
int	*unddel, *undap1, *undap2, *undadot;
char	undkind;

#define	UNDCHANGE	0
#define	UNDMOVE		1
#define	UNDALL		2
#define	UNDNONE		3

int	io, erfile, tfile;
char	*globp, *erpath;
int	names[27];
int	outcol;
char	home[30];
char	*Command;

int	getfile(), gettty(), getchar(), getsub();


#include "ex_vars.h"

#define	eq(a, b)	(strcmp(a, b) == 0)

#ifndef CTRL
#define	CTRL(c)	(c & 037)
#endif

#ifndef ECHO
#define	ECHO	010
#endif
#define	RAW	040

char	normtty;
struct termios	normf;
struct {
	int	fildes;
	int	nunused;
	char	*xfree;
	char	buff[512];
} obuf;
int	oldhup, onhup(), oldquit, onintr();

struct {
	int	Atime[2];
	int	Auid;
	int	Alines;
	int	Afname[FNSIZE];
	int	Ablocks[100];
} header;

#define	savedfile	header.Afname
#define	blocks		header.Ablocks

int	dirtcnt;

char	recov;

extern char	TTYNAM[];
int	TMODE;

int	lastc, peekc;
jmp_buf	resetlab;		/* For error throws to top level (cmd mode) */
#define	getexit(a)	copy(a, resetlab, sizeof (jmp_buf))
#define	lastchar()	lastc
#define	outchar(c)	(*Outchar)(c)
#define	pline(no)	(*Pline)(no)
#define	reset()		longjmp(resetlab,1)
#define	resexit(a)	copy(resetlab, a, sizeof (jmp_buf))
#define	setexit()	setjmp(resetlab)
#define	setlastchar(c)	lastc = c
#define	ungetchar(c)	peekc = c

char	aiflag;
#define	setai(i)	aiflag = i

int	pid, rpid, status;
struct termios tty;
char	allredraw, pfast;
int	mask, vcntcol;

extern int	(*Outchar)();
extern int	(*Pline)();
extern int	(*Putchar)();

void listchar(int);
void normchar(int);
void tabulate(char);
void xpand(void);
void lprintf(char *, ...);
int (*setlist(void))();
int (*setnorm(void))();
void setoutt(void);
void ex_newline(void);
void pstop(void);
void pstart(void);
void fgoto(void);
char *cgoto(void);
