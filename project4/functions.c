#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "functions.h"
#include "list.h"
#include "cfs.h"

FILE * _fp = NULL;
Superblock * superblock = NULL;
int _wd = -1;
char _filename[1000];

void cfs_printcwd() {
    if (_fp) {
        char * block = initialize_block(superblock);

        read_block_by_number(_fp, _wd, block, superblock);

        char * dirname = block + sizeof (MDS);

        if (_wd == 1) {
            printf("inode: %d, %s:%s", _wd, _filename, dirname);
        } else {
            printf("inode: %d, %s: /.../%s", _wd, _filename, dirname);
        }

        destroy_block(block);
    }
}

void cfs_create(ListNode * options, char * filename) {
    int bs = 512, fns = 20, cfs = bs * 20, mdfn = 1001;

    printf("executing function cfs_create for file: %s ", filename);
    printf("\nwith options: \n\t");
    printL(options);
    printf("\n");

    ListNode * p = options;

    while (p != NULL) {
        char * key = p->data;
        p = p->next;
        char * value = p->data;
        p = p->next;

        if (strcmp(key, "-bs") == 0) {
            bs = atoi(value);
        } else if (strcmp(key, "-fns") == 0) {
            fns = atoi(value);
        } else if (strcmp(key, "-cfs") == 0) {
            cfs = atoi(value);
        } else if (strcmp(key, "-mdfn") == 0) {
            mdfn = atoi(value);
        } else {
            printf("syntax error \n");
            return;
        }
    }

    int maxblocknums_for_files = cfs / bs;
    int maxblocknums_for_dirs = mdfn / ((bs / (fns + sizeof (int))));
    int maxentries_per_block = bs / (fns + sizeof (int));

    if (maxblocknums_for_files <= 0) {
        printf("Values are invalid \n");
        return;
    }

    if (maxblocknums_for_dirs <= 0) {
        printf("Values are invalid \n");
        return;
    }

    printf("Creating file with the following arguments : \n");

    printf("blocksize                  = %d \n", bs);
    printf("filenamesize               = %d \n", fns);
    printf("maxfilesize                = %d \n", cfs);
    printf("maxdirectoryfilenumber     = %d \n", mdfn);

    FILE * fp = fopen(filename, "w+");

    Superblock superblock;
    superblock.bs = bs;
    superblock.fns = fns;
    superblock.cfs = cfs;
    superblock.mdfn = mdfn;
    superblock.maxblocknums_for_files = maxblocknums_for_files;
    superblock.maxblocknums_for_dirs = maxblocknums_for_dirs;
    superblock.maxentries_per_block = maxentries_per_block;

    char * block = initialize_block(&superblock);

    memcpy(block, &superblock, sizeof (superblock));

    write_block(fp, block, &superblock);

    destroy_block(block);

    // ----------------------------

    MDS mds;

    mds.nodeid = 0;
    mds.access_time = time(0);
    mds.creation_time = time(0);
    mds.modification_time = time(0);
    mds.parent_nodeid = -1;
    mds.size = bs;
    mds.type = 'D';

    block = initialize_block(&superblock);

    memcpy(block, &mds, sizeof (MDS));

    strcpy(block + sizeof (MDS), "/");

    int n = write_block(fp, block, &superblock);

    printf("Root block number is: %d \n", n);

    destroy_block(block);

    fclose(fp);
}

void cfs_workwith(char * filename) {
    if (_fp) {
        cfs_close();
        free(superblock);
        superblock = NULL;
    }

    printf("executing function cfs_workwith for filename: %s \n", filename);

    _fp = fopen(filename, "r+");

    if (!_fp) {
        printf("file not found \n");
        return;
    }

    superblock = malloc(sizeof (Superblock));

    fread(superblock, 1, sizeof (Superblock), _fp);

    printf("File system opened file with the following arguments : \n");

    printf("blocksize                  = %d \n", superblock->bs);
    printf("filenamesize               = %d \n", superblock->fns);
    printf("maxfilesize                = %d \n", superblock->cfs);
    printf("maxdirectoryfilenumber     = %d \n", superblock->mdfn);
    printf("maxblocknums_for_dirs      = %d \n", superblock->maxblocknums_for_dirs);
    printf("maxblocknums_for_files     = %d \n", superblock->maxblocknums_for_files);
    printf("maxentries_per_block       = %d \n", superblock->maxentries_per_block);

    _wd = 1;

    strcpy(_filename, filename);
    
    printf("Ready. \n");    
}

