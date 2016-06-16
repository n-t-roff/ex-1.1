#
/*
 * Ex - a text editor
 * Bill Joy UCB September, 1977
 *
 * This glob routine mercilessly stolen from the shell.
 */
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "ex.h"
#include "ex_glob.h"
#include "ex_io.h"

static	int gflag;

static void sort(char **);
static void expand(char *);
static int match(char *, char *);
static int amatch(char *, char *);
static char *cat(char *, char *);
static void scan(char **, int (*)());
static int tglob(int);

static	char **av;
static	char *string, *str_end;
static	int ncoll;

static	char *gpathp;
static	int globbed;
static	char *entp;

struct	Glob *g;

#define	ab	g->Ab
#define	ava	g->Ava

void
glob(char **v, struct Glob *g0)
{
	register char **oav;

	g = g0;
	av = ava;
	string = ab;
	str_end = ab + 511;
	ncoll = 0;
	gpathp = file;
	gflag = 0;
	scan(v, &tglob);
	if (gflag == 0)
		ncoll++;
	while (*v) {
		oav = av;
		globbed = 0;
		expand(*v++);
		sort(oav);
	}
	*av = 0;
	if (ncoll == 0)
		error("No match@in filename expansion of `*', `/' or `[...]'");
	gargc = av - &ava[0];
}

static void
sort(char **oav)
{
	register char **p1, **p2, *c;

	p1 = oav;
	while (p1 < av-1) {
		p2 = p1;
		while (++p2 < av) {
			if (strcmp(*p1, *p2) > 0) {
				c = *p1;
				*p1 = *p2;
				*p2 = c;
			}
		}
		p1++;
	}
}

static void
expand(char *as)
{
	register char *cs, *sgpathp;
	register int dirf;
	static struct {
		int	ino;
		char	name[16];
	} entry;
	struct stat stbuff;

	sgpathp = gpathp;
	cs = as;
	while (*cs != '*' && *cs != '?' && *cs != '[') {
		if (gpathp >= &file[FNSIZE - 2])
			goto gpatherr;
		else if ((*gpathp++ = *cs++) == 0) {
			if (!globbed)
				*av++ = cat(as, "");
			else if (stat(file, &stbuff) >= 0) {
				*av++ = cat(file, "");
				ncoll++;
			}
			goto endit;
		}
	}
	gpathp = sgpathp;
	cs--;
	while (cs >= as && *cs != '/')
		cs--;
	while (as <= cs)
		if (gpathp >= &file[FNSIZE])
gpatherr:
			error("Path name too long@- editor limit 64 characters");
		else
			*gpathp++ = *as++;
	*gpathp = 0;
	dirf = open(file, 0);
	if (dirf < 0) {
		if (globbed)
			goto endit;
		else
			filioerr(file);
	}
	globbed++;
	cs++;
	while (read(dirf, &entry, 16) == 16) {
		if (entry.ino==0)
			continue;
		if (match(entry.name, cs)) {
			*av++ = cat(file, entry.name);
			ncoll++;
		}
	}
	close(dirf);
endit:
	gpathp = sgpathp;
	*gpathp = 0;
}

static int
match(char *s, char *p)
{
	int c;
	char *sentp;

	if (*s == '.' && *p != '.')
		return (0);
	sentp = entp;
	entp = s;
	c = amatch(s, p);
	entp = sentp;
	return (c);
}

static int
amatch(char *as, char *ap)
{
	register char *s, *p;
	int scc;
	int c, cc, ok, lc;
	char *sgpathp;
	struct stat stbuff;

	s = as;
	p = ap;
nextc:
	if ((scc = *s++ & 0177))
		if ((scc &= 0177) == 0)
			scc = 0200;
	switch (c = *p++) {
		case '[':
			ok = 0;
			lc = 077777;
			while ((cc = *p++)) {
				if (cc == ']') {
					if (ok)
						goto nextc;
					else
						return (0);
				} else if (cc == '-') {
					if (lc <= scc && scc <= *p++)
						ok++;
				} else
					if (scc == (lc = cc))
						ok++;
			}
			error("Missing ]@matching [ in filename");
		case '*':
			if (*p == '\0')
				return (1);
			else if (*p == '/') {
				p++;
				goto slash;
			}
			s--;
			while (*s)
				if (amatch(s, p))
					return (1);
				else
					s++;
			return (0);
		case '\0':
			return (scc == '\0');
		default:
			if (c == scc)
				goto nextc;
			else
				return (0);
		case '?':
			if (scc != '\0')
				goto nextc;
			else
				return (0);
		case '/':
			if (scc == '\0') {
slash:
				s = entp;
				sgpathp = gpathp;
				while ((*gpathp = *s++)) {
					if (gpathp >= &file[FNSIZE - 3])
						error("Path name too long@- editor limit 64 characters");
					gpathp++;
				}
				*gpathp++ = '/';
				*gpathp = 0;
				if (stat(file, &stbuff) == 0)
					if ((stbuff.st_mode & 060000)
					    == 040000) {
						if (*p == 0) {
							*av++ = cat(file, "");
							ncoll++;
						} else
							expand(p);
					}
				gpathp = sgpathp;
				*gpathp = 0;
			}
			return (0);
	}
}

static char *
cat(char *as1, char *as2)
{
	register char *s1, *s2;

	s2 = string;
	s1 = as1;
	while ((*s2++ = (*s1++ & 0177)))
		if (s2 >= str_end)
			goto toolong;
	s1 = as2;
	s2--;
	while ((*s2++ = *s1++))
		if (s2 > str_end)
toolong:
			error("Argument list too long@- editor limit 512 characters");
	s1 = string;
	string = s2;
	return (s1);
}

static void
scan(char **t, int (*f)())
{
	register char *p, c;

	while ((p = *t++))
		while ((c = *p))
			*p++ = (*f)(c);
}

static int
tglob(int c)
{
	if (any(c, "[?*"))
		gflag = 1;
	return (c);
}

#if 0
static int
trim(int c)
{

	return (c & 0177);
}
#endif

void
getone(void)
{
	register char *str;
	char *gv[2], *lp;
	struct Glob G;

	switch (getargs()) {
		case 0:
			error("Missing filename@- if you give blanks you must give a name");
		case 1:
			break;
		default:
			error("Too many names|Multiple file names allowed only on next command");
	}
	gv[0] = lp = genbuf;
	gv[1] = 0;
	glob(gv, &G);
	str = G.Ava[0];
	if (G.Ava[1] != NIL)
		error("Ambiguous|Pattern is ambiguous, matches more than one file");
	if (strlen(str) > 63)
		error("Filename too long@- limit 63 characters");
	strcpy(file, str);
}

void
filioerr(char *cp)
{
	register int oerrno;

	oerrno = errno;
	lprintf("\"%s\"", cp);
	errno = oerrno;
	ioerror();
}

int
any(int c, char *s)
{
	register int x;

	while ((x = *s++))
		if (x == c)
			return (1);
	return (0);
}
