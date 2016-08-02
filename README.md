# ex-1.1 (from 1BSD release)
This is the very first `vi` version (1.1) taken from 1BSD.
It had been released on January 1, 1978.
## Installation notes
The software is downloaded with
```sh
git clone https://github.com/n-t-roff/ex-1.1.git
```
and can be kept up-to-date with
```sh
git pull
```
Some configuration (e.g. installation paths) can be done in the
[`makefile`](https://github.com/n-t-roff/ex-1.1/blob/master/Makefile.in).
For compiling it on BSD and Linux autoconfiguration is required:
```sh
$ ./configure
```
The software is build with
```sh
$ make
```
and installed with
```
$ su
# make install
# exit
```
All generated files are removed with
```sh
$ make distclean
```
## Usage notes
While `vi`
[version 2.2](https://github.com/n-t-roff/ex-2.2)
behaves quite similar to later versions,
`vi` version 1.1 is rather different (see
[ex1.1-2.0.pdf](http://n-t-roff.github.io/ex/3.2/ex1.1-2.0.pdf)).
Please study the manual
[exrefm.pdf](http://n-t-roff.github.io/ex/1.1/exrefm.pdf)
carefully.
This version does not start in visual mode.
There is no `-v` option.
Program names `vi` and `view` did not exist.
So you have to enter the command `vi` at the `ex` prompt `:`.
If the file is empty (or no file is loaded), command `vi` won't
work.
In this case you need to issue the `a` command (*append*) to type
some text.
After that you put a `.` (*dot*) on a line by itself to quit
append mode.
With the added text, command `vi` will work.
Note that ex-1.1 places the cursor at the last line of the file.
Use `1G` or `-` or `k` to move upwards.
Alternatively use `1vi` as first command to open visual at first line.
After any command ex-1.1 stays in ex mode.
`vi` will put the current line to top of the window.
Use command `vi.` instead to open visual with the current line
centered in the window.
The most significant differences for visual mode are:
* **Don't use unescaped `@` in input mode, type `\@` instead.**
  `@` in input mode deletes the current line.
  This is an intended feature and not a bug.
* Use `q` (instead of `:`) to enter an `ex` command.
  After that you stay in `ex` mode.
  Use command `vi` to go back to visual mode.
* Only *n*`G`, `-`, `k` can be used to move upwards.
  `^B`, `^U` and `^Y` did not exist.
  (`H-` can be used instead of `^U`.)
* `^F` did not exist, use `^D`
* Use `^Z` instead of `^L`.
  (Job control had been added in version 3.4.)
* Use `\\` instead of `dd`.
  After deleting lines a `@` appears at that position.
  This is intended and not a bug.
  (You may ignore these lines or type `^Z` to remove them.)
* Use `Y` instead of `yy`.
* Note that it is not possible to delete a range of lines
  with `:`*startline*`,`*endline*`d` and then insert these
  lines with `p` or `P`.
  On change between ex and visual mode the yank buffer
  information it lost.
  So even `:`*line*`put` can't be used.
  The combination `delete`/`put` does only work if ex
  mode is not left.
  Alternatively use *n*`\\` to delete and `p`/`P` to
  insert.
* In input mode use `^H` instead of &lt;BACKSPACE&gt;
* Use `` :e` `` instead of `:e#` to address the alternate
  file name.
  (Also `^^` and `^6` did not exist.)
* Neither `ZZ` nor `:wq` nor `:x` had been implemented.
  Use the sequence `qw`&lt;ENTER&gt; `q`&lt;ENTER&gt;
  to write and quit.
  (Can be combined as `qw|q`.)
* `/`, `?` and `n` search only for the first occurrence
  of a pattern in a line.
* Use `K`*letter* instead of `m`*letter* to set marks.
* `d` can only be used for delete operations within
  a single line.
  Operations like `dG` or `d1G` don't exist before ex
  version 2.
  Use ex commands `,$d` and `1,d` instead.
* Control characters are printed as `?` and it is not
  possible to insert them preceded with `^V`.
  Use at least
  [vi version 2.2](https://github.com/n-t-roff/ex-2.2)
  to edit control characters in files.
  (Existing control characters are left unchanged by
  vi 1.1.)
* `%` (find matching `()`, `{}` or `[]`) is not
  available before ex version 2.
* `]]` and `[[` (search for begin of a C function,
  i.e. a line starting with `{`) had not been available
  before ex version 2.
* In rare cases some word characters are displayed as
  spaces.
  This is a visual effect and not a data issue.
  Going to ex mode with `q` and returning to vi mode
  fixes this.
* The code is compiled for editing files with at most
  16384 lines and 256 KB temp file size.

**Attention**:
The original `vi` had not been 8-bit clean!
Moreover it does automatically change all 8-bit characters to 7-bit
in the whole file even if no editing is done!
This will e.g. destroy all UTF-8 characters.
