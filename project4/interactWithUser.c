/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <string.h>
#include <stdio.h>

#include "list.h"
#include "functions.h"

void interactWithUser() {
    char buffer[5000];

    // initialize for dewbugging
    {
        cfs_create(NULL, "boo.cfs");
        cfs_workwith("boo.cfs");

        ListNode * directories = initializeL();
        insertL(&directories, "music");
        insertL(&directories, "books");
        insertL(&directories, "movies");
        cfs_mkdir(directories);
        cleanupL(directories);

        ListNode * files = initializeL();
        insertL(&files, "a");
        insertL(&files, "b");
        insertL(&files, "c");
        cfs_touch(NULL, files);
        cleanupL(files);
    }

    while (1) {
        cfs_printcwd();

        printf("$ ");
        if (!fgets(buffer, sizeof (buffer), stdin)) {
            break;
        }

        int length = strlen(buffer);
        buffer[length - 1] = '\0';

        if (length < 3) {
            printf("unknown first token \n");
            continue;
        }
        char * firsttoken = strtok(buffer, " ");

        if (strcmp(firsttoken, "cfs_workwith") == 0 || strcmp(firsttoken, "workwith") == 0) {
            printf("first token correct: cfs_workwith \n");

            char * secondtoken = strtok(NULL, " ");

            if (secondtoken == NULL) {
                printf("second token invalid \n");
            } else {
                cfs_workwith(secondtoken);
            }
        } else if (strcmp(firsttoken, "cfs_mkdir") == 0 || strcmp(firsttoken, "mkdir") == 0) {
            printf("first token correct: cfs_mkdir \n");

            ListNode * directories = initializeL();

            while (1) {
                char * anothertoken = strtok(NULL, " ");
                if (anothertoken == NULL) {
                    break;
                } else {
                    insertL(&directories, anothertoken);
                }
            }

            if (countL(directories) == 0) {
                printf("tokens missing \n");
            } else {
                cfs_mkdir(directories);
            }

            cleanupL(directories);
        } else if (strcmp(firsttoken, "cfs_touch") == 0 || strcmp(firsttoken, "touch") == 0) {
            printf("first token correct: cfs_touch \n");

            ListNode * options = initializeL();
            ListNode * files = initializeL();

            while (1) {
                char * anothertoken = strtok(NULL, " ");
                if (anothertoken == NULL) {
                    break;
                } else {
                    if (anothertoken[0] == '-') {
                        insertL(&options, anothertoken);
                    } else {
                        insertL(&files, anothertoken);
                    }
                }
            }

            if (countL(files) == 0) {
                printf("tokens missing \n");
            } else {
                cfs_touch(options, files);
            }

            cleanupL(options);
            cleanupL(files);
        } else if (strcmp(firsttoken, "cfs_pwd") == 0 || strcmp(firsttoken, "pwd") == 0) {
            cfs_pwd();
        } else if (strcmp(firsttoken, "cfs_cd") == 0 || strcmp(firsttoken, "cd") == 0) {
            printf("first token correct: cfs_cd \n");

            char * secondtoken = strtok(NULL, " ");

            if (secondtoken == NULL) {
                printf("second token invalid \n");
            } else {
                cfs_cd(secondtoken);
            }
        } else if (strcmp(firsttoken, "cfs_ls") == 0 || strcmp(firsttoken, "ls") == 0) {
            printf("first token correct: cfs_ls \n");

            ListNode * options = initializeL();
            ListNode * files = initializeL();

            while (1) {
                char * anothertoken = strtok(NULL, " ");
                if (anothertoken == NULL) {
                    break;
                } else {
                    if (anothertoken[0] == '-') {
                        insertL(&options, anothertoken);
                    } else {
                        insertL(&files, anothertoken);
                    }
                }
            }

            cfs_ls(options, files);

            cleanupL(options);
            cleanupL(files);
        } else if (strcmp(firsttoken, "cfs_ll") == 0 || strcmp(firsttoken, "ll") == 0) {
            printf("first token correct: cfs_ls \n");

            ListNode * options = initializeL();
            ListNode * files = initializeL();

            while (1) {
                char * anothertoken = strtok(NULL, " ");
                if (anothertoken == NULL) {
                    break;
                } else {
                    if (anothertoken[0] == '-') {
                        insertL(&options, anothertoken);
                    } else {
                        insertL(&files, anothertoken);
                    }
                }
            }

            if (!containsL(options, "-l")) {
                insertL(&options, "-l");
            }

            cfs_ls(options, files);

            cleanupL(options);
            cleanupL(files);
        } else if (strcmp(firsttoken, "cfs_la") == 0 || strcmp(firsttoken, "la") == 0) {
            printf("first token correct: cfs_la \n");

            ListNode * options = initializeL();
            ListNode * files = initializeL();

            while (1) {
                char * anothertoken = strtok(NULL, " ");
                if (anothertoken == NULL) {
                    break;
                } else {
                    if (anothertoken[0] == '-') {
                        insertL(&options, anothertoken);
                    } else {
                        insertL(&files, anothertoken);
                    }
                }
            }

            if (!containsL(options, "-a")) {
                insertL(&options, "-a");
            }

            cfs_ls(options, files);

            cleanupL(options);
            cleanupL(files);

        } else if (strcmp(firsttoken, "cfs_cp") == 0 || strcmp(firsttoken, "cp") == 0) {
            printf("first token correct: cfs_cp \n");

            ListNode * options = initializeL();
            ListNode * temp = initializeL();
            ListNode * sources = initializeL();
            char * destination = NULL;
            
            while (1) {
                char * anothertoken = strtok(NULL, " ");
                if (anothertoken == NULL) {
                    break;
                } else {
                    if (anothertoken[0] == '-') {
                        if(strcmp(anothertoken, "-r") == 0 ||strcmp(anothertoken, "-i") == 0||strcmp(anothertoken, "-l") == 0||strcmp(anothertoken, "-R") == 0){
                            insertL(&options, anothertoken);
                        }else{
                            printf("Invalid <OPTIONS>. Try again! \n\n");
                            break;
                        }
                     }
                     else {
                        insertL(&temp, anothertoken);
                        destination = anothertoken;
                    }
                }
            }

             if (countL(temp) == 0 || destination == NULL) {
                printf("tokens missing \n");
            } else {
                int length = countL(temp);

                while (temp != NULL && length > 1) {
                    insertL(&sources, temp->data);
                    length--;
                    temp = temp->next;
                }

                cfs_cp(options, sources, destination);
            }
            cleanupL(options);
            cleanupL(temp);
            cleanupL(sources);
           
            
        } else if (strcmp(firsttoken, "cfs_cat") == 0 || strcmp(firsttoken, "cat") == 0) {
            printf("first token correct: cfs_cat \n");

            ListNode * sources = initializeL();
            char * output = NULL;

            int flag_minus_o_found = 0;

            while (1) {
                char * anothertoken = strtok(NULL, " ");
                if (anothertoken == NULL) {
                    break;
                } else {
                    if (anothertoken[0] == '-' && anothertoken[1] == 'o') {
                        flag_minus_o_found = 1;
                    } else {
                        if (flag_minus_o_found == 0) {
                            insertL(&sources, anothertoken);
                        } else {
                            output = anothertoken;
                            break;
                        }
                    }
                }
            }

            if (countL(sources) == 0 || output == NULL) {
                printf("tokens missing \n");
            } else {
                cfs_cat(sources, output);
            }

            cleanupL(sources);
        } else if (strcmp(firsttoken, "cfs_ln") == 0 || strcmp(firsttoken, "ln") == 0) {

            printf("first token correct: cfs_ln \n");

            ListNode * temp = initializeL();
            ListNode * sources = initializeL();
            char * output = NULL;

            while (1) {
                char * anothertoken = strtok(NULL, " ");

                if (anothertoken == NULL) {
                    break;
                } else {
                    insertL(&temp, anothertoken);
                    output = anothertoken;
                }
            }

            if (countL(temp) == 0 || output == NULL) {
                printf("tokens missing \n");
            } else {
                int length = countL(temp);

                ListNode * n = temp;

                while (temp != NULL && length > 1) {
                    insertL(&sources, temp->data);
                    length--;
                    temp = temp->next;
                }

                cfs_ln(sources, output);
            }

            cleanupL(temp);
            cleanupL(sources);
        } else if (strcmp(firsttoken, "cfs_mv") == 0 || strcmp(firsttoken, "mv") == 0) {
           printf("first token correct: cfs_mv \n");

            ListNode * options = initializeL();
            ListNode * temp = initializeL();
            ListNode * sources = initializeL();
            char * destination = NULL;
            
            while (1) {
                char * anothertoken = strtok(NULL, " ");
                if (anothertoken == NULL) {
                    break;
                } else {
                    if (anothertoken[0] == '-') {
                        if(strcmp(anothertoken, "-i") == 0){
                            insertL(&options, anothertoken);
                        }else{
                            printf("Invalid <OPTIONS>. Try again! \n\n");
                            break;
                        }
                     }
                     else {
                        insertL(&temp, anothertoken);
                        destination = anothertoken;
                    }
                }
            }

             if (countL(temp) == 0 || destination == NULL) {
                printf("tokens missing \n");
            } else {
                int length = countL(temp);

                while (temp != NULL && length > 1) {
                    insertL(&sources, temp->data);
                    length--;
                    temp = temp->next;
                }

                cfs_mv(options, sources, destination);
            }
            cleanupL(options);
            cleanupL(temp);
            cleanupL(sources);
           
        } else if (strcmp(firsttoken, "cfs_rm") == 0 || strcmp(firsttoken, "rm") == 0) {
            printf("first token correct: cfs_rm \n");

            ListNode * options = initializeL();
            ListNode * destinations = initializeL();

            while (1) {
                char * anothertoken = strtok(NULL, " ");
                if (anothertoken == NULL) {
                    break;
                } else {
                    if (anothertoken[0] == '-') {
                        insertL(&options, anothertoken);
                    } else {
                        insertL(&destinations, anothertoken);
                    }
                }
            }

            if (countL(destinations) == 0) {
                printf("tokens missing \n");
            } else {
                cfs_rm(options, destinations);
            }

            cleanupL(options);
            cleanupL(destinations);
        } else if (strcmp(firsttoken, "cfs_import") == 0 || strcmp(firsttoken, "import") == 0) {
            printf("first token correct: cfs_import \n");

            ListNode * temp = initializeL();
            ListNode * sources = initializeL();
            char * directory = NULL;

            while (1) {
                char * anothertoken = strtok(NULL, " ");

                if (anothertoken == NULL) {
                    break;
                } else {
                    insertL(&temp, anothertoken);
                    directory = anothertoken;
                }
            }

            if (countL(temp) == 0 || directory == NULL) {
                printf("tokens missing \n");
            } else {
                int length = countL(temp);

                while (temp != NULL && length > 1) {
                    insertL(&sources, temp->data);
                    length--;
                    temp = temp->next;
                }

                cfs_import(sources, directory);
            }

            cleanupL(temp);
            cleanupL(sources);
        } else if (strcmp(firsttoken, "cfs_export") == 0 || strcmp(firsttoken, "export") == 0) {
            printf("first token correct: cfs_export \n");

            ListNode * temp = initializeL();
            ListNode * sources = initializeL();
            char * directory = NULL;

            while (1) {
                char * anothertoken = strtok(NULL, " ");

                if (anothertoken == NULL) {
                    break;
                } else {
                    insertL(&temp, anothertoken);
                    directory = anothertoken;
                }
            }

            if (countL(temp) == 0 || directory == NULL) {
                printf("tokens missing \n");
            } else {
                int length = countL(temp);

                while (temp != NULL && length > 1) {
                    insertL(&sources, temp->data);
                    length--;
                    temp = temp->next;
                }

                cfs_export(sources, directory);
            }

            cleanupL(temp);
            cleanupL(sources);
        } else if (strcmp(firsttoken, "cfs_create") == 0 || strcmp(firsttoken, "create") == 0) {
            printf("first token correct: cfs_create \n");

            ListNode * options = initializeL();
            char * file = NULL;

            while (1) {
                char * anothertoken = strtok(NULL, " ");
                if (anothertoken == NULL) {
                    break;
                } else {
                    if (anothertoken[0] == '-') {
                        insertL(&options, anothertoken);
                        anothertoken = strtok(NULL, " ");
                        if (anothertoken != NULL) {
                            insertL(&options, anothertoken);
                        }
                    } else {
                        file = anothertoken;
                    }
                }
            }

            if (file == NULL) {
                printf("tokens missing \n");
            } else if (countL(options) % 2 != 0) {
                printf("options should be even \n");
            } else {
                cfs_create(options, file);
            }

            cleanupL(options);
        } else if (strncmp(buffer, "close", 4) == 0) {
            cfs_close();
        } else if (strncmp(buffer, "exit", 4) == 0) {
            cfs_close();
            break;
        } else {
            printf("unknown first token \n");
        }
    }
}