
#ifndef CFS_H
#define CFS_H

#include <time.h>


#define DATABLOCK_NUM 100

typedef struct Superblock {
    int bs;
    int fns;
    int cfs;
    int mdfn;
    int maxblocknums_for_files;
    int maxblocknums_for_dirs;
    int maxentries_per_block;
} Superblock;

typedef struct {
    unsigned int nodeid;    
    unsigned int size;
    unsigned int type;
    unsigned int parent_nodeid;
    time_t creation_time;
    time_t access_time;
    time_t modification_time;
} MDS;


char * initialize_block(Superblock * superblock);

void destroy_block(char * block);

int allocate_block(FILE * fp, char * block, Superblock * superblock);

int write_block(FILE * fp, char * block, Superblock * superblock);

int read_block(FILE * fp, char * block, Superblock * superblock);

void read_block_by_number(FILE * fp, int blocknum, char * block, Superblock * superblock);

void write_block_by_number(FILE * fp, int blocknum, char * block, Superblock * superblock);

int get_datablock_num(char * block, int datablock_num, Superblock * superblock) ;

int add_datablock_num_to_directory(char * block, int newdatablocknum, Superblock * superblock);

char * get_filename(char * block, Superblock * superblock);

// ----------------------------------------------------

char *get_entryname(char * block, int entry_num, Superblock * superblock);

int get_entryblocknum(char * block, int entry_num, Superblock * superblock);

int add_entry_pair(char * block, char * entryname, int entry_num, Superblock * superblock);


char *set_entryname(char * block, int offset, char * entryname, Superblock * superblock);

int set_entryblocknum(char * block, int offset, int entry_num, Superblock * superblock);



#endif /* CFS_H */

