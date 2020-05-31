#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

int child_pid = 0;

void timer_handler(int pid){
    kill(child_pid, SIGKILL);
}

int fail_handler(char *msg){
    perror(msg);
    return -1; // to catch this as python execption
}

void exec_testcase(char **argv, int *ret){
    int stdout_pipe[2] = {-1, -1};
    int stderr_pipe[2] = {-1, -1};

    if (pipe(stdout_pipe) < 0 || pipe(stderr_pipe) < 0){
        fail_handler("[!] Error: pipe fail");
    }

    extern char **environ;
    pid_t pid = vfork();
    while (pid < 0) {
        sleep(1);
        pid = vfork();
    }
    child_pid = pid;

    if(pid < 0){
        fail_handler("[!] Error: vfork fail");
    
    }else if(pid == 0){
        signal(SIGALRM, timer_handler);
        close(stdout_pipe[0]);
        close(stderr_pipe[0]);

        dup2(stdout_pipe[1], 1);
        dup2(stderr_pipe[1], 2);
        close(stdout_pipe[1]);
        close(stderr_pipe[1]);

        alarm(3); // timeout with 3 sec

        int code = execve(argv[0], argv, environ);

        if(code == -1){
            fail_handler("[!] Error: executetion fail");
        }
    }

    close(stdout_pipe[1]);
    close(stderr_pipe[1]);

    int status;
    if(waitpid(pid, &status, 0) < 0){
        fail_handler("[!] wait failed");
    }

    ret[0] = pid;
    ret[1] = stdout_pipe[0];
    ret[2] = stderr_pipe[0];
    ret[3] = (WIFEXITED(status)) ? WEXITSTATUS(status) : WTERMSIG(status); // program exit code
}