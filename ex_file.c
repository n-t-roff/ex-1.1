#include "ex.h"
/*
 * Ex - a text editor
 * Bill Joy UCB September 1977
 */

void
filename(int comm)
{
	register c;

	c = getchar();
	if (endcmd(c)) {
		if (savedfile[0] == 0 && comm != 'f')
			error("No file|No current filename");
		strcpy(file, savedfile);
	} else {
		ungetchar(c);
		getone();
		c = comm;
		if (savedfile[0] == 0 && c != 'E' && c != 'e') {
			c = 'e';
			value(EDITED) = 0;
		}
		switch (c) {
			case 'f':
				value(EDITED) = 0;
			case 'e':
				if (savedfile[0])
					strcpy(altfile, savedfile);
				strcpy(savedfile, file);
				break;
			default:
				if (file[0])
					strcpy(altfile, file);
				break;
		}
	}
	if (value(HUSH) && comm != 'f' || comm == 'E')
		return;
	if (file[0] != 0) {
		lprintf("\"%s\"", file);
		if (comm == 'f') {
			if (value(EDITED))
				ex_printf(" [Edited]");
			if (tchngflag)
				ex_printf(" [Modified]");
		}
		flush();
	} else
		ex_printf("No file ");
	if (comm == 'f')
		ex_printf(mesg(" %d lin%s@in buffer"),
		    dol - zero, dol == one ? "e" : "es");
}
