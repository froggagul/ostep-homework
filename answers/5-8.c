  
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

/*
fork, pipe: https://www.geeksforgeeks.org
int pipe(int fds[2]);

Parameters :
fd[0] will be the fd(file descriptor) for the 
read end of pipe.
fd[1] will be the fd for the write end of pipe.
Returns : 0 on Success.
-1 on error.
*/

int main(int argc, char *argv[])
{
    char buffer[64];
    int pipe_entry[2];    // used to store end of pipes
    // check pipe entries
    if (pipe(pipe_entry) < 0) {
        fprintf(stderr, "Pipe failed! try again..\n");
        exit(1);
    }
    // start fork
    int child1 = fork();
    if (child1 < 0) {
        fprintf(stderr, "fork to child1 failed! try again..\n");
        exit(1);
    } else if (child1 == 0) {
        printf("child 1 process is running..\n");
        close(pipe_entry[0]);   // close pipe_entry[0], which means child1 only writes into pipe
        dup2(pipe_entry[1], 1); // redirect stdout to pipe write
        printf("hello child2! I'm child1!\n-from child1\n");
    } else {
        // Parent process
        int child2 = fork();
        if (child2 < 0) {
            fprintf(stderr, "fork to child2 failed! try again..\n");
            exit(1);
        } else if (child2 == 0) {
            printf("child 2 process is running..\n");
            close(pipe_entry[1]);      // close pipe_entry[1], which means child2 only reads from pipe
            dup2(pipe_entry[0], 0);    // Redirect stdin to pipe read
            char buffer[512];   // Make a buffer
            read(STDIN_FILENO, buffer, 512); // Read in from stdin
            printf("chlid2 has received from child1:\n%s", buffer);     // Print out buffer
        } else {
            // wait until child2 ends
            int wc = waitpid(child2, NULL, 0);
            printf("ending parent process...\n");
        }
    }
    return 0;
}