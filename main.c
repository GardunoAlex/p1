#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "./include/table.h"

/*

TASKS TO DO IN MAIN

(1) 
Accept input arguments for: <log directory> <number of mappers> <number of reducers>
    * If the user passes a number of arguments less than necessary, print the following error and return from the function:

Usage: mapreduce <directory> <n mappers> <n reducers>
    * If the number of mappers or reducers is less than one, print the following error and return from the function:

mapreduce: cannot have less than one mapper or reducer
    * If the directory is invalid, use the perror function to print "opendir", and perror will automatically add the reason for the failed syscall.

    * These error messages above are necessary to pass the testcases

(2)
Use the opendir, readdir functions to read the files in the requested directory, and keep track of the filenames to distribute evenly among child processes
    * If you plan to iterate through the directory twice, use the rewinddir function before your second loop. 
    Otherwise, your directory stream pointer will be at the end, resulting in no files read on the second iteration

    * Reading directories also includes the . and .. files. Make sure you skip over these, so you don't assign processes to read them

    * View documentation of struct dirent for how to get the name of a file from a directory entry

(3)
Dispatch the requested number of child processes to run the ./map executable, and wait for them to complete only after they have all been forked.
    * The files that these map processes write to must be named: 0.tbl, 1.tbl, ... n-1.tbl for n mappers. 
    They must be written to the ./intermediate directory. Do not worry about creating the directory automatically, just make sure that you have an ./intermediate directory present when running mapreduce. 
    See the file structure below for an example with n mappers

pa-1/
├── test_cases/
├── intermediate/
│ ├── 0.tbl
│ ├── 1.tbl
│ ...
│ └── n-1.tbl
└── ...
    * If any child exits with a nonzero status, print an error message and return from the main, you can use the WEXITSTATUS macro to extract this information. 
    Make sure you wait on all map children before spawning reduce children

    * Remember that when passing arguments into exec, you should include NULL as the last argument

(4)
Dispatch the requested number of reducer processes to run the ./reduce executable, and wait for them to complete after forking them all.
    * Each child should be given an even portion of the key space to examine, unless it is not evenly divisible, in which case one process will need to take the remainder

    * They should all be instructed to read from the ./intermediate directory

    * The files should be written in the same style as above, but to the ./out directory

    * The same rule with the child exit status applies as well

(5)
Read all of the files in the ./out directory and print the resulting tables. 
You should not need to combine them, since they each examine a unique subset of the key space.
*/

int main(int argc, char *argv[]) {
    return 0;
}
