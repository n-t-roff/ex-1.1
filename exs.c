#include <unistd.h>
#include "ex.h"
#include "ex_re.h"
#include "ex_vis.h"
/*
 * Ex - a text editor
 * Bill Joy UCB June 1977
 */

static int compsub(int);
static void comprhs(int);
static int dosubcon(int, int *);
static int confirmed(int *);
static void ugo(int, int);
static void dosub(void);
static int fixcase(int);
static char *place(char *, char *, char *);

STATIC	char xflag;
STATIC	int scount, slines, stotal;

int
substitute(char c)
{
	register int *addr, n;
	char gsubf;

	gsubf = compsub(c);
	if (!inglobal)
		save12(), undkind = UNDCHANGE;
	stotal = 0;
	slines = 0;
	for (addr = addr1; addr <= addr2; addr++) {
		scount = 0;
		if (dosubcon(0, addr) == 0)
			continue;
		if (gsubf)
			while (*loc2)
				if (dosubcon(1, addr) == 0)
					break;
		if (scount) {
			stotal += scount;
			slines++;
			putmark(addr);
			n = append(getsub, addr);
			addr += n;
			addr2 += n;
		}
	}
	if (stotal == 0 && !inglobal && !xflag)
		error("Fail|Substitute pattern match failed");
	snote(stotal, slines);
	return (stotal);
}

static int
compsub(int ch)
{
	int seof, c;
	int gsubf;

	gsubf = 0;
	xflag = 0;
	switch (ch) {
		case 's':
			skipwh();
			seof = ex_getchar();
			if (endcmd(seof))
				error("Substitute needs re|Missing regular expression for substitute");
			seof = compile(seof, 0);
			savere(&subre);
			comprhs(seof);
			break;
		case '&':
			if (subre.sexpbuf[0] == 0)
				error("No previous substitute re|No previous substitute to repeat");
			resre(&subre);
			break;
		case '~':
			if (re.sexpbuf[0] == 0)
				error("No previous re|No previous regular expression");
			savere(&subre);
			break;
	}
	for (;;) {
		c = ex_getchar();
		switch (c) {
			case 'g':
				gsubf++;
				continue;
			case 'c':
				xflag++;
				continue;
			default:
				ungetchar(c);
				setcount();
				ex_newline();
				return (gsubf);
		}
	}
}

static void
comprhs(int seof)
{
	register char *rp, *orp;
	int c;
	char orhsbuf[LBSIZE / 2];

	rp = rhsbuf;
	strcpy(orhsbuf, rp);
	for (;;) {
		c = ex_getchar();
		if (c == seof)
			break;
		switch (c) {

		case '\\':
			c = ex_getchar();
			if (value(MAGIC)) {
				/*
				 * When "magic", \& turns into a plain &,
				 * and all other chars work fine quoted.
				 */
				if (c != '&')
					c |= QUOTE;
				break;
			}
			/*
			 * When "nomagic", \~ forces rhs, \& forces a
			 * quoted & like & normally does, and all others
			 * force themselves quoted.
			 */
magic:
			switch (c) {
				case '~':
					for (orp = orhsbuf; *orp; *rp++ = *orp++)
						if (rp >= &rhsbuf[LBSIZE / 2 + 1])
							goto toobig;
					continue;
			     /* case '&': */
				default:
					c |= QUOTE;
					break;
			}
			break;
		case '\n':
		case EOF:
			ungetchar(c);
			goto endrhs;
		case '~':
		case '&':
			if (value(MAGIC))
				goto magic;
			break;
		}
		if (rp >= &rhsbuf[LBSIZE / 2 - 1])
toobig:
			error("Replacement pattern too long@- limit 256 characters");
		*rp++ = c;
	}
endrhs:
	*rp++ = 0;
}

int
getsub(void)
{
	register char *p;

	if ((p = linebp) == 0)
		return (EOF);
	strcLIN(p);
	linebp = 0;
	return (0);
}

static int
dosubcon(int f, int *a)
{

	if (execute(f, a) == 0)
		return (0);
	if (confirmed(a)) {
		dosub();
		scount++;
	}
	return (1);
}

static int
confirmed(int *a)
{
	int c, ch;

	if (xflag == 0)
		return (1);
	inconf++;	/* suppress iul */
	pline(a - zero);
	putNFL();
	c = column(loc1 - 1);
	ugo(c, ' ');
	ugo(column(loc2 - 1) - c, '^');
	flush();
	inconf = 0;
	c = ch = getch();
	while (c != '\n' && c != EOF)
		c = getch();
	noteinp();
	return (ch == 'y');
}

int
getch(void)
{
	char c;

	if (read(0, &c, 1) != 1)
		return (EOF);
	return (c & 0177);
}

static void
ugo(int cnt, int with)
{

	if (cnt > 0)
		do
			ex_putchar(with);
		while (--cnt > 0);
}

int	casecnt;
int	destcase;

static void
dosub(void)
{
	register char *lp, *sp, *rp;
	int c;

	lp = linebuf;
	sp = genbuf;
	rp = rhsbuf;
	while (lp < loc1)
		*sp++ = *lp++;
	casecnt = 0;
	while ((c = *rp++)) {
		if ((c & QUOTE))
			switch (c & 0177) {
				case '&':
					sp = place(sp, loc1, loc2);
					if (sp == 0)
						goto ovflo;
					continue;
				case 'l':
					casecnt = 1;
					destcase = 0;
					continue;
				case 'L':
					casecnt = 1000;
					destcase = 0;
					continue;
				case 'u':
					casecnt = 1;
					destcase = 1;
					continue;
				case 'U':
					casecnt = 1000;
					destcase = 1;
					continue;
				case 'E':
				case 'e':
					casecnt = 0;
					continue;
		}
		if (c < 0 && (c &= 0177) >= '1' && c < nbra + '1') {
			sp = place(sp, braslist[c - '1'], braelist[c - '1']);
			if (sp == 0)
				goto ovflo;
			continue;
		}
		if (casecnt)
			*sp++ = fixcase(c & 0177);
		else
			*sp++ = c & 0177;
		if (sp >= &genbuf[LBSIZE])
ovflo:
			error("Line overflow@in substitute - limit 512 chars");
	}
	lp = loc2;
	loc2 = sp - genbuf + linebuf;
	while ((*sp++ = *lp++))
		if (sp >= &genbuf[LBSIZE])
			goto ovflo;
	strcLIN(genbuf);
}

static int
fixcase(int c)
{
	if (casecnt == 0 || !letter(c))
		return (c);
	casecnt--;
	if (destcase)
		c &= ~' ';
	else
		c |= ' ';
	return (c);
}

static char *
place(char *sp, char *l1, char *l2)
{

	while (l1 < l2) {
		*sp++ = fixcase(*l1++);
		if (sp >= &genbuf[LBSIZE])
			return (0);
	}
	return (sp);
}

void
putmark(int *addr)
{

	putmk1(addr, putline());
}

void
putmk1(int *addr, int n)
{
	register int *markp;

	*addr &= ~1;
	for (markp = names; markp < &names[27]; markp++)
		if (*markp == *addr)
			*markp = n;
	*addr = n;
}
