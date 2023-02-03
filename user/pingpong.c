#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
panic(char *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}

int
main(void)
{   
    // parent   [1]---pipe1-->[0]   child
    //          [0]<--pipe2---[1]  

    int pipe1[2];
    int pipe2[2];
    char buff[1];
    if (pipe(pipe1) < 0 || pipe(pipe2) < 0) {
        panic("pipe");
        exit(1);
    }
    int rc = fork();
    if (rc == -1) {
        panic("fork");
        exit(1);
    }
    if (rc == 0) {
        // child process
        close(pipe1[1]);
        close(pipe2[0]);
        if (read(pipe1[0], buff, 1) < 0) {
            panic("read");
        } else {
            fprintf(1, "%d: received ping\n", getpid());
        }
        if (write(pipe2[1], "a", 1) < 0) {
            panic("write");
        }
        exit(0);
    } 
    // parent process
    close(pipe1[0]);
    close(pipe2[1]);
    if (write(pipe1[1], "a", 1) < 0) {
        panic("write");
    }
    if (read(pipe2[0], buff, 1) < 0) {
        panic("read");
    } else {
        fprintf(1, "%d: received pong\n", getpid());
    }
    wait(0);
    exit(0);
}