void cfs_close() {
    if (_fp) {
        fclose(_fp);
        _fp = NULL;
        _wd = -1;

        free(superblock);
        superblock = NULL;
    }
}

void cfs_pwd_rec(int currentDir) {
    char * block = initialize_block(superblock);

    read_block_by_number(_fp, currentDir, block, superblock);

    MDS mds;

    memcpy(&mds, block, sizeof (MDS));

    if (mds.parent_nodeid != -1) {
        cfs_pwd_rec(mds.parent_nodeid);
    }

    char * dirname = block + sizeof (MDS);

    if (strcmp(dirname, "/") == 0) {
        //        printf("");        
    } else {
        printf("/%s", dirname);
    }

    destroy_block(block);
}

void cfs_pwd() {
    if (_fp) {
        cfs_pwd_rec(_wd);
        printf("\n");
    } else {
        printf("Err: No filesystem has been opened \n");
    }
}

// returns blocknum with MDS of filename

int cfs_contains(int parentblocknum, char * queryentryname) {
    int i, j;
    char * block = initialize_block(superblock);

    read_block_by_number(_fp, parentblocknum, block, superblock);

    MDS mds;

    memcpy(&mds, block, sizeof (MDS));

    for (i = 0; i < superblock->maxblocknums_for_dirs; i++) {
        int datablock_num = get_datablock_num(block, i, superblock);

        if (datablock_num == 0) {
            continue;
        }

        char * datablock = initialize_block(superblock);

        read_block_by_number(_fp, datablock_num, datablock, superblock);

        for (j = 0; j < superblock->maxentries_per_block; j++) {
            int entryblock = get_entryblocknum(datablock, j, superblock);

            if (entryblock == 0) {
                continue;
            }

            char * entryname = get_entryname(datablock, j, superblock);

            if (strcmp(entryname, queryentryname) == 0) {
                return entryblock;
            }
        }


        destroy_block(datablock);
    }


    destroy_block(block);

    return -1;
}

int cfs_findemptydatastreamblock(int parentblocknum) {
    int i, j;
    char * block = initialize_block(superblock);

    read_block_by_number(_fp, parentblocknum, block, superblock);

    MDS mds;

    memcpy(&mds, block, sizeof (MDS));

    for (i = 0; i < superblock->maxblocknums_for_dirs; i++) {
        int datablock_num = get_datablock_num(block, i, superblock);

        if (datablock_num == 0) {
            continue;
        }

        char * datablock = initialize_block(superblock);

        read_block_by_number(_fp, datablock_num, datablock, superblock);

        for (j = 0; j < superblock->maxentries_per_block; j++) {
            int entryblock = get_entryblocknum(datablock, j, superblock);

            if (entryblock == 0) {
                return datablock_num;
            }
        }

        destroy_block(datablock);
    }

    destroy_block(block);

    return -1;
}

