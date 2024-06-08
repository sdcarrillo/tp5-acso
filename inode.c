#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"

int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    
    int inodes_per_block = DISKIMG_SECTOR_SIZE / sizeof(struct inode);
    int inode_block = (inumber - 1) / inodes_per_block + INODE_START_SECTOR; // El primer inode es 1

    struct inode inodes_block[inodes_per_block];
    int bytes_read = diskimg_readsector(fs->dfd, inode_block, inodes_block);
    if (bytes_read < 0) {
        return -1; 
    }

    int inode_index = (inumber - 1) % inodes_per_block;
    *inp = inodes_block[inode_index];

    return 0;  
}

int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {  

    if (inp->i_mode & ILARG) {

        int pointers_per_block = DISKIMG_SECTOR_SIZE / sizeof(uint16_t);
        int pointers_in_total = pointers_per_block * 7;

        if (blockNum < pointers_in_total){

            uint16_t pointer_block[pointers_per_block];
            int index_block = blockNum / pointers_per_block;
            int bytes_read = diskimg_readsector(fs->dfd, inp->i_addr[index_block], pointer_block);
            if (bytes_read < 0) {
                return -1;
            }
            int inode_index = blockNum % pointers_per_block;
            return pointer_block[inode_index];

        } else {

            uint16_t pointer_block[pointers_per_block];
            int read_1 = diskimg_readsector(fs->dfd, inp->i_addr[7], pointer_block);
            if (read_1 < 0) {
                return -1;
            }
            int index_block = blockNum - pointers_in_total; // Resto por los bloques del indexado simple
            index_block = index_block / pointers_per_block;
            
            uint16_t pointer_block_2[pointers_per_block];
            int read_2 = diskimg_readsector(fs->dfd, pointer_block[index_block], pointer_block_2);

            if (read_2 < 0) {
                return -1;
            }

            int inode_index = (blockNum - pointers_in_total) % pointers_per_block;
            return pointer_block_2[inode_index];

        }

    } else {
        return inp->i_addr[blockNum];
    }
    return 0;
}

int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}
