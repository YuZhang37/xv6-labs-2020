#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int size = 32;

int 
get_line_from_input(char* buff, int size) {
    char* p = buff;
    *p = 0;
    char ch = 'a';
    if (read(0, &ch, 1) <= 0) {
        // fprintf(2, "Error in get_line_from_input: read init\n");
        return -1;
    }
    if (ch == 0) {
        return 0;
    }
    while (ch != '\n') {
        *p++ = ch;
        *p = 0;
        if (p - buff >= size) {
            // fprintf(2, "Error in get_line_from_input: buff overflow\n");
            return -1;
        }
        if (read(0, &ch, 1) <= 0) {
            // fprintf(2, "Error in get_line_from_input: read\n");
            return -1;
        }
    }
    return strlen(buff);
}


int
parse_line(char* buff, char argvs[][size], int start_index, int max_args, int max_size) {
    int index_row = start_index, index_col = 0;
    int reading = 0;
    int i;
    for (i = 0; i < strlen(buff); i++) {
        if (reading == 0) {
            if (buff[i] == ' ') {
                continue;
            } else {
                reading = 1;
            }
        }
        if (reading == 1 && buff[i] == ' ') {
            reading = 0;
            argvs[index_row][index_col] = 0;
            index_col = 0;
            index_row++;
            argvs[index_row][index_col] = 0;
            if (index_row >= max_args) {
                fprintf(2, "Error in parse line: argvs rows overflow\n");
                return -1;
            }
            continue;
        }
        argvs[index_row][index_col++] = buff[i];
        argvs[index_row][index_col] = 0;
        if (index_col >= max_size - 1) {
            fprintf(2, "Error in parse line: argvs columns overflow\n");
            return -1;
        }
    }
    return 0;
}


int
set_mem(char argvs[][size], char** argv, int argc, int max_size) {
    int i, len;
    for (i = 0; i < argc; i++) {
        len = strlen(argv[i]);
        if (len >= max_size) {
            return -1;
        }
        memcpy(argvs[i], argv[i], strlen(argv[i]));
    }
    return 0;
}


int
main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(2, "Usage: xargs <command> <args>\n");
        exit(1);
    }

    char buff[1024];
    char argvs[MAXARG + 1][size];
    memset(argvs, 0, sizeof argvs);
    if (set_mem(argvs, argv, argc, 32) < 0) {
        fprintf(2, "Error in xargs: set_mem\n");
        exit(1);
    }
    while(get_line_from_input(buff, 1024) > 0) {
        // fprintf(1, "line: %s\n", buff);
        parse_line(buff, argvs, argc, MAXARG, size);
        char* exec_args[MAXARG + 1];
        memset(exec_args, 0, sizeof exec_args);
        for (int i = 0; i < MAXARG; i++) {
            if (strlen(argvs[i]) == 0) {
                break;
            }
            // fprintf(1, "argvs: %s, size: %d\n", argvs[i], strlen(argvs[i]));
            exec_args[i] = argvs[i];
        }
        int rc = fork();
        if (rc < 0) {
            fprintf(2, "Error in xargs: fork\n");
            exit(1);
        }
        if (rc == 0) {
            //child process
            if (exec(argv[1], exec_args + 1) < 0) {
                fprintf(2, "Error in xargs: exec\n");
                exit(1);
            }
        } else {
            //parent process
            wait(0);
        }
    }
    exit(0);
}