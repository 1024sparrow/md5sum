#include <ftw.h>
#include <string.h>

int
validateAnalizingPath(
    const char *path,
    int baseNameIndex,
    int fileType // FTW_F, FTW_D &etc. declared in <ftw.h>
){
    const char * md5sumFilesPrefix = "./md5sum.";
    if (!strncmp(path, md5sumFilesPrefix, strlen(md5sumFilesPrefix))){
        return 0;
    }
    return 1;
}
