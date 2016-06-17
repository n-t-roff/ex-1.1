#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "ex.h"
#include "ex_io.h"
/*
 * Ex - a text editor
 * Bill Joy UCB June 1977
 */

void
rop(int c)
{
	register int i;
	struct stat stbuf;
	int magic;

	io = open(file, 0);
	if (io < 0) {
		if (c == 'e' && errno == ENOENT)
			value(EDITED)++;
		ioerror();
	}
	if (value(EDITANY) == 0) {
		if (fstat(io, &stbuf))
			ioerror();
		switch (stbuf.st_mode & S_IFMT) {
			case S_IFBLK:
				error(" Block special file");
			case S_IFCHR:
				if (gTTY(io) == 0)
					error(" Teletype");
				if (major(stbuf.st_dev) == DVNLMAJ
				    && minor(stbuf.st_dev) == DVNLMIN)
					break;
				error(" Character special file");
			case S_IFDIR:
				error(" Directory");
			case S_IFREG:
				i = read(io, &magic, 2);
				lseek(io, 0, 0);
				if (i != 2)
					break;
				switch (magic) {
					case 0404:
						error(" Pascal object");
					case 0405:
						error(" Overlay executable");
					case 0407:
						error(" Executable");
					case 0410:
						error(" Pure executbable");
					case 0411:
						error(" Separate executable");
					case 0177545:
						error(" Archive");
					case 0177555:
						error(" Old archive");
					default:
						if (magic & 0100200)
							error(" Non-ascii file");
						break;
				}
		}
	}
	if (c == 'r')
		setdot();
	else
		setall();
	rop2();
	rop3(c);
}

void
rop2(void)
{

	deletenone();
	clrstats();
	append(getfile, addr2);
}

void
rop3(int c)
{

	if (iostats() == 0)
		if (c == 'e')
			value(EDITED)++;
	if (c == 'e')
		undkind = UNDNONE;
	if (laste) {
		laste = 0;
		ex_sync();
	}
}
