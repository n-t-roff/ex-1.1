#include <termios.h>
#include <unistd.h>
#include "ex.h"
#include "ex_tty.h"
#include "ex_vis.h"
/*
 * Ex - a text editor
 * Bill Joy UCB June 1977
 */

static void putS(char *);
static void plod(void);
static void flush1(void);
static void flush2(void);
static int motion(void);
static void ttcharoff(void);

STATIC	char line[66] = "Error message file not available\n/usr/lib/ex1.1strings";
STATIC	char *linp = line;
STATIC	char phadnl;
void	(*Outchar)() = termchar;
void	(*Putchar)() = normchar;
void	(*Pline)() = normline;

void
setnumb(void)
{
	Pline = numbline;
}

/*
 * Indirect to current definition of putchar.
 */
void
ex_putchar(int c)
{

	(*Putchar)(c);
}

void
termchar(int c)
{

	if (pfast == 0 && phadnl)
		pstart();
	if (c == '\n')
		phadnl = 1;
	else if (linp >= &line[63])
		flush1();
	*linp++ = c;
	if (c == '\n' && pfast == 0) {
		fgoto();
		flush1();
	}
}

void
flush(void)
{

	flush1();
	flush2();
}

static void
flush1(void)
{
	register char *lp;
	int c;

	*linp = 0;
	lp = line;
	while (*lp)
		switch (c = *lp++) {
			case '\r':
				destline += destcol / COLUMNS;
				destcol = 0;
				continue;
			case '\013':
				destline--;
				continue;
			case '\b':
				if (value(PRINTALL))
					goto printit;
				if (destcol)
					destcol--;
				continue;
			case '\032':
				if (CLEAR) {
					putS(CLEAR);
					outcol = 0;
					outline = 0;
					destcol = 0;
					destline = 0;
				}
				continue;
			case ' ':
				if (value(PRINTALL))
					goto printit;
				destcol++;
				continue;
			case '\t':
				if (value(PRINTALL))
					goto printit;
				destcol += 8;
				destcol &= ~7;
				continue;
			case '\n':
				destline += destcol / COLUMNS + 1;
				if (destcol != 0 && destcol % COLUMNS == 0)
					destline--;
				destcol = 0;
				continue;
			default:
printit:
				fgoto();
				for (;;) {
					if (AM == 0 && outcol == COLUMNS)
						fgoto();
					c &= 0177;
					putch(c);
					if (c == '\b') {
						outcol--;
						destcol--;
					} else if (c >= ' ' && c != 0177) {
						outcol++;
						destcol++;
					}
					c = *lp++;
					if (c <= ' ')
						break;
				}
				--lp;
				continue;
		}
	linp = line;
}

static void
putS(char *cp)
{

	if (cp == NIL)
		return;
	while (*cp)
		putch(*cp++);
}


static void
flush2(void)
{

	fgoto();
	flusho();
	pstop();
}

/*
 * Goto destline, destcol from outline, outcol.
 * We must be cognizant of the capabilities of the terminal
 * and (if its a crt) the possibly finite size of its screen.
 * The following capability variables are considered:
 *
 *	NOCR	if true, can't CR
 *	BS	if true, can BS
 *	CA	if true, can cursor address
 *	NDSPACE	non-destructive space sequence (cursor right)
 *	UPLINE	up line sequence (reverse line-feed)
 *	AM	if true, terminal has automatic return at right margin
 */
void
fgoto(void)
{
	register int l, c;

	/*
	 * Adjust the destination column to be
	 * non-virtual by performing line folding.
	 */
	if (destcol > COLUMNS - 1) {
		destline += destcol / COLUMNS;
		destcol %= COLUMNS;
	}
	if (outcol > COLUMNS - 1) {
		l = (outcol + 1) / COLUMNS;
		outline += l;
		outcol %= COLUMNS;
		if (AM == 0) {
			/*
			 * Software automatic margins
			 */
			while (l > 0) {
				putch('\n');
				if (pfast)
					putch('\r');
				l--;
			}
			outcol = 0;
		}
		/*
		 * Outline has to be a real line...
		 * rounding outline and destline down corresponds
		 * to "rolling" the screen up.
		 */
		if (outline > LINES - 1) {
			destline -= outline - (LINES - 1);
			outline = LINES - 1;
		}
	}
	/*
	 * If destination line is off the screen
	 * provide an appropriate "on screen" target,
	 * call ourselves recursively to get it,
	 * and then roll the screen up.
	 */
	if (destline > LINES - 1) {
		l = destline;
		destline = LINES - 1;
		if (outline < LINES - 1) {
			c = destcol;
			if (pfast == 0 && !CA)
				destcol = 0;
			fgoto();
			destcol = c;
		}
		while (l > LINES - 1) {
			putch('\n');
			l--;
			if (pfast == 0)
				outcol = 0;
		}
	}
	if (destline < outline && !(CA || UPLINE))
		/*
		 * If can't CA or UPLINE
		 * we have no hope of reducing outline.
		 */
		destline = outline;
	/*
	 * Try to do the move with very local cursor motions,
	 * perhaps using a carriage return to move to
	 * the left margin.
	 */
	if (motion())
		return;
	if (CA && !value(PRINTALL)) {
		putS(cgoto());
		outline = destline;
		outcol = destcol;
	} else
		plod();
}

