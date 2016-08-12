#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <stdlib.h>
#include "ex.h"
#include "ex_re.h"
#include "ex_io.h"
/*
 * Ex - a text editor
 * Bill Joy UCB June 1977
 */

int tfile = -1;

#define	READ	0
#define	WRITE	1

static char *getblock(int, int);
static void wrerror(void);
static void blkio(off_t, void *, ssize_t (*)());

STATIC	char ichanged;
STATIC	int nleft;
STATIC	int ninbuf;
STATIC	int tline;

STATIC	long cntch, cntnull, cntodd;
STATIC	int cntln;

STATIC	char ibuff[512];
STATIC	int iblock;
STATIC	char obuff[512];
STATIC	int oblock;

STATIC	char tfname[40];
STATIC	char tftail[] = "/ExXXXXX";
STATIC	char havetmp;

void
fileinit(void)
{
	register char *p;
	register int i, j;
	struct stat stbuf;
	int serrno;
	char dumbcnt;

	cleanup();
	close(tfile);
	tline = 0200 * 3;
	blocks[0] = 1;
	blocks[1] = 2;
	blocks[2] = -1;
	dirtcnt = 0;
	iblock = -1;
	oblock = -1;
	strcpy(tfname, svalue(DIRECTORY));
	if (stat(tfname, &stbuf)) {
dumbness:
		serrno = errno;
		lprintf("PANIC: \"%s\":", tfname);
		flush();
		dumbcnt = 0;
		setexit();
		if (dumbcnt == 0) {
			errno = serrno;
			dumbcnt++;
			ioerror();
		}
		exit(1);
	}
	if (!S_ISDIR(stbuf.st_mode)) {
		errno = ENOTDIR;
		goto dumbness;
	}
	ichanged = 0;
	strcat(tfname, tftail);
	for (p = strend(tfname), i = 5, j = getpid(); i > 0; i--, j /= 10)
		*--p = j % 10 | '0';
	tfile = creat(tfname, 0600);
	if (tfile < 0)
		goto dumbness;
	havetmp = 1;
	close(tfile);
	tfile = open(tfname, O_RDWR);
	if (tfile < 0)
		goto dumbness;
#ifdef UNIX_SBRK
	brk(fendcore);
#endif
}

void
cleanup(void)
{
	if (havetmp)
		unlink(tfname);
	havetmp = 0;
}

void
onhup(int i)
{

	(void)i;
	if (chngflag == 0 || preserve())
		cleanup();
	exit(1);
}

void
clrstats(void)
{

	ninbuf = 0;
	cntch = 0;
	cntln = 0;
	cntnull = 0;
	cntodd = 0;
}

int
iostats(void)
{

	close(io);
	io = -1;
	if (value(HUSH) == 0) {
		if (value(TERSE))
			ex_printf(" %d/%ld", cntln, cntch);
		else
			ex_printf(" %d line%s, %ld character%s", cntln, plural(cntln), cntch, cntch == 1 ? "" : "s");
		if (cntnull || cntodd) {
			ex_printf(" (");
			if (cntnull) {
				ex_printf("%ld null", cntnull);
				if (cntodd)
					ex_printf(", ");
			}
			if (cntodd)
				ex_printf("%ld dirty", cntodd);
			ex_putchar(')');
		}
		putnl();
	}
	return (cntnull != 0 || cntodd != 0);
}

char *
plural(int i)
{

	return (i == 1 ? "" : "s");
}

char *
ex_getline(int tl)
{
	register char *bp, *lp;
	int nl;
	char scratch[LBSIZE];

	lp = scratch;
	bp = getblock(tl, READ);
	nl = nleft;
	tl &= ~0177;
	while ((*lp++ = *bp++))
		if (--nl == 0) {
			bp = getblock(tl += 0200, READ);
			nl = nleft;
		}
	return (unpack(scratch));
}

int
putline(void)
{
	register char *bp, *lp;
	int nl;
	int tl;
	char scratch[LBSIZE];

	dirtcnt++;
	lp = scratch;
	pack(lp);
	change();
	tl = tline;
	bp = getblock(tl, WRITE);
	nl = nleft;
	tl &= ~0177;
	while ((*bp++ = *lp++)) {
		if (--nl == 0) {
			bp = getblock(tl += 0200, WRITE);
			nl = nleft;
		}
	}
	nl = tline;
	tline += (((lp - scratch) + 07) >> 2) & 077776;
	return (nl);
}

STATIC	char *nextip;

