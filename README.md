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
**Attention**:
The original `vi` had not been 8-bit clean!
Moreover it does automatically change all 8-bit characters to 7-bit
in the whole file even if no editing is done!
This will e.g. destroy all UTF-8 characters.
