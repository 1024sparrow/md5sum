#include <ftw.h>
#include <string.h>
#include <stdlib.h> // exit()

int
validateAnalizingPath(
    const char *path,
    int baseNameIndex,
    int fileType // FTW_F, FTW_D &etc. declared in <ftw.h>
){
    const char * md5sumFilesPrefix = "./.git/";
    if (!strncmp(path, md5sumFilesPrefix, strlen(md5sumFilesPrefix))){
        return 0;
    }

    md5sumFilesPrefix = "./.git";
    if (!strcmp(path, md5sumFilesPrefix)){
        return 0;
    }
    //return 1;//

    struct descr {
        enum {
            FILETYPE_ANY,
            FILETYPE_FILE,
            FILETYPE_DIR
        } fileType;
        enum {
            FILTERTYPE_FULLPATH_EQUAL = 1,
            FILTERTYPE_FULENAME_EQUAL = 2,
            FILTERTYPE_FULLPATH_STARTS_WITH = 3,
            FILTERTYPE_FULENAME_STARTS_WITH = 4
        } filterType;
        const char * filename;
    } descr [] = {
        {
            FILETYPE_FILE,
            FILTERTYPE_FULLPATH_EQUAL,
            "./md5sum"
        },
        {
            FILETYPE_FILE,
            FILTERTYPE_FULLPATH_EQUAL,
            "./md5sum-win64.exe"
        },
        {
            FILETYPE_FILE,
            FILTERTYPE_FULLPATH_EQUAL,
            "./md5sum.detalized.txt"
        },
        {
            FILETYPE_FILE,
            FILTERTYPE_FULLPATH_EQUAL,
            "./md5sum.txt"
        },
        #include "SRC"
    };
    int i;
    struct descr *oDescr;
    for (i = 0, oDescr = &descr ; i < sizeof(descr)/sizeof(struct descr) ; ++i, ++oDescr){
        if (oDescr->fileType != FILETYPE_ANY){
            switch (oDescr->fileType){
            case FILETYPE_FILE:
                if (fileType != FTW_F){
                    continue;
                }
                break;
            case FILETYPE_DIR:
                if (fileType != FTW_D){
                    continue;
                }
                break;
            default:
                puts("NOT IMPLEMENTED. ERROR.");
                exit(1);
            }
        }
        const char *actualName = path;
        if (oDescr->filterType & 2){
            actualName += baseNameIndex;
        }

        if (oDescr->filterType & 1){
            if (!strcmp(actualName, oDescr->filename)){
                //printf("filtered file: \"%s\"\n", path);//
                return 0;
            }
        }
        else{
            if (!strncmp(actualName, oDescr->filename, strlen(oDescr->filename))){
                //printf("filtered file: \"%s\"\n", path);//
                return 0;
            }
        }
    }

    return 1;
}
