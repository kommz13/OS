#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "list.h"

void cfs_printcwd();

void cfs_workwith(char * filename);

void cfs_create(ListNode * options, char * file);

void cfs_close();

void cfs_pwd();

void cfs_mkdir(ListNode * directories);

void cfs_cd(char * filename);

// -------------------------------------------

void cfs_touch(ListNode * options, ListNode * files);


void cfs_ls(ListNode * options, ListNode * files);

void cfs_cp(ListNode * options, ListNode * sources, char * destination);

void cfs_mv(ListNode * options,ListNode * sources, char * destination);

void cfs_rm(ListNode * options, ListNode * destinations); 

void cfs_cat(ListNode * sources, char * file);

void cfs_ln(ListNode * sources, char * file);

void cfs_import(ListNode * sources, char * directory);

void cfs_export(ListNode * sources, char * directory);

#endif