#include "md5.h"

#define _XOPEN_SOURCE 500 // Required under GLIBC for nftw()
#define _XOPEN_SOURCE_EXTENDED 500 // Same
#define _GNU_SOURCE // for FTW_ACTIONRETVAL flag in nftw()

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for getdtablesize(), getcwd() declarations
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <errno.h>
#include <ftw.h>
#include <limits.h> // for PATH_MAX

#define READ_DATA_SIZE 1024
#define MD5_SIZE 16
#define MD5_STR_LEN (MD5_SIZE * 2)

#define WRITE_DATA_BUFFER_SIZE 1024

#define SPARE_FDS 5	/* fds for use by other functions, see text */

static char md5_str[MD5_STR_LEN + 1];
static int fdMd5Detalized;
static unsigned char writeBuffer[WRITE_DATA_BUFFER_SIZE]; // limit for row: 1024 symbols

// function declare
int
validateAnalizingPath(
    const char *path,
    int baseNameIndex,
    int fileType // FTW_F, FTW_D &etc. declared in <ftw.h>
);

int
Compute_string_md5(
    unsigned char *dest_str,
    unsigned int dest_len,
    char *md5_str
);

int
Compute_file_md5(
    const char *file_path,
    char *md5_str
);

#ifdef WIN32
static int dtablesize;
int getdtablesize (void)
{
    if (dtablesize == 0)
    {
        /* We are looking for the number N such that the valid file descriptors
           are 0..N-1.  It can be obtained through a loop as follows:
           {
           int fd;
           for (fd = 3; fd < 65536; fd++)
           if (dup2 (0, fd) == -1)
           break;
           return fd;
           }
           On Windows XP, the result is 2048.
           The drawback of this loop is that it allocates memory for a libc
           internal array that is never freed.

           The number N can also be obtained as the upper bound for
           _getmaxstdio ().  _getmaxstdio () returns the maximum number of open
           FILE objects.  The sanity check in _setmaxstdio reveals the maximum
           number of file descriptors.  This too allocates memory, but it is
           freed when we call _setmaxstdio with the original value.  */
        int orig_max_stdio = _getmaxstdio ();
        unsigned int bound;
        for (bound = 0x10000; _setmaxstdio (bound) < 0; bound = bound / 2)
            ;
        _setmaxstdio (orig_max_stdio);
        dtablesize = bound;
    }
    return dtablesize;
}
#endif


/**
 * compute the value of a string
 * @param  dest_str
 * @param  dest_len
 * @param  md5_str
 */
int Compute_string_md5(unsigned char *dest_str, unsigned int dest_len, char *md5_str)
{
	int i;
	unsigned char md5_value[MD5_SIZE];
	MD5_CTX md5;

	// init md5
	MD5Init(&md5);

	MD5Update(&md5, dest_str, dest_len);

	MD5Final(&md5, md5_value);

	// convert md5 value to md5 string
	for(i = 0; i < MD5_SIZE; i++)
	{
		sprintf(md5_str + i*2, "%02x", md5_value[i]);
	}

	return 0;
}

/**
 * compute the value of a file
 * @param  file_path
 * @param  md5_str
 * @return 0: ok, -1: fail
 */
int Compute_file_md5(const char *file_path, char *md5_str)
{
	int i;
	int fd;
	int ret;
	unsigned char data[READ_DATA_SIZE];
	unsigned char md5_value[MD5_SIZE];
	MD5_CTX md5;

#ifdef WIN32
	fd = open(file_path, O_RDONLY | O_BINARY);
#else
	fd = open(file_path, O_RDONLY);
#endif
	if (-1 == fd)
	{
		perror("open");
		return -1;
	}

	MD5Init(&md5);

	while (1)
	{
		ret = read(fd, data, READ_DATA_SIZE);
		if (-1 == ret)
		{
			perror("read");
			close(fd);
			return -1;
		}

		MD5Update(&md5, data, ret);

		if (0 == ret || ret < READ_DATA_SIZE)
		{
			break;
		}
	}

	close(fd);

	MD5Final(&md5, md5_value);

	// convert md5 value to md5 string
	for(i = 0; i < MD5_SIZE; i++)
	{
		sprintf(md5_str + i*2, "%02x", md5_value[i]);
	}

	return 0;
}

