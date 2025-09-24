#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "byos.h"

int interp(const struct cmd *c) {
    if (c == NULL) {return 1;}
    
    int return_status = 0;
    int outfd = -1;

    // dealing with redirection file
    if (c->redir_stdout != NULL) {
        int fd = open(c->redir_stdout, O_CREAT | O_TRUNC | O_WRONLY, 0666);
        if (fd == -1) {return 1;}
        
        outfd = dup(STDOUT_FILENO);
        if (outfd == -1) {close(fd); return 1;}
        
        if (dup2(fd, STDOUT_FILENO) == -1) {close(fd); close(outfd); return 1;}
        close(fd);
    }

    switch(c->type) {
    
        case ECHO:
            write(STDOUT_FILENO, c->data.echo.arg, strlen(c->data.echo.arg));
            break;
        
        case FORX:
            pid_t pid = fork();
            if (pid == 0) {
                // child process
                if (execvp(c->data.forx.pathname, c->data.forx.argv) == -1) {
                    perror("exec failed\n"); 
                    exit(127);
                }
            }
            else {
                // parent process
                int status;
                if (wait(&status) == -1) {
                    perror("wait failed\n");
                    return_status = 1;
                }
                else {
                    if (WIFEXITED(status)) {return_status = WEXITSTATUS(status);}
                    else if (WIFSIGNALED(status)) {return_status = 128 + WTERMSIG(status);}
                    else {return_status = 1;}
                }
            }
            break;
        
        case LIST:
            int n = c->data.list.n;
            struct cmd *cmds = c->data.list.cmds;

            for (int i = 0; i < n; i++) {return_status = interp(cmds + i);}
            break;
    }
    
    if (outfd != -1) {
        if (dup2(outfd, STDOUT_FILENO) != STDOUT_FILENO) {return_status = 1;}
        close(outfd);
    }

    return return_status;
}



