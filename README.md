# md5sum
> forked from https://github.com/chinaran/Compute-file-or-string-md5

## Functionality

It is console application that calculates md5 hashsum for whole current directory.

This program is not md5sum from coreutils: that utility not support hashing whole directories recursively. Moreover this program can be compiled simply for portable windows md5sum-win64.exe.

Change history is [here](changelog.md).

## TODO

1. Ignore md5sum.txt and md5sum.detalized.txt (oops...). Critical error.
2. Add filter function to prevent calculating log-files and version control internals.
3. Add support for "--help".

## Building

Tested on Ubuntu-20.04 LTS and Ubuntu-18.04 LTS. Others not tryed (and not adapted).

Just run ```make```: it builds **both linux and windows binaries**.

Used compilers: *gcc* and *x86_64-w64-mingw32-gcc* (Ubuntu: ```sudo apt install -y gcc-mingw-w64```).

## License

[Apache-2.0 License](https://github.com/1024sparrow/md5sum/blob/master/LICENSE)
