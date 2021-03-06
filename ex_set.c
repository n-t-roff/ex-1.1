#include "ex.h"
#include "ex_tty.h"
/*
 * Ex - a text editor
 * Bill Joy UCB September 1977
 */

static int setend(void);
static void propts(char);
static void propt(struct varbl *);
static void pro(int);

char	shellname[ONMSZ]		= "/bin/sh";
char	direct[ONMSZ]		= "/tmp";
char	ttytype[ONMSZ]		= "unknown";

char	NONAME[]		= "@";

#define	ONOFF	0
#define	NUMERIC	1
#define	STRING	2
#define	TERM	3

char	NOTify[]	= "notify";

struct varbl varbls[] = {
	{ { "autoindent",	"ai"   }, ONOFF,   0,    0,	0 },
	{ { "autoprint",	"ap"   }, ONOFF,   1,    1,	0 },
	{ { "beautify",		NONAME }, ONOFF,   0,    0,	0 },
	{ { "directory",	"dir"  }, STRING,  0,    0,	direct },
	{ { "editany",		"ea"   }, ONOFF,   0,    0,	0 },
	{ { "edited",		NONAME }, ONOFF,   1,    0,	0 },
	{ { "errorbells",	"eb"   }, ONOFF,   1,    1,	0 },
	{ { "fork",		NONAME }, ONOFF,   1,    1,	0 },
	{ { "home",		NONAME }, STRING,  0,    0,	home },
	{ { "hush",		NONAME }, ONOFF,   0,    0,	0 },
	{ { "ignorecase",	"ic"   }, ONOFF,   0,    0,	0 },
	{ { "indicateul",	"iu"   }, ONOFF,   0,    0,	0 },
	{ { "list",		NONAME }, ONOFF,   0,    0,	0 },
	{ { "magic",		NONAME }, ONOFF,   1,    1,	0 },
	{ { "mode",		NONAME }, NUMERIC, 0644, 0644,	0 },
	{ { NOTify,		NONAME }, NUMERIC, 5,    5,	0 },
	{ { "number",		NONAME }, ONOFF,   0,    0,	0 },
	{ { "open",		NONAME }, ONOFF,   1,    1,	0 },
	{ { "optimize",		NONAME }, ONOFF,   1,    1,	0 },
	{ { "printall",		"pa"   }, ONOFF,   0,    0,	0 },
	{ { "prompt",		NONAME }, ONOFF,   1,    1,	0 },
	{ { "scroll",		NONAME }, NUMERIC, 12,   12,	0 },
	{ { "shell",		"sh"   }, STRING,  0,    0,	shellname },
	{ { "shiftwidth",	"sw"   }, NUMERIC, 8,    8,	0 },
	{ { "sticky",		NONAME }, ONOFF,   0,    0,	0 },
	{ { "ttytype",		"tty"  }, TERM,    0,    0,	ttytype },
	{ { "terse",		NONAME }, ONOFF,   0,    0,	0 },
	{ { "visualmessage",	"vm"   }, ONOFF,   0,    0,	0 },
	{ { "window",		NONAME }, NUMERIC, 23,   23,	0 },
	{ { "wrap",		NONAME }, ONOFF,   1,    1,	0 },
	{ { 0,			0      }, 0,       0,    0,	0 }
};

