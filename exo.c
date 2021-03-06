#include "ex.h"
#include "ex_re.h"
#include "ex_vis.h"
#include "ex_tty.h"
/*
 * Ex - a text editor
 * Bill Joy UCB June 1977
 */

void
oop(void)
{
#ifndef VISUAL

	error("No open in this version");
}
#endif
#ifdef VISUAL
	char *ic;
	static char atube[TUBESIZE + LBSIZE];
	int lines;

	if (!value(OPEN))
		error("Can't open unless open option is set");
	if (OS)
		error("Can't open on a terminal which overstrikes (yet)");
	VCOLUMNS = COLUMNS;
	if (!CA && AM)
		VCOLUMNS--;
	visual = 0;
	setdot();
	nonzero();
	skipwh();
	if (!inglobal)
		saveall();
	lines = dol - zero;
	if (peekchar() == '/') {
		compile(ex_getchar(), 1);
		savere(&scanre);
		if (execute(0, addr1) == 0)
			error("Fail|Pattern not found on addressed line");
		ic = loc1;
		if (ic > linebuf && *ic == 0)
			ic--;
	} else {
		ex_getline(*addr1);
		ic = vskipwh(linebuf);
	}
	ex_newline();
	VLINES = LINES;
	vok(atube);
	Outchar = &vputchar;
	ostart();
	vch = 0;
	if (CA) {
		if (outcol == -20)
			outcol = 0;
		vup1();
	} else if (!UPLINE)
		outline = destline = LINES - 1;
	vholdmove = 1;
	dot = addr1;
	for (;;) {
		if (!vholdmove)
			vup1();
		vholdmove = 0;
		voinit();
		if (vmain(ic) == -1 || vch != '.')
			break;
		getDOT();
		ic = vskipwh(linebuf);
	}
	ostop();
	inopen = 0;
	if (!CA)
		vup1();
	flusho();
	setlastchar('\n');
	setoutt();
	netchHAD(lines);
}

char *
vskipwh(char *cp)
{

	while (white(*cp) && cp[1])
		cp++;
	return (cp);
}

void
voinit(void)
{
	register int i;

	vcnt = 0;
	vcline = 0;
	i = LINES - 1;
	vclrbyte(vtube[i], VCOLUMNS);
	if (CA) {
		i--;
		vclrbyte(vtube[i], VCOLUMNS);
	}
	vopen(dot, i);
}
#endif