int process(const char *file, const struct stat *sb, int flag, struct FTW *s)
{
	//const char *name = file + s->base;
	const char *name = file + 2; // removed first two symbols: "./"
    const char * md5sumFilesPrefix = "./md5sum.";

    switch (flag)
    {
    case FTW_F:
        if (!validateAnalizingPath(file, s->base, flag))
			;
		else {
			//printf("---- %s\n", name);
			if (Compute_file_md5(file, md5_str))
			{
				puts("can not calculate md5");
				return FTW_STOP;
			}
			//printf("%s: %s\n", name, md5_str);
			snprintf(writeBuffer, WRITE_DATA_BUFFER_SIZE, "%s: %s\n", name, md5_str);
			write(fdMd5Detalized, writeBuffer, strlen(writeBuffer));
			return FTW_CONTINUE;
		}
		break;
    case FTW_D:
        //printf("directory %s\n", name);
        if (!strcmp(file, "."))
			return 0;
        if (!validateAnalizingPath(file, s->base, flag))
            ;
        else {
            snprintf(writeBuffer, WRITE_DATA_BUFFER_SIZE, "directory %s\n", name);
            if (write(fdMd5Detalized, writeBuffer, strlen(writeBuffer)) == -1)
            {
                perror("can not write md5 report (detalized)");
                return FTW_STOP;
            }
			return FTW_CONTINUE;
        }
        break;
    default:
        printf("oops... Error happened...\n");
        return FTW_STOP;
    }

	printf("FILTERED: \"%s\"\n", file);//
    return FTW_SKIP_SUBTREE;
}

/*int _process(const char *file, const struct stat *sb, int flag, struct FTW *s)
{
	int retval = 0;
	const char *name = file + s->base;

	printf("%*s", s->level * 4, "");	// indent over

	switch (flag) {
	case FTW_F:
		printf("%s (file)\n", name);
		break;
	case FTW_D:
		printf("%s (directory)\n", name);
		break;
	case FTW_DNR:
		printf("%s (unreadable directory)\n", name);
		break;
	case FTW_SL:
		printf("%s (symbolic link)\n", name);
		break;
	case FTW_NS:
		printf("%s (stat failed): %s\n", name, strerror(errno));
		break;
	case FTW_DP:
	case FTW_SLN:
		printf("%s: FTW_DP or FTW_SLN: can't happen!\n", name);
		retval = 1;
		break;
	default:
		printf("%s: unknown flag %d: can't happen!\n", name, flag);
		retval = 1;
		break;
	}

	return retval;
}*/

/*int _main(int argc, char *argv[])
{
    // TODO: https://github.com/1024sparrow/linux-programming-by-example/blob/master/book/ch08/ch08-nftw.c

	int ret;
	const char *file_path = "md5.c";
	char md5_str[MD5_STR_LEN + 1];
	const char *test_str = "gchinaran@gmail.com";

	// test file md5
	ret = Compute_file_md5(file_path, md5_str);
	if (0 == ret)
	{
		printf("[file - %s] md5 value:\n", file_path);
		printf("%s\n", md5_str);
	}

	// test string md5 
	Compute_string_md5((unsigned char *)test_str, strlen(test_str), md5_str);
	printf("[string - %s] md5 value:\n", test_str);
	printf("%s\n", md5_str);

	return 0;
}*/

int main(int argc, char **argv)
{
	int nfds;
	char start[PATH_MAX];
    const char *md5sumDetalizedPath = "md5sum.detalized.txt";
    const char *md5sumPath = "md5sum.txt";

	puts(
		"1024sparrow/md5sum program for calculating md5-sum for current directory\n"
		"source code: https://github.com/1024sparrow/md5sum\n"
		"version 1.0"
	);

    creat(
        md5sumDetalizedPath,
        S_IRUSR | S_IWUSR |
            S_IRGRP | S_IWGRP |
            S_IROTH | S_IWOTH
    );
    if ((fdMd5Detalized = open(md5sumDetalizedPath, O_WRONLY | O_TRUNC)) < 0)
    {
        perror("can not create detalized report");
        return 1;
    }

	getcwd(start, sizeof start);
	nfds = getdtablesize() - SPARE_FDS;	// leave some spare descriptors
    if (nftw(".", process, nfds, FTW_ACTIONRETVAL) != 0) {
        fprintf(stderr, "failed\n");
        return 1;
    }
    close(fdMd5Detalized);

    creat(
        md5sumPath,
        S_IRUSR | S_IWUSR |
            S_IRGRP | S_IWGRP |
            S_IROTH | S_IWOTH
    );
    if ((fdMd5Detalized = open(md5sumPath, O_WRONLY | O_TRUNC)) == -1)
    {
        perror("can not create sumary md5sum report");
        return 1;
    }
    if (Compute_file_md5(md5sumDetalizedPath, md5_str))
    {
        return 1;
    }
    snprintf(writeBuffer, WRITE_DATA_BUFFER_SIZE, "summary md5:  %s\n", md5_str);
    if (write(fdMd5Detalized, writeBuffer, strlen(writeBuffer)) == -1)
    {
        perror("can not write sumary md5");
        return 1;
    }
    close(fdMd5Detalized);

#ifdef WIN32
    printf(
        "Hash-sum for current directory analizer\n"
        "=======================================\n"
        //"Special edition for MP ASDC\n"
        //"\n"
        "Summary hash-sum: %s\n"
        "\n"
        "Press <ENTER> to exit...",
        md5_str
    );
    getchar();
#endif

	return 0;
}