void cfs_mkdir(ListNode * directories) {
    printf("executing function cfs_mkdir for directories: \n\t");
    printL(directories);
    printf("\n");

    if (_fp) {
        ListNode * horizontalPointer = directories;

        if (horizontalPointer == NULL) {
            return;
        } else {
            while (horizontalPointer != NULL) {
                char *newentryname = horizontalPointer->data;

                printf("Trying to create: '%s'   ... \n", newentryname);

                // for each given directory name ...

                if (cfs_contains(_wd, newentryname) != -1) {
                    printf("Err: Entry %s already exists \n", newentryname);

                    horizontalPointer = horizontalPointer->next;
                    continue;
                }

                printf("Entry not found, so far so good. \n");

                int ds_n = cfs_findemptydatastreamblock(_wd);

                if (ds_n == -1) {
                    char * block = initialize_block(superblock);

                    ds_n = allocate_block(_fp, block, superblock);

                    destroy_block(block);


                    block = initialize_block(superblock);

                    read_block_by_number(_fp, _wd, block, superblock);

                    add_datablock_num_to_directory(block, ds_n, superblock);

                    write_block_by_number(_fp, _wd, block, superblock);

                }

                int mds_n;
                {
                    // add MDS data to new directory
                    char * block = initialize_block(superblock);

                    MDS mds;

                    mds.nodeid = 1473;
                    mds.access_time = time(0);
                    mds.creation_time = time(0);
                    mds.modification_time = time(0);
                    mds.parent_nodeid = _wd;
                    mds.size = superblock->bs;
                    mds.type = 'D';

                    memcpy(block, &mds, sizeof (MDS));

                    strcpy(block + sizeof (MDS), newentryname);

                    mds_n = allocate_block(_fp, block, superblock);

                    destroy_block(block);
                }

                {
                    // add pair entry,blocknum to DS
                    char * block = initialize_block(superblock);

                    read_block_by_number(_fp, ds_n, block, superblock);

                    add_entry_pair(block, newentryname, mds_n, superblock);

                    write_block_by_number(_fp, ds_n, block, superblock);

                    destroy_block(block);
                }


                horizontalPointer = horizontalPointer->next;
            }
        }
    } else {
        printf("Err: No filesystem has been opened \n");
    }
}

void cfs_cd(char * dirname) {
    printf("executing function cfs_cd for directory: \n\t");
    printf("\t%s\n", dirname);

    if (_fp) {
        printf("Trying to enter: '%s'   ... \n", dirname);

        if (strcmp(dirname, "..") == 0) {
            char * block = initialize_block(superblock);

            read_block_by_number(_fp, _wd, block, superblock);

            MDS mds;
            memcpy(&mds, block, sizeof (MDS));

            destroy_block(block);

            if (mds.parent_nodeid >= 1) {
                _wd = mds.parent_nodeid;
            } else {
                printf("Err: no parent exists. \n");
            }
        } else {
            // for each given directory name ...

            int targetblocknum = cfs_contains(_wd, dirname);
            if (targetblocknum == -1) {
                printf("Err: directory not found \n");
                return;
            }

            printf("Entry found, so far so good. \n");


            char * block = initialize_block(superblock);

            read_block_by_number(_fp, targetblocknum, block, superblock);

            MDS mds;
            memcpy(&mds, block, sizeof (MDS));

            destroy_block(block);

            if (mds.type != 'D') {
                printf("Err: entry is not a directory \n");
                return;
            }

            _wd = targetblocknum;
        }
    } else {
        printf("Err: No filesystem has been opened \n");
    }
}