int
getfile(void)
{
	int c;
	register char *lp, *fp;

	lp = linebuf;
	fp = nextip;
	do {
		if (--ninbuf < 0) {
			ninbuf = read(io, genbuf, LBSIZE) - 1;
			if (ninbuf < 0) {
				if (lp != linebuf) {
					ex_printf("[Incomplete last line] ");
					break;
				}
				return (EOF);
			}
			fp = genbuf;
		}
		if (lp >= &linebuf[LBSIZE])
			error(" Line too long@- limit 512 characters");
		c = *fp++;
		if (c == 0) {
			cntnull++;
			continue;
		}
		if (c & 0200) {
			cntodd++;
			c &= 0177;
			if (c == 0)
				continue;
		}
		*lp++ = c;
	} while (c != '\n');
	cntch += lp - linebuf;
	*--lp = 0;
	nextip = fp;
	cntln++;
	return (0);
}

void
putfile(void)
{
	int *a1;
	register char *fp, *lp;
	register int nib;

	a1 = addr1;
	clrstats();
	cntln = addr2 - a1 + 1;
	if (cntln == 0)
		return;
	nib = 512;
	fp = genbuf;
	do {
		lp = ex_getline(*a1++);
		for (;;) {
			if (--nib < 0) {
				nib = fp-genbuf;
				if (write(io, genbuf, nib) != nib)
					wrerror();
				cntch += nib;
				nib = 511;
				fp = genbuf;
			}
			if ((*fp++ = *lp++) == 0) {
				fp[-1] = '\n';
				break;
			}
		}
	} while (a1 <= addr2);
	nib = fp-genbuf;
	if (write(io, genbuf, nib) != nib)
		wrerror();
	cntch += nib;
}

static void
wrerror(void)
{

	if (eq(file, savedfile) && value(EDITED))
		change();
	ioerror();
}

void
ioerror(void)
{

	switch (errno) {
		case ENOENT:
			error(" No such file or directory");
		case EIO:
			error(" Physical I/O error");
		case EACCES:
			error(" Permission denied");
		case ENOTDIR:
			error(" Not a directory");
		case EISDIR:
			error(" Is a directory");
		case ENFILE:
			error(" File table overflow");
		case ENOSPC:
			error(" No space left on device");
		case EDQUOT:
			error(" Quota exceeded");
		case EROFS:
			error(" Read-only file system");
		default:
			error(" %s", strerror(errno));
	}
}

static char *
getblock(int atl, int iof)
{
	register int bno, off;
	
	bno = (atl >> 7) & 0777;
	off = (atl << 2) & 0770;
	if (bno >= 506)
		error(" Tmp file too large");
	nleft = 512 - off;
	if (bno == iblock) {
		ichanged |= iof;
		return (ibuff + off);
	}
	if (bno == oblock)
		return (obuff + off);
	if (iof == READ) {
		if (ichanged)
			blkio(iblock, ibuff, write);
		ichanged = 0;
		iblock = bno;
		blkio(bno, ibuff, read);
		return (ibuff + off);
	}
	if (oblock >= 0)
		blkio(oblock, obuff, write);
	oblock = bno;
	return (obuff + off);
}

static void
blkio(off_t b, void *buf, ssize_t (*iofcn)())
{

	lseek(tfile, b * 512, SEEK_SET);
	if ((*iofcn)(tfile, buf, 512) != 512)
		ioerror();
}

void
synctmp(void)
{
	register int *bp, *a, cnt;

	if (ichanged)
		blkio(iblock, ibuff, write);
	ichanged = 0;
	if (oblock != -1)
		blkio(oblock, obuff, write);
	time(&header.Atime);
	header.Auid = getuid();
	*zero = (int)header.Atime;
	for (a = zero, bp = blocks; a <= dol; a += 256, bp++) {
		if (*bp < 0) {
			tline = (tline + 0177) &~ 0177;
			*bp = ((tline >> 7) & 0777);
			tline += 0200;
			oblock = *bp + 1;
			bp[1] = -1;
		}
		if (*bp >= 511) {
			ex_printf(" Tmp file overflow");
			putNFL();
		}
		lseek(tfile, *bp * 512, SEEK_SET);
		cnt = (dol - a + 2) << 1;
		if (cnt > 512)
			cnt = 512;
		if (write(tfile, a, cnt) != cnt) {
oops:
			*zero = 0;
			ex_printf("SYNC error:");
			ioerror();
		}
		*zero = 0;
	}
	header.Alines = dol - zero;
	lseek(tfile, 0, SEEK_SET);
	if (write(tfile, &header, sizeof header) != sizeof header)
		goto oops;
/*
	lseek(tfile, 504, 0);
	write(tfile, "LOST", 5);
*/
}

void
TSYNC(void)
{

	if (dirtcnt > 12) {
		synctmp();
		dirtcnt = 0;
	}
}