void
setcol(int col)
{
	/*register int del;*/

	flush1();
	/*del = col - outcol;*/
	destcol = col;
	if (destcol < 0)
		destcol = 0;
	/* off the line may not work on 3a here */
	fgoto();
#if 0
	/*
	 * ADM3A special - fix tabs
	 */
	if (TTY == 'ca' && (del <= -4 || del >= 4)) {
		del -= 3;
		del &= 07;
		if (del < 0)
			del += 8;
		while (del > 0) {
			/*
			 * A tip of the hatlo hat here too!
			 */
			putch('\033');
			putch('x');
			del--;
		}
	}
#endif
	flush();
}

/*
 * v is vertical distance, then cost with cr
 * h is horizontal distance, then direct move cost
 */
static int
motion(void)
{
	register int v, h;

	if (!BS)
		return (0);
	v = destline - outline;
	if (v < 0) {
		if (CA || UPLINE)
			v = -v;
		else
			destline = outline;
	}
	h = destcol;
	if (!v || pfast) {
		h -= outcol;
		if (h < 0)
			h = -h;
	}
	h += v;
	if (pfast || !NOCR) {
		if (outcol)
			v++;
		v += destcol;
	} else
		v = 5;
	if (v >= 4 && h >= 4)
		return (0);
	plod();
	return (1);
}

static void
plod(void)
{
	register int i;

	while (outline < destline) {
		outline++;
		putch('\n');
		if (NOCR || pfast == 0)
			outcol = 0;
	}
	if (!NOCR && (outcol - destcol > destcol + 1 || (outcol > destcol && !BS))) {
		putch('\r');
		outcol = 0;
	}
	while (outcol > destcol) {
		outcol--;
		putch('\b');
	}
	while (outline > destline) {
		outline--;
		putS(UPLINE);
	}
	if (!CA && PT)
		while ((i = ((outcol + 8) &~ 7)) < destcol) {
			putch('\t');
			outcol = i;
		}
	while (outcol < destcol) {
		if (inopen) {
			if (!NDSPACE) {
				/*
				 * What a feat of engineering!
				 * A terminal with cursor addressibility
				 * but no cursor move 1 char right sequence!
				 */
				putS(cgoto());
				outcol = destcol;
				return;
			}
			putS(NDSPACE);
		} else
			putch(' ');
		outcol++;
	}
}

void
partinp(void)
{

	putch('\n');
	noteinp();
}

void
noteinp(void)
{

	outline++;
	if (outline > LINES - 1)
		outline = LINES - 1;
	destline = outline;
	destcol = outcol = 0;
}

void
notech(int i)
{

	outcol += i;
	destcol += i;
}


#if 0
#define	ECHO	010
#define	CRLF	020
#endif

/*
 * Newline + flush.
 */
void
putNFL(void)
{

	putnl();
	flush();
}

void
pstart(void)
{

 	if (NOCR || !value(OPTIMIZE) || value(PRINTALL))
		return;
	if (ruptible == 0)
		return;
	fgoto();
	flusho();
	if (pfast)
		return;
	pfast = 1;
	gTTY(1);
	normtty = 1;
	normf = tty;
	tty.c_oflag &= ~(ONLCR
#ifdef TAB3
	|TAB3
#endif
	);
	tty.c_lflag &= ~ECHO;
	sTTY(1);
}

void
pstop(void)
{

	phadnl = 0;
	linp = line;
	draino();
	normal();
	if (pfast != 1)
		return;
	pfast = 0;
}

void
termreset(void)
{

	destcol = 0;
	destline = LINES - 1;
	if (!value(PRINTALL)) {
		if (CA) {
			outcol = -20;
			outline = -20;
		} else {
			outcol = destcol;
			outline = destline;
		}
	}
}

void
draino(void)
{

}

void
ostart(void)
{

	if (!intty)
		error("Open and visual must be used interactively");
	gTTY(1);
	normtty = 1;
	normf = tty;
	tty.c_iflag &= ~ICRNL;
	tty.c_lflag &= ~(ECHO|ICANON);
	tty.c_oflag &= ~(
#ifdef TAB3
	TAB3|
#endif
	ONLCR);
	tty.c_cc[VMIN] = 1;
	tty.c_cc[VTIME] = 1;
	ttcharoff();
	sTTY(1);
	pfast = 2;
}

static void
ttcharoff(void)
{
	long vdisable;

	if ((vdisable = fpathconf(STDIN_FILENO, _PC_VDISABLE)) == -1)
		vdisable = '\377';
	tty.c_cc[VQUIT] = vdisable;
#ifdef VSUSP
	tty.c_cc[VSUSP] = vdisable;
#endif
#ifdef VDSUSP
	tty.c_cc[VDSUSP] = vdisable;
#endif
#ifdef VSTART
	tty.c_cc[VSTART] = vdisable;
#endif
#ifdef VSTOP
	tty.c_cc[VSTOP] = vdisable;
#endif
}

void
ostop(void)
{

	pfast = 0;
	normal();
	termreset();
}

void
flusho(void)
{

	fflush(stdout);
}

void
putnl(void)
{

	ex_putchar('\n');
}

int
gTTY(int i)
{
	return tcgetattr(i, &tty);
}

int
sTTY(int i)
{
	return tcsetattr(i, TCSAFLUSH, &tty);
}

void
putch(int c)
{

	putc(c, stdout);
}