void
set(int c)
{
	register char *op;
	register struct varbl *vp;
	char no, optname[ONMSZ];
	int base;
	extern char uxb[];

	setnoaddr();
	if (endcmd(peekchar())) {
		ex_getchar();
		propts(0);
		return;
	}
	if (c == 0)
		error("Blank required@before options in set");
	do {
		switch (c = ex_getchar()) {
			case '!':
				op = uxb;
				if (!*op)
					error("No previous command");
				goto pfile;
			case '`':
				op = altfile;
				if (!*op)
					error("No alternate filename");
				goto pfile;
			case '%':
				op = savedfile;
				if (!*op)
					error("No current filename");
pfile:
				if (!setend())
					error("Can't assign@to %c with set", c);
				lprintf("%c=%s\n", c, op);
				goto next;
		}
		if (!letter(c))
			error("Unexpected %c|Expected option name, found %c", c);
		op = optname;
		do {
			if (op >= &optname[ONMSZ])
				error("No such option|Ridiculously long option name");
			*op++ = letter(c);
			c = ex_getchar();
		} while (letter(c));
		ungetchar(c);
		*op = 0;
		no = 0;
		op = optname;
		if (eq("all", op)) {
			propts(1);
			goto next;
		}
		if (!eq(op, NOTify) && op[0] == 'n' && op[1] == 'o') {
			op += 2;
			no++;
		}
		for (vp = varbls; vp->vname[0]; vp++)
			if (eq(vp->vname[0], op) || eq(vp->vname[1], op))
				break;
		if (vp->vname[0] == 0)
			error("%s: No such option@- 'set all' gives all option values", op);
		c = skipwh();
		if (peekchar() == '?') {
			ex_getchar();
printone:
			propt(vp);
			goto next;
		}
		if (no && vp->vtype != ONOFF)
			error("Option %s is not a toggle", vp->vname[0]);
		switch (vp->vtype) {
			case ONOFF:
				vp->vvalue = 1 - no;
				break;
			case NUMERIC:
				if (c != 0 || setend())
					goto printone;
				if (ex_getchar() != '=')
					error("= expected@before number for numeric option %s", vp->vname[0]);
				if (!digit(peekchar()))
					error("Digits required@after = when assigning numeric option");
				c = 0;
				base = 10;
				if (peekchar() == '0' || vp == &varbls[MODE])
					base = 8;
				do
					c = c * base + ex_getchar() - '0';
				while (digit(peekchar()));
				vp->vvalue = c;
				break;
			case STRING:
			case TERM:
				if (c != 0 || setend())
					goto printone;
				if (ex_getchar() != '=')
					error("Missing =@in assignment to option %s", vp->vname[0]);
				op = optname;
				while (!setend()) {
					if (op >= &optname[ONMSZ])
						error("String too long@in option assignment");
					*op++ = ex_getchar();
				}
				*op = 0;
				if (vp->vtype == TERM)
					setterm(optname);
				else {
					strcpy(vp->vsvalue, optname);
					vp->vdefault = 1;
				}
				break;
		}
next:
		flush();
		skipwh();
	} while (!endcmd(peekchar()));
	eol();
}

static int
setend(void)
{

	return (white(peekchar()) || endcmd(peekchar()));
}

static void
propts(char all)
{
	register struct varbl *vp;

	for (vp = varbls; vp->vname[0] != NIL; vp++) {
		if (!all) switch (vp->vtype) {
			case ONOFF:
			case NUMERIC:
				if (vp->vvalue == vp->vdefault)
					continue;
				break;
			case STRING:
				if (vp->vdefault == 0)
					continue;
				break;
/*
			case TERM:
				break;
*/
		}
		propt(vp);
	}
}

static void
propt(struct varbl *vp)
{

	switch (vp->vtype) {
		case ONOFF:
			ex_printf("%s%s\n", vp->vvalue ? "" : "no", vp->vname[0]);
			break;
		case NUMERIC:
			if (vp == &varbls[MODE]) {
				ex_printf("mode=");
				pro(value(MODE));
				putnl();
			} else
				ex_printf("%s=%d\n", vp->vname[0], vp->vvalue);
			break;
		case STRING:
		case TERM:
			ex_printf("%s=%s\n", vp->vname[0], vp->vsvalue);
			break;
	}
	flush();
}

static void
pro(int i)
{

	if (i != 0)
		pro((i >> 3) & 017777);
	ex_putchar((i & 7) | '0');
}

void
REset(void)
{
	register struct varbl *vp;

	for (vp = varbls; vp->vname[0]; vp++)
		if (vp->vtype == NUMERIC || vp->vtype == ONOFF)
			vp->vvalue = vp->vdefault;
}
