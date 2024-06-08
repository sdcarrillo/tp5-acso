#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"


int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    struct inode ind;
    int result = inode_iget(fs, inumber, &ind);
    if (result < 0) {
        return -1;
    }

    int physical_block = inode_indexlookup(fs, &ind, blockNum);
    if (physical_block < 0) {
        return -1;
    }

    int content_block = diskimg_readsector(fs->dfd, physical_block, buf);
    if (content_block < 0) {
        return -1;
    }

    int valid_bytes = inode_getsize(&ind);
    if (valid_bytes < 0){
        return -1;
    }

    int total_blocks = valid_bytes / DISKIMG_SECTOR_SIZE;
    if (blockNum != total_blocks){ // Si no estoy en el ultimo bloque entonces el bloque está lleno
        return DISKIMG_SECTOR_SIZE;
    } else {
        int bytes_in_last_block = valid_bytes % DISKIMG_SECTOR_SIZE;
        return bytes_in_last_block == 0 ? DISKIMG_SECTOR_SIZE : bytes_in_last_block;
    }

    return 0;
}

