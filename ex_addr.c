#include "ex.h"
#include "ex_re.h"
/*
 * Ex - a text editor
 * Bill Joy UCB June, 1977
 */
static	char bigmove;

void
setdot(void)
{

	setdot1();
	if (bigmove)
		markDOT();
}

void
setdot1(void)
{

	if (addr2 == 0)
		addr1 = addr2 = dot;
	if (addr1 > addr2) {
		notempty();
		error("Addr1 > addr2|First address exceeds second");
	}
}

void
setcount(void)
{
	register int cnt;

	skipwh();
	if (!digit(peekchar())) {
		setdot();
		return;
	}
	addr1 = addr2;
	setdot();
	cnt = getnum();
	if (cnt <= 0)
		error("Bad count|Nonzero count required");
	addr2 += cnt - 1;
	if (addr2 > dol)
		addr2 = dol;
	nonzero();
}

int
getnum(void)
{
	register int cnt;

	for (cnt = 0; digit(peekchar());)
		cnt = cnt * 10 + ex_getchar() - '0';
	return (cnt);
}

void
markDOT(void)
{

	markpr(dot);
}

void
markpr(int *dot)
{

	if ((inglobal == 0 || inopen) && dot <= endcore)
		names[26] = *dot & ~01;
}

void
notempty(void)
{

	if (dol == zero)
		error("No lines@in the buffer");
}

void
setall(void)
{

	if (addr2 == 0) {
		addr1 = one;
		addr2 = dol;
		if (dol == zero) {
			dot = zero;
			return;
		}
	}
	setdot1();
}

void
setnoaddr(void)
{

	if (addr2 != 0)
		error("No address allowed@on this command");
}

void
nonzero(void)
{

	if (addr1 == zero) {
		notempty();
		error("Nonzero address required@on this command");
	}
}

int *
address(void)
{
	register int *addr, offset;
	int c;
	int lastsign;

	bigmove = 0;
	lastsign = 0;
	offset = 0;
	addr = 0;
	for (;;) {
		if (digit(peekchar())) {
			if (addr == 0) {
				addr = zero;
				bigmove = 1;
			}
			addr += offset;
			offset = getnum();
			if (lastsign >= 0)
				addr += offset;
			else
				addr -= offset;
			lastsign = 0;
			offset = 0;
		}
		switch (c = ex_getchar()) {

		case '?':
		case '/':
		case '$':
		case '\'':
		case '\\':
			bigmove++;
		case '.':
			if (addr || offset)
				error("Relocation error@- can only offset by constants");
		}
		offset += lastsign;
		lastsign = 0;
		switch (c) {

		case ' ':
		case '\t':
			continue;

		case '+':
			lastsign = 1;
			if (addr == 0)
				addr = dot;
			continue;

		case '^':
		case '-':
			lastsign = -1;
			if (addr == 0)
				addr = dot;
			continue;

		case '\\':
		case '?':
		case '/':
			c = compile(c, 1);
			if (c == '&') {
				c = ex_getchar();
				if (c != '?' && c != '/')
					error("\\& needs / or ? in address");
			}
			notempty();
			addr = scanfor(c);
			if (addr == NIL)
				error("Fail|Pattern not found");
			continue;

		case '$':
			addr = dol;
			continue;

		case '.':
			addr = dot;
			continue;

		case '\'':
			c = ex_getchar();
			if (c == '\'')
				c = 'z' + 1;
			else if (c < 'a' || c > 'z')
				error("Marks are ' and a-z");
			c -= 'a';
			for (addr = one; addr <= dol; addr++)
				if (names[c] == (*addr &~ 01))
					break;
			if (addr > dol)
				error("Undefined mark@referenced");
			break;

		default:
			ungetchar(c);
			if (offset) {
				if (addr == 0)
					addr = dot;
				addr += offset;
			}
			if (addr == 0) {
				bigmove = 0;
				return (0);
			}
			if (addr != zero)
				notempty();
			addr += lastsign;
			if (addr < zero)
				error("Negative address@- first buffer line is 1");
			if (addr > dol)
				error("Not that many lines@in buffer");
			return (addr);
		}
	}
}

/*
 * Abbreviations to make code smaller
 * Left over from squashing ex version 1.1 into
 * 11/34's and 11/40's.
 */
void
setCNL(void)
{

	setcount();
	ex_newline();
}

void
setNAEOL(void)
{

	setnoaddr();
	eol();
}

int *
scanfor(int c)
{
	register int *addr;

	savere(&scanre);
	addr = dot;
	for (;;) {
		if (c == '/') {
			addr++;
			if (addr > dol) {
				if (value(EX_WRAP) == 0)
					error("No match to BOTTOM|Address search hit BOTTOM without matching pattern");
				addr = zero;
			}
		} else {
			addr--;
			if (addr < zero) {
				if (value(EX_WRAP) == 0)
					error("No match to TOP|Address search hit TOP without matching pattern");
				addr = dol;
			}
		}
		if (execute(0, addr))
			return (addr);
		if (addr == dot)
			return (NIL);
	}
}
