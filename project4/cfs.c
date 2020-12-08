
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cfs.h"

char * initialize_block(Superblock * superblock) {
    char * block = malloc(sizeof (char)*superblock->bs);
    memset(block, 0, superblock->bs);
    return block;
}

void destroy_block(char * block) {
    free(block);
}

int allocate_block(FILE * fp, char * block, Superblock * superblock) {
    fseek(fp, 0, SEEK_END);

    printf("New block allocated \n");

    return write_block(fp, block, superblock);
}

int write_block(FILE * fp, char * block, Superblock * superblock) {
    fwrite(block, superblock->bs, 1, fp);

    long offset = ftell(fp);

    int blocknum = offset / superblock->bs;

    return blocknum - 1;
}

int read_block(FILE * fp, char * block, Superblock * superblock) {
    fread(block, superblock->bs, 1, fp);

    long offset = ftell(fp);

    int blocknum = offset / superblock->bs;

    return blocknum - 1;
}

// Accessors for all blocks

void read_block_by_number(FILE * fp, int blocknum, char * block, Superblock * superblock) {
    fseek(fp, blocknum * superblock->bs, SEEK_SET);
    read_block(fp, block, superblock);
}

void write_block_by_number(FILE * fp, int blocknum, char * block, Superblock * superblock) {
    fseek(fp, blocknum * superblock->bs, SEEK_SET);
    write_block(fp, block, superblock);
}

// Accessors for all MDS

int get_datablock_num(char * block, int datablock_offset, Superblock * superblock) {
    return *((int*) (block + sizeof (MDS) + superblock->fns + sizeof (int)*datablock_offset));
}

int add_datablock_num_to_directory(char * block, int newdatablocknum, Superblock * superblock) {
    int datablock_offset;

    for (datablock_offset = 0; datablock_offset < superblock->maxblocknums_for_dirs; datablock_offset++) {
        int datablock_num = get_datablock_num(block, datablock_offset, superblock);

        if (datablock_num == 0) {
            *((int*) (block + sizeof (MDS) + superblock->fns + sizeof (int)*datablock_offset)) = newdatablocknum;
            return datablock_offset;
        }

    }
    return -1;
}

char * get_filename(char * block, Superblock * superblock) {
    return block + sizeof (MDS);
}



// ------------------------------------------------------------------------
//                              Entry block
// ------------------------------------------------------------------------

char *get_entryname(char * block, int entry_num, Superblock * superblock) {
    return block + (superblock->fns + sizeof (int)) * entry_num;
}

int get_entryblocknum(char * block, int entry_num, Superblock * superblock) {
    return *((int*) (block + (superblock->fns + sizeof (int)) * entry_num + superblock->fns));
}

char *set_entryname(char * block, int entry_num, char * entryname, Superblock * superblock) {
    strcpy(block + (superblock->fns + sizeof (int)) * entry_num, entryname);

    return block + (superblock->fns + sizeof (int)) * entry_num;
}

int set_entryblocknum(char * block, int offset, int new_entry_num, Superblock * superblock) {
    *((int*) (block + (superblock->fns + sizeof (int)) * offset + superblock->fns)) = new_entry_num;
    return 0;
}

int add_entry_pair(char * datablock, char * entryname, int entry_num, Superblock * superblock) {
    int j;

    for (j = 0; j < superblock->maxentries_per_block; j++) {

        int entryblock = get_entryblocknum(datablock, j, superblock);

        if (entryblock == 0) {
            set_entryname(datablock, j, entryname, superblock);
            set_entryblocknum(datablock, j, entry_num, superblock);
            return j;
        }
    }

    printf("Max FS limit reached, please download latest patch \n");

    exit(1);
}