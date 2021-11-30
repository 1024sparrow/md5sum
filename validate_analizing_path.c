#include <ftw.h>
#include <string.h>

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
        /*{
            FILETYPE_DIR,
            FILTERTYPE_FULLPATH_EQUAL,
            "./Journal"
        }*/
        #include "SRC"
    };
    int i;
    struct descr *oDescr;
    for (i = 0, oDescr = &descr ; i < sizeof(descr)/sizeof(struct descr) ; ++i, ++oDescr){
        if (oDescr->fileType != FILETYPE_ANY){
            puts("NOT IMPLEMENTED. ERROR.");
            return 0;
        }
        const char *actualName = path;
        if (oDescr->filterType & 2){
            actualName += baseNameIndex;
        }

        if (oDescr->filterType & 1){
            if (!strcmp(actualName, oDescr->filename)){
                return 0;
            }
        }
        else{
            if (!strncmp(actualName, oDescr->filename, strlen(oDescr->filename))){
                return 0;
            }
        }
    }

    return 1;
}
