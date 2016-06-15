#
/*
 * Ex - a text editor
 * Bill Joy UCB September 1977
 */

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include "ex.h"
#include "ex_io.h"
#include "ex_glob.h"

STATIC	int slevel;

void
source(char *file, int okfail)
{
	jmp_buf osetexit;
	register int saveinp, ointty, oerrno;
	char reenter;

	signal(INTR, SIG_IGN);
	saveinp = dup(0);
	if (saveinp < 0)
		error("Too many nested sources");
	close(0);
	if (open(file, 0) < 0) {
		oerrno = errno;
		if (ruptible)
			signal(INTR, onintr);
		dup(saveinp);
		close(saveinp);
		errno = oerrno;
		if (!okfail)
			filioerr(file);
		return;
	}
	slevel++;
	ointty = intty;
	intty = isatty(0);
	reenter = 0;
	getexit(osetexit);
	setexit();
	if (ruptible)
		signal(INTR, onintr);
	if (reenter == 0) {
		reenter++;
		commands(1, 1);
	} else if (slevel > 1) {
		close(0);
		dup(saveinp);
		close(saveinp);
		slevel--;
		resexit(osetexit);
		reset();
	}
	intty = ointty;
	close(0);
	dup(saveinp);
	close(saveinp);
	slevel--;
	resexit(osetexit);
}
