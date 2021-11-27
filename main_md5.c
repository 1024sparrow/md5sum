#include "md5.h"

#define _XOPEN_SOURCE 1			/* Required under GLIBC for nftw() */
#define _XOPEN_SOURCE_EXTENDED 1	/* Same */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for getdtablesize(), getcwd() declarations
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <errno.h>
#include <ftw.h>
#include <limits.h>     /* for PATH_MAX */

#define READ_DATA_SIZE	1024
#define MD5_SIZE		16
#define MD5_STR_LEN		(MD5_SIZE * 2)

#define SPARE_FDS 5	/* fds for use by other functions, see text */

// function declare
int Compute_string_md5(unsigned char *dest_str, unsigned int dest_len, char *md5_str);
int Compute_file_md5(const char *file_path, char *md5_str);


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
		//snprintf(md5_str + i*2, 2+1, "%02x", md5_value[i]); // C99-specific
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

	// init md5
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
		//snprintf(md5_str + i*2, 2+1, "%02x", md5_value[i]); // C99-specific
		sprintf(md5_str + i*2, "%02x", md5_value[i]);
	}

	return 0;
}

static char md5_str[MD5_STR_LEN + 1];

int process(const char *file, const struct stat *sb, int flag, struct FTW *s)
{
	//const char *name = file + s->base;
	const char *name = file;

    switch (flag)
    {
    case FTW_F:
        if (Compute_file_md5(file, md5_str))
        {
            return 1;
        }
        printf("%s: %s\n", name, md5_str);
        break;
    case FTW_D:
        printf("directory %s\n", name);
        break;
    default:
        printf("oops... Error happened...\n");
        return 1;
    }

    return 0;
}

int _process(const char *file, const struct stat *sb, int flag, struct FTW *s)
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
}

int _main(int argc, char *argv[])
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
}

int main(int argc, char **argv)
{
	int nfds;
	int flags = FTW_PHYS;
	char start[PATH_MAX];

	getcwd(start, sizeof start);
	nfds = getdtablesize() - SPARE_FDS;	// leave some spare descriptors
    if (nftw(".", process, nfds, flags) != 0) {
        fprintf(stderr, "failed\n");
        return 1;
    }

	return 0;
}
