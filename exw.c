#include <sys/types.h>
#ifdef USE_SYS_MKDEV_H
# include <sys/mkdev.h>
#endif
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "ex.h"
#include "ex_io.h"
/*
 * Ex - a text editor
 * Bill Joy UCB June 1977
 */

void
wop(void)
{
	int c;
	int exclam;
	register int bl;
	struct stat stbuf;

	c = 0;
	exclam = 0;
	if (peekchar() == '!') {
		exclam++;
		ex_getchar();
	}
	bl = skipwh();
	while (peekchar() == '>') {
		ex_getchar();
		c++;
		skipwh();
	}
	if (c == 0 && bl == 0 && !endcmd(peekchar()))
		error("Blank required@before filename");
	if (c != 0 && c != 2)
		error("Write forms are 'w' and 'w>>'@- use 'w!' and 'w!>>' to override checks");
	filename('w');
	switch (c) {
		case 0:
			if (exclam)
				goto cre;
			if (value(EDITED) && eq(file, savedfile)) {
				if (addr1 != one || addr2 != dol)
					error(" Can't 'w' partial buffer to edited file@- use 'w!' to override checks");
				goto cre;
			}
			if (stat(file, &stbuf))
				goto cre;
			if (S_ISCHR(stbuf.st_mode)) {
				if (major(stbuf.st_dev) == DTTYMAJ
				    && minor(stbuf.st_dev) == DTTYMIN)
					goto cre;
				if (major(stbuf.st_dev) == DVNLMAJ
				    && minor(stbuf.st_dev) == DVNLMIN)
					goto cre;
			}
			io = open(file, O_WRONLY);
			if (io < 0)
				ioerror();
			if (!isatty(io))
				error(" File exists@- use \"write! %s\" if you really want to overwrite it", file);
			close(io);
cre:
			synctmp();
			io = creat(file, value(MODE));
			if (io < 0)
				ioerror();
			break;
		case 2:
			io = open(file, O_WRONLY);
			if (io < 0 || lseek(io, 0, SEEK_END) == -1) {
				if (exclam /* || value(WRITEANY) */)
					goto cre;
				ioerror();
			}
			break;
	}
	putfile();
	iostats();
	if (c != 2 && addr1 == one && addr2 == dol) {
		if (eq(file, savedfile))
			value(EDITED) = 1;
		ex_sync();
	}
}
