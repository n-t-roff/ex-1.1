# ex-1.1 (from 1BSD release)
This is `vi` version 1.1 taken from 1BSD.
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
While `vi` version 2.2 behaves quite similar to later versions,
`vi` version 1.1 is rather different.
Please study the manual
[exrefm.pdf](https://github.com/n-t-roff/ex-1.1/blob/master/exrefm/exrefm.pdf)
carefully.
This version doesn't start in visual mode even if installed with
name `vi`.
There is no `-v` option.
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
* Use `^Z` instead of `^L`
  (Job control had been added in version 3.4)
* Use `\\` instead of `dd`.
  After deleting lines a `@` appears at that position.
  This is intended and not a bug.
* Use `Y` instead of `yy`
* In input mode use `^H` instead of &lt;BACKSPACE&gt;
* Neither `ZZ` nor `:wq` nor `:x` had been implemented.
  Use the sequence `qw`&lt;ENTER&gt; `q`&lt;ENTER$gt;
  to write and quit.

**Attention**:
The original `vi` had not been 8-bit clean!
Moreover it does automatically change all 8-bit characters to 7-bit
in the whole file even if no editing is done!
This will e.g. destroy all UTF-8 characters.
