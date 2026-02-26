#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>``
#include <unistd.h>

#include "./include/table.h"

/*

TASKS TO DO IN MAIN

(1)
Accept input arguments for: <log directory> <number of mappers> <number of reducers>
    * If the user passes a number of arguments less than necessary, print the following error and
return from the function:

Usage: mapreduce <directory> <n mappers> <n reducers>
    * If the number of mappers or reducers is less than one, print the following error and return
from the function:

mapreduce: cannot have less than one mapper or reducer
    * If the directory is invalid, use the perror function to print "opendir", and perror will
automatically add the reason for the failed syscall.

    * These error messages above are necessary to pass the testcases

(2)
Use the opendir, readdir functions to read the files in the requested directory, and keep track of
the filenames to distribute evenly among child processes
    * If you plan to iterate through the directory twice, use the rewinddir function before your
second loop. Otherwise, your directory stream pointer will be at the end, resulting in no files read
on the second iteration

    * Reading directories also includes the . and .. files. Make sure you skip over these, so you
don't assign processes to read them

    * View documentation of struct dirent for how to get the name of a file from a directory entry

(3)
Dispatch the requested number of child processes to run the ./map executable, and wait for them to
complete only after they have all been forked.
    * The files that these map processes write to must be named: 0.tbl, 1.tbl, ... n-1.tbl for n
mappers. They must be written to the ./intermediate directory. Do not worry about creating the
directory automatically, just make sure that you have an ./intermediate directory present when
running mapreduce. See the file structure below for an example with n mappers

pa-1/
├── test_cases/
├── intermediate/
│ ├── 0.tbl
│ ├── 1.tbl
│ ...
│ └── n-1.tbl
└── ...
    * If any child exits with a nonzero status, print an error message and return from the main, you
can use the WEXITSTATUS macro to extract this information. Make sure you wait on all map children
before spawning reduce children

    * Remember that when passing arguments into exec, you should include NULL as the last argument

(4)
Dispatch the requested number of reducer processes to run the ./reduce executable, and wait for them
to complete after forking them all.
    * Each child should be given an even portion of the key space to examine, unless it is not
evenly divisible, in which case one process will need to take the remainder

    * They should all be instructed to read from the ./intermediate directory

    * The files should be written in the same style as above, but to the ./out directory

    * The same rule with the child exit status applies as well

(5)
Read all of the files in the ./out directory and print the resulting tables.
You should not need to combine them, since they each examine a unique subset of the key space.
*/

int main(int argc, char *argv[]) {
    /*
        STEP 1: Reading the file and setting up errors.
    */

    // does the argument list need to have 4 items?
    if (argc != 4) {
        printf("Usage: mapreduce <directory> <n mappers> <n reducers> \n");
        return 1;
    }

    DIR *log_dir;
    char *num_mappers = argv[3];
    char *num_reducers = argv[2];
    log_dir = opendir(argv[1]);
    // log directory stream to check for all for all of the entries
    struct dirent *log_dir_stream;

    // check for number of mappers/reducers; if function doesn't return, args save to use in other
    // functions.
    if (atoi(num_mappers) < 1) {
        printf("mapredue: cannot have less than one mapper or reducer\n");
        return 1;
    }

    if (atoi(num_reducers) < 1) {
        printf("  mapreduce: cannot have less than one mapper or reducer\n");
        return 1;
    }

    if (log_dir == NULL) {
        perror("opendir");
        exit(1);
    } else {
        printf("File has successfully opened");
    }

    /*
        STEP 2
    */

    // log_dir should be open if the program has made it to this point.
    int count = 0;
    while ((log_dir_stream = readdir(log_dir))) {
        // have to look for files with . or ..
        char *file_name = log_dir_stream->d_name;

        if (strcmp(file_name, ".") != 0 || strcmp(file_name, "..") != 0) {
            printf("File: %s added", file_name);
        }
        count++;
    }

    rewinddir(log_dir);
    char *file_names[count];
    count = 0;

    while ((log_dir_stream = readdir(log_dir))) {
        char *file_name = "fileName";

        if (strcmp(file_name, ".") != 0 && strcmp(file_name, "..") != 0) {
            int size = strlen(argv[1]) + strlen(file_name) + 2;
            char *full_path = malloc(size);
            if (full_path == NULL) {
                perror("malloc");
                exit(1);
            }

            snprintf(full_path, size, "%s/%s", argv[1], file_name);

            file_names[count++] = full_path;
        }
    }

    // closed the initial log directory; I also think that the log_dir_stream closes with the
    // closedir(log_dir) function call.
    closedir(log_dir);

    // Step 3: assign mappers to a file

    pid_t pid;

    int n_map = atoi(num_mappers);
    int n_reduce = atoi(num_reducers);
    int file_name_index = 0;
    int file_names_rem = count % n_map;

    for (int i = 0; i < n_map; i++) {
        int num_files = (count / n_map) + (i < file_names_rem ? 1 : 0);

        pid = fork();

        if (pid < 0) {
            perror("Fork failed in Step 3");
            return 1;
        } else if (pid == 0) {
            char buffer[40];
            snprintf(buffer, sizeof(buffer), "./intermediate/%d.tbl", i);

            char **args = malloc((num_files + 3) * sizeof(char *));
            args[0] = "./map";
            args[1] = strdup(buffer);

            strcpy(args[1], buffer);

            for (int i = 0; i < num_files; i++) {
                args[i + 2] = strdup(file_names[file_name_index + j]);
            }

            args[num_files + 2] = NULL;
            j++;
            execv("./map", args);

            perror("execv failed");
            for (int i = 0; args[i] != NULL; i++) {
                free(args[i]);
            }
            free(args);
            exit(1);
        }
        file_name_index += num_files;
    }

    // wait for all children to finish
    for (int i = 0; i < n_map; i++) {
        int status;
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);

            if (exit_code != 0) {
                fprintf(stderr, "Error: Child failed with status %d\n", exit_code);
                exit(1);
            }
        } else {
            fprintf(stderr, "Error: Child  terminated abnormally\n", pid);
            exit(1);
        }
    }

    return 0;
}
