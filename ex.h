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
#define	CHAR

#define	NIL	0
#define	QUOTE	0200

#define	HUP	1
#define	INTR	2
#define	QUIT	3

#define	FNSIZE	64
#define	LBSIZE	512

#define	EOF	-1

extern
char	ruptible, inglobal, inopen, inconf, listf, endline, laste, intty;
extern
char	shudclob, diddle, die;

extern
int	chngflag, xchngflag, tchngflag;

extern
char	/* savedfile[FNS, */ file[FNSIZE], altfile[FNSIZE];
extern
char	linebuf[LBSIZE], genbuf[LBSIZE];

extern
int	*address(), *addr1, *addr2;
extern
int	*zero, *one, *dot, *dol, *unddol, *endcore, *fendcore;
extern
int	*unddel, *undap1, *undap2, *undadot;
extern
char	undkind;

#define	UNDCHANGE	0
#define	UNDMOVE		1
#define	UNDALL		2
#define	UNDNONE		3

extern
int	io, erfile, tfile;
extern
char	*globp, *erpath;
extern
int	names[27];
extern
int	outcol;
extern
char	home[30];
extern
char	*Command;

int	getfile(), gettty(), getchar(), getsub();


#include "ex_vars.h"

#define	eq(a, b)	(strcmp(a, b) == 0)

#ifndef CTRL
#define	CTRL(c)	('c' & 037)
#endif

#ifndef ECHO
#define	ECHO	010
#endif
#define	RAW	040

extern
char	normtty;
extern
int	normf;
struct obuf {
	int	fildes;
	int	nunused;
	char	*xfree;
	char	buff[512];
};
extern struct obuf obuf;
extern
int	oldhup, onhup(), oldquit, onintr();

struct header {
	int	Atime[2];
	int	Auid;
	int	Alines;
	int	Afname[FNSIZE];
	int	Ablocks[100];
};
extern struct header header;

#define	savedfile	header.Afname
#define	blocks		header.Ablocks

extern
int	dirtcnt;

extern
char	recov;

extern
char	TTYNAM[];
extern
int	TMODE;

extern
int	lastc, peekc;
#define	lastchar()	lastc
#define	setlastchar(c)	lastc = c
#define	ungetchar(c)	peekc = c

extern
char	aiflag;
#define	setai(i)	aiflag = i

extern
int	pid, rpid, status, tty[3];
extern
char	allredraw, pfast;
extern
int	mask, vcntcol;
