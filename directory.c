#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int directory_findname(struct unixfilesystem *fs, const char *name, int dirinumber, struct direntv6 *dirEnt) {

  struct inode dir_inode;
  int result = inode_iget(fs, dirinumber, &dir_inode);

  if (result < 0) {
    return -1;
  }

  if ((dir_inode.i_mode & IFMT) == IFDIR){ // Es un directorio
    
    int dir_size = inode_getsize(&dir_inode);
    int numBlocks = (dir_size + DISKIMG_SECTOR_SIZE - 1) / DISKIMG_SECTOR_SIZE;
  
    for (int i = 0; i < numBlocks; i++) {

      struct direntv6 dir[DISKIMG_SECTOR_SIZE / sizeof(struct direntv6)];
      int active_bytes = file_getblock(fs, dirinumber, i, dir);
      if (active_bytes < 0) {
        return -1;
      }
      int block_entries = active_bytes / sizeof(struct direntv6);

      for (int j = 0; j < block_entries; j++) {
        if (strcmp(dir[j].d_name, name) == 0) {
          *dirEnt = dir[j];
          return 0;

        }
      }
    }

  } else {
    return -1;
  }
  return -1; // Es un directorio pero no estaba ese name presente
}
