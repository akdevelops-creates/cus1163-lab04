#include "executor.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/**
 * Hey Professor Sigera! I noticed a lot of similarities between how processses communicate and how they execute, 
 * so I'm expanding on my film production set/director-production assistant analogy 
 * from lab 3 into lab 4.
 *
 * My Film Production Set aka THE FORK-EXEC-WAIT PATTERN
 * #1. **fork** aka THE CASTING CALL: Like in Lab 3, the Director (Parent process) clones 
 * themselves to create a Production Assistant (Child). They both start with the same script.
 *
 * #2. **execvp** aka THE SCRIPT SWAP: This part distinguishes lab 4 from lab 3. 
 * The PA (child) throws away the Director's (parent's) script and loads a completely new one (the command like ls). 
 * The process's memory is erased and replaced by the new program.
 * 
 * #3. **waitpid** aka THE SCENE WRAP-UP: The Director (parent) stays on set (in memory) to collect the 
 * PA's final exit status, preventing zombie processes.
 *
 * @param command The command to execute (e.g., "ls", "pwd", "echo")
 * @param args Array of arguments: [command, arg1, arg2, ..., NULL]
 *             Example: {"ls", "-l", NULL} or {"echo", "Hello", NULL}
 * @return Exit status of the command (0 = success, non-zero=failure, -1 = error)
 */
 
 int execute_command(char *command, char **args) {
    pid_t pid;
    int status;

    // #1: THE CASTING CALL (fork)
    // here, I create the child process that will execute the new program.
    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return -1;
    } 

    // #2: THE SCRIPT SWAP (execvp)
    if (pid == 0) {
        /* here, I am the child process (production Assistant). I call execvp() to overwrite my memory 
         * with the new command. If this succeeds, I’ll stop being the 
         * “PA” and become the new program (e.g., 'ls'). */
        if (execvp(command, args) == -1) {
            /* If execvp returns, the command wasn’t found. 
             * I need to exit(1) so I don't continue running the parent's script. */
            perror("execvp failed");
            exit(1); 
        }
    } 

    // #3: THE WRAP-UP (waitpid)
    /* Here, I’m the “Director” Parent process. 

     * I wait until the "PA" child process (pid)
     * finishes executing, then I collect its final reportc(exit status), checking it off my clipboard. */
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid failed");
        return -1;
    }

    // Returns the exit status if the child ended normally.
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }

    return -1;
}
