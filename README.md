# md5sum
> forked from https://github.com/chinaran/Compute-file-or-string-md5


## TODO

1. Ignore md5sum.txt and md5sum.detalized.txt (oops...). Critical error.
2. Add filter function to prevent calculating log-files and version control internals.
3. Add support of "--help".

## Building

Tested on Ubuntu-20.04 LTS and Ubuntu-18.04 LTS. Others not tryed (and not adapted).
Just run ```make```: it builds both linux and windows binaries. Used compilers: *gcc* and *x86_64-w64-mingw32-gcc* (Ubuntu: ```sudo apt install -y gcc-mingw-w64```).
