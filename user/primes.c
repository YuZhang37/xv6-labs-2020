#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

const int PIPE_INPUT = 1;
const int PIPE_OUTPUT = 0;

void
panic(char *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}

int
fork1(void)
{
  int pid;

  pid = fork();
  if(pid == -1)
    panic("fork");
  return pid;
}

void
pipe1(int* pipe_fd) {
    if (pipe(pipe_fd) < 0) {
        panic("pipe");
    }
}


void
send_to_pipe(int* pipe_fd, int value) {
    if (write(pipe_fd[PIPE_INPUT], &value, 4) < 0) {
        panic("write to pipe");
    }
}

int
read_from_pipe(int* pipe_fd) {
    char buff[4] = {0};
    int rc = read(pipe_fd[PIPE_OUTPUT], buff, 4);
    if (rc < 0) {
        panic("read from pipe");
    }
    return rc == 0 ? 0 : *((int*) buff);
}

int
setup_pipeline_node(int* pipe_fd) {
    close(pipe_fd[PIPE_INPUT]);
    int pipe2_fd[2];
    pipe1(pipe2_fd);
    int init_value = read_from_pipe(pipe_fd);
    
    fprintf(1, "prime %d\n", init_value);
    int value = read_from_pipe(pipe_fd);
    if (value == 0) {
        exit(0);
    } 
    int rc = fork1();
    if (rc == 0) {
        //child process
        close(pipe_fd[PIPE_OUTPUT]);
        setup_pipeline_node(pipe2_fd);
    } else {
        //parent process
        while (value != 0) {
            if (value % init_value != 0) {
                send_to_pipe(pipe2_fd, value);
            }
            value = read_from_pipe(pipe_fd);

        }
        close(pipe2_fd[PIPE_INPUT]);
        wait(0);
    }
    exit(0);

}

int
main(int argc, char** argv)
{   
    int max_limit = 35;
    if (argc == 2) {
        max_limit = atoi(argv[1]);
    }
    int i = 2;
    int pipe_fd[2];
    pipe1(pipe_fd);

    int rc = fork1();
    if (rc == 0) {
        // child process
        if (setup_pipeline_node(pipe_fd) < 0) {
            panic("setup_pipeline_node");
        }
    } else {
        // parent process
        for (i = 2; i <= max_limit; i++){
            send_to_pipe(pipe_fd, i);
        }
        close(pipe_fd[PIPE_INPUT]);
        wait(0);

    }

    exit(0);
}