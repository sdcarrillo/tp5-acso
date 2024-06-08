#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {

    int inumber = ROOT_INUMBER;
    char component[14]; // tamaño maximo de nombre de archivo/subdirectorio

    if (strcmp(pathname, "/") == 0) {
        return inumber;
    }

    char *path_copy = strdup(pathname);
    if (path_copy == NULL) {
        return -1;
    }

    char *next_token = strtok(path_copy, "/");
    while (next_token != NULL) {

        if (strlen(next_token) > 14) {
            free(path_copy);
            return -1;
        }
        strcpy(component, next_token);
        struct direntv6 dir;
        int result = directory_findname(fs, component, inumber, &dir);
        if (result < 0) {
            free(path_copy);
            return -1;
        }
        inumber = dir.d_inumber;
        next_token = strtok(NULL, "/");
        
    }

    free(path_copy);
    return inumber;
}