void cfs_touch(ListNode * options, ListNode * files) {
    printf("executing function cfs_touch for files: \n\t");
    printL(files);
    printf("\nwith options: \n\t");
    printL(options);
    printf("\n");

    if (_fp) {
        ListNode * horizontalPointer = files;

        if (horizontalPointer == NULL) {
            return;
        } else {
            while (horizontalPointer != NULL) {
                char *newentryname = horizontalPointer->data;

                // for each given directory name ...

                int blocknum = cfs_contains(_wd, newentryname);

                if (blocknum != -1) {
                    if (containsL(options, "-m") || containsL(options, "-a")) {
                        char * block = initialize_block(superblock);

                        read_block_by_number(_fp, blocknum, block, superblock);

                        MDS * mds = (MDS*) block;

                        if (containsL(options, "-m")) {
                            printf("Updated modification time ...\n");
                            mds->modification_time = time(0);
                        }

                        if (containsL(options, "-a")) {
                            printf("Updated access time ... \n");
                            mds->access_time = time(0);
                        }

                        write_block_by_number(_fp, blocknum, block, superblock);

                        destroy_block(block);
                    } else {
                        printf("Err: file already exists \n");
                    }

                    horizontalPointer = horizontalPointer->next;
                    continue;
                }

                printf("Entry not found, so far so good. \n");

                int ds_n = cfs_findemptydatastreamblock(_wd);

                if (ds_n == -1) {
                    char * block = initialize_block(superblock);

                    ds_n = allocate_block(_fp, block, superblock);

                    destroy_block(block);


                    block = initialize_block(superblock);

                    read_block_by_number(_fp, _wd, block, superblock);

                   add_datablock_num_to_directory(block, ds_n, superblock);

                    write_block_by_number(_fp, _wd, block, superblock);

                }

                int mds_n;
                {
                    // add MDS data to new directory
                    char * block = initialize_block(superblock);

                    MDS mds;

                    mds.nodeid = 1473;
                    mds.access_time = time(0);
                    mds.creation_time = time(0);
                    mds.modification_time = time(0);
                    mds.parent_nodeid = _wd;
                    mds.size = 0;
                    mds.type = 'F';

                    memcpy(block, &mds, sizeof (MDS));

                    strcpy(block + sizeof (MDS), newentryname);

                    mds_n = allocate_block(_fp, block, superblock);

                    destroy_block(block);
                }

                {
                    // add pair entry,blocknum to DS
                    char * block = initialize_block(superblock);

                    read_block_by_number(_fp, ds_n, block, superblock);

                    add_entry_pair(block, newentryname, mds_n, superblock);

                    write_block_by_number(_fp, ds_n, block, superblock);

                    destroy_block(block);
                }


                horizontalPointer = horizontalPointer->next;
            }
        }
    } else {
        printf("Err: No filesystem has been opened \n");
    }
}

void cfs_ls_rec(ListNode * options, ListNode * files, int currentblocknum) {
    int i, j;
    char * block = initialize_block(superblock);

    read_block_by_number(_fp, currentblocknum, block, superblock);

    MDS mds;

    memcpy(&mds, block, sizeof (MDS));

    for (i = 0; i < superblock->maxblocknums_for_dirs; i++) {
        int datablock_num = get_datablock_num(block, i, superblock);

        if (datablock_num == 0) {
            continue;
        }

        char * datablock = initialize_block(superblock);

        read_block_by_number(_fp, datablock_num, datablock, superblock);

        for (j = 0; j < superblock->maxentries_per_block; j++) {
            char * entryname = get_entryname(datablock, j, superblock);
            int entryblock = get_entryblocknum(datablock, j, superblock);

            if (entryblock == 0) {
                continue;
            }

            {
                char * block = initialize_block(superblock);

                read_block_by_number(_fp, entryblock, block, superblock);

                MDS mds;
                memcpy(&mds, block, sizeof (MDS));

                char * filename = get_filename(block, superblock);

                char * copy = malloc(superblock->fns);
                strcpy(copy, filename);

                if (mds.type == 'D') {
                    strcat(copy, "/");
                }

                destroy_block(block);

                if (containsL(options, "-a") == 0 && copy[0] == '.') {
                    free(copy);
                    continue;
                }

                if (containsL(options, "-d") == 1 && mds.type != 'D') {
                    free(copy);
                    continue;
                }

                if (containsL(options, "-h") == 1 && mds.type != 'L') {
                    free(copy);
                    continue;
                }


                if (countL(files) == 0 || (countL(files) > 0 && containsL(files, filename))) {
                    if (containsL(options, "-l") == 1) {
                        char buff_a[20];
                        char buff_m[20];
                        char buff_c[20];
                        strftime(buff_a, 20, "%Y-%m-%d %H:%M:%S", localtime(&mds.access_time));
                        strftime(buff_m, 20, "%Y-%m-%d %H:%M:%S", localtime(&mds.modification_time));
                        strftime(buff_c, 20, "%Y-%m-%d %H:%M:%S", localtime(&mds.creation_time));

                        printf("%-15s %-15s %5dB p:%d c:%s a:%s m:%s\n",
                                entryname,
                                copy,
                                mds.size,
                                mds.parent_nodeid,
                                buff_c,
                                buff_a,
                                buff_m
                                );
                    } else {
                        printf("%-15s %-15s %5dB p:%d\n", entryname, copy, (int) mds.size, mds.parent_nodeid);
                    }

                    if (containsL(options, "-r") == 1 && mds.type == 'D') {
                        printf(" ================ Recursion on: %s \n", filename);
                        cfs_ls_rec(options, files, entryblock);
                    }

                }
                free(copy);

            }
        }

        destroy_block(datablock);
    }

    destroy_block(block);
}

