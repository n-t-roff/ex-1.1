#include <time.h>
#include <stdarg.h>
#include <setjmp.h>
#include <termios.h>

int tgetent(char *bp, const char *name);
int tgetflag(char *id);
int tgetnum(char *id);
char *tgetstr(char *id, char **area);
char *tgoto(const char *cap, int col, int row);
int tputs(const char *str, int affcnt, int (*putc)(int));

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

#ifndef EOF
#define	EOF	-1
#endif

char	ruptible, inglobal, inopen, inconf, listf, endline, laste, intty;
char	shudclob, diddle, die;

int	chngflag, xchngflag, tchngflag;

char	/* savedfile[FNS, */ file[FNSIZE], altfile[FNSIZE];
char	linebuf[LBSIZE], genbuf[LBSIZE];

int	*address(void);
int	*addr1, *addr2;
int	*zero, *one, *dot, *dol, *unddol, *endcore, *fendcore;
int	*unddel, *undap1, *undap2, *undadot;
char	undkind;

#define	UNDCHANGE	0
#define	UNDMOVE		1
#define	UNDALL		2
#define	UNDNONE		3

int	io, erfile;
extern	int tfile;
char	*globp;
extern	char *erpath;
int	names[27];
int	outcol;
char	home[30];
char	*Command;

int	getfile(void);

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
void	(*oldhup)(int);
void	onhup(void);
void	(*oldquit)(int);
void	onintr(int);

struct {
	time_t	Atime;
	int	Auid;
	int	Alines;
	char	Afname[FNSIZE];
	int	Ablocks[100];
} header;

#define	savedfile	header.Afname
#define	blocks		header.Ablocks

int	dirtcnt;

char	recov;

extern char	TTYNAM[];
int	TMODE;

extern int	lastc;
int	peekc;
jmp_buf	resetlab;		/* For error throws to top level (cmd mode) */
#define	getexit(a)	copy((char *)a, (char *)resetlab, sizeof (jmp_buf))
#define	lastchar()	lastc
#define	outchar(c)	(*Outchar)(c)
#define	pline(no)	(*Pline)(no)
#define	reset()		longjmp(resetlab,1)
#define	resexit(a)	copy((char *)resetlab, (char *)a, sizeof (jmp_buf))
#define	setexit()	setjmp(resetlab)
#define	setlastchar(c)	lastc = c
#define	ungetchar(c)	peekc = c

char	aiflag;
#define	setai(i)	aiflag = i

int	pid, rpid, status;
struct termios tty;
char	allredraw, pfast;
int	mask;

extern void (*Putchar)();

void listchar(int);
void normchar(int);
void tabulate(char);
void xpand(void);
void error(char *, ...);
void lprintf(char *, ...);
void lvprintf(char *, va_list);
void ex_printf(char *, ...);
void ex_vprintf(char *, va_list);
void (*setlist(void))();
void (*setnorm(void))();
void setoutt(void);
void ex_newline(void);
void pstop(void);
void pstart(void);
void fgoto(void);
char *cgoto(void);
void putfile(void);
void set(int);
void initoptions(char);
void source(char *, int);
void setall(void);
void setcount(void);
void snote(int, int);
void netchange(int);
void killed(void);
void vappend(int, int, int);
void reverse(int *, int *);
void filename(int);
void ex_sync(void);
int commands(int, char);
void change(void);
void plines(int *, int *, char);
void eol(void);
void resetflav(void);
int endcmd(int);
void fileinit(void);
void cleanup(void);
void clrstats(void);
int iostats(void);
char *plural(int);
char *ex_getline(int);
int putline(void);
void ioerror(void);
void synctmp(void);
void TSYNC(void);
void putch(int);
void rop(int);
void rop2(void);
void rop3(int);
int append(int (*)(), int *);
void appendnone(void);
int substitute(char);
void delete(void);
void deletenone(void);
void join(char);
void setdot(void);
void setdot1(void);
int getnum(void);
void markDOT(void);
void markpr(int *);
void notempty(void);
void setnoaddr(void);
void nonzero(void);
void setCNL(void);
void setNAEOL(void);
int *scanfor(int);
int gTTY(int);
int sTTY(int);
void netchHAD(int);
void noteargs(void);
void move(void);
void move1(char, int *);
void yank(void);
void PUT(void);
void undo(char);
void helpinit(void);
void dingdong(void);
char *mesg(char *);
void normal(void);
void helpthem(void);
void ex_putchar(int);
void flush(void);
void setcol(int);
void partinp(void);
void noteinp(void);
void notech(int);
void putNFL(void);
void termreset(void);
void draino(void);
void ostart(void);
void ostop(void);
void flusho(void);
void putnl(void);
void wop(void);
int skipwh(void);
int letter(int);
int white(int);
int digit(int);
int ucletter(int);
int getchar(void);
int peekchar(void);
int gettty(void);
int backtab(int);
int *setin(int *);
int getsub(void);
int getch(void);
void putmark(int *);
void putmk1(int *, int);
void xop(int (*)(), int);
void global(int);
void save12(void);
void saveall(void);
void zop(char);
void zeq(int);
void xtrey(char);
void setDEL(void);
void setBUF(char *);
void oop(void);
char *vskipwh(char *);
void voinit(void);
void vop(void);
void undiddle(void);
int compile(int, char);
int execute(int, int *);
void copy(char *, char *, int);
char *strend(char *);
char *strcLIN(char *);
int column(char *);
int qcolumn(char *, char *);
char *genindent(int);
char *vpastwh(char *);
int whitecnt(char *);
char *vgetline(int, char *, char *);
void vdoappend(char *);
void shift(int, int);
void dographic(void);
void doulg(void);
int getargs(void);
char *unpack(char *);
void pack(char *);
void ex_unix(void);
void unix2(char *, char, char *);
void recover(void);
void waitfor(void);
void revocer(void);
int preserve(void);
void REset(void);