void cfs_ls(ListNode * options, ListNode * files) {
    printf("executing function cfs_ls for files: \n\t");
    printL(files);
    printf("\nwith options: \n\t");
    printL(options);
    printf("\n");

    if (_fp) {
        cfs_ls_rec(options, files, _wd);
    } else {
        printf("Err: No filesystem has been opened \n");
    }

}

void cfs_cp(ListNode * options, ListNode * sources, char * destination) {
    printf("executing function cfs_cp for source or sources: \n\t");
    printL(sources);
    printf("\nwith options: \n\t");
    printL(options);
    printf("\n");
    printf("\n to Destination or Directory:  %s\n\t",destination);
   
    
}

void cfs_cat(ListNode * sources, char * file) {


}

void cfs_ln(ListNode * sources, char * file) {
    printf("executing function cfs_ln for sources: \n\t");
    printL(sources);
    printf("\nOutput: \n\t");
    printf("Output: %s", file);
    printf("\n");

    if (_fp) {
        // first check that file exists

        int targetblocknum = cfs_contains(_wd, file);

        if (targetblocknum == -1) {
            printf("Err: entry does not exist with such name\n");
            return;
        }

        // then for each link, create a symbolic link:

        ListNode * horizontalPointer = sources;

        if (horizontalPointer == NULL) {
            return;
        } else {
            while (horizontalPointer != NULL) {
                char *newentryname = horizontalPointer->data;

                // for each given directory name ...

                int blocknum = cfs_contains(_wd, newentryname);

                if (blocknum != -1) {
                    printf("Err: entry already exists with name : %s \n", newentryname);

                    horizontalPointer = horizontalPointer->next;
                    continue;
                }

                printf("Entry %s not found, so far so good. \n", newentryname);

                int ds_n = cfs_findemptydatastreamblock(_wd);

                if (ds_n == -1) {
                    char * block = initialize_block(superblock);

                    ds_n = allocate_block(_fp, block, superblock);

                    destroy_block(block);


                    block = initialize_block(superblock);

                    read_block_by_number(_fp, _wd, block, superblock);

                    add_datablock_num_to_directory(block, ds_n, superblock);

                    write_block_by_number(_fp, _wd, block, superblock);

                }

                {
                    // add pair entry,blocknum to DS
                    char * block = initialize_block(superblock);

                    read_block_by_number(_fp, ds_n, block, superblock);

                    add_entry_pair(block, newentryname, targetblocknum, superblock);

                    write_block_by_number(_fp, ds_n, block, superblock);

                    destroy_block(block);
                }

                horizontalPointer = horizontalPointer->next;
            }
        }
    } else {
        printf("Err: No filesystem has been opened \n");
    }

}

void cfs_mv(ListNode * options,ListNode * sources, char * destination){
    printf("executing function cfs_cp for source or sources: \n\t");
    printL(sources);
    printf("\nwith options: \n\t");
    printL(options);
    printf("\n");
    printf("\n to Destination or Directory:  %s\n\t",destination);
   
}

void cfs_rm(ListNode * options, ListNode * destinations) {
    printf("executing function cfs_ls for files: \n\t");
    printL(destinations);
    printf("\nwith options: \n\t");
    printL(options);
    printf("\n");
}

void cfs_import(ListNode * sources, char * directory)   {
    printf("executing function cfs_import for files/directories: \n\t");
    printL(sources);
    printf("\nto Directory: %s\n\t",directory);
    printf("\n");
}

void cfs_export(ListNode * sources, char * directory)   {
    printf("executing function cfs_export for files/directories: \n\t");
    printL(sources);
    printf("\nto Directory: %s\n\t",directory);
    printf("\n");
}
