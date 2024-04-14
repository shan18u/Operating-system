/**
* Description: This module performs matrix multiplication using parallel processes, creating child processes for each row of the matrix and utilizing inter-process communication through pipes.
* It also handles input matrices from files and dynamically allocates memory for the results.
* Last modified date: 12/05/2023
* Creation date: 12/05/2023
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_ROWS 8
#define MAX_COLUMNS 8
#define FILENAME_SIZE 15

int pipes[MAX_COLUMNS][2];
int realStdout;
clock_t startClock, endClock, inputStart, inputEnd;
double cpuTimeUsed, inputTime;

int calculateMultiplication(char *inputMatrix, char **matrixList, const int matrixCount);
int acceptMatrix(const int numMatrices);
void releaseMemory(char **dynamicMatrix, const int items);

int main(int argc, char *argv[]) {
    startClock = clock();
    if (argc < 3) {
        fprintf(stderr, "You must pass in at least 2 matrices as input.\n");
        return 1;
    }

    realStdout = dup(STDOUT_FILENO);

    int numMatrices = argc - 2;
    // Create pipes for IPC.
    for (int i = 0; i < numMatrices; ++i) {
        if (pipe(pipes[i]) == -1) {
            fprintf(stderr, "Pipe creation failed.\n");
            return 1;
        }
    }

    char **matrixList = (char **)malloc(sizeof(char *) * numMatrices);
    if (matrixList == NULL) {
        fprintf(stderr, "Memory allocation failed for matrixList.\n");
        return 1;
    }

    // Store the names into matrixList.
    for (int i = 0; i < numMatrices; ++i) {
        matrixList[i] = strdup(argv[i + 2]);
    }

    if (calculateMultiplication(argv[1], matrixList, numMatrices) == 1) {
        fprintf(stderr, "Multiplication calculation failed. Refer to prior messages for cause.\n");
        releaseMemory(matrixList, numMatrices);
        return 1;
    }

    releaseMemory(matrixList, numMatrices);

    endClock = clock();
    cpuTimeUsed = ((double)(endClock - startClock)) / CLOCKS_PER_SEC;
    cpuTimeUsed -= inputTime;

    printf("\nRuntime: %f secs\n", cpuTimeUsed);

    return 0;
}

// Calculate matrix multiplication with the input in inputMatrix
int calculateMultiplication(char *inputMatrix, char **matrixList, const int matrixCount) {
    char outFile[FILENAME_SIZE];
    char errFile[FILENAME_SIZE];
    int outFD = -1;
    int errFD = -1;

    for (int i = 0; i < matrixCount; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            fprintf(stderr, "fork() failed.\n");
            return 1;
        } else if (pid == 0) {
            // Close all unnecessary read and write ends of the pipe.
            for (int j = 0; j < MAX_COLUMNS; ++j) {
                if (j != i) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }
            }

            pid_t childPID = getpid();

            snprintf(outFile, FILENAME_SIZE, "%d.out", childPID);
            snprintf(errFile, FILENAME_SIZE, "%d.err", childPID);

            // Create the actual files on disk, open in write only and append mode
            outFD = open(outFile, O_WRONLY | O_CREAT | O_APPEND | O_DSYNC, 0644);
            errFD = open(errFile, O_WRONLY | O_CREAT | O_APPEND | O_DSYNC, 0644);

            // Redirect stderr and stdout to errFile and outFile
            if ((dup2(errFD, STDERR_FILENO) == -1) || (dup2(outFD, STDOUT_FILENO) == -1)) {
                fprintf(stderr, "Redirecting stderr and stdout failed.\n");
                return 1;
            }

            fprintf(stdout, "Starting command %d: child %d pid of parent %d\n", i + 1, childPID, getppid());
            fflush(stdout);

            // Redirect stdin to the read end of the pipe.
            if (dup2(pipes[i][0], STDIN_FILENO) == -1) {
                fprintf(stderr, "Redirecting stdin and stdout to pipe read and write ends failed.\n");
                return 1;
            }

            char realSOUT[12];
            snprintf(realSOUT, sizeof(realSOUT), "%d", realStdout);

            char *args[] = {"matrixmult_parallel", inputMatrix, matrixList[i], realSOUT, NULL};
            if (execv("./matrixmult_parallel", args) == -1) {
                // Error handling.
                fprintf(stderr, "execv() failed. Command tried to execute: %s %s %s %s\n", "./matrixmult_parallel", args[1], args[2], args[3]);
                close(outFD);
                close(errFD);
                close(pipes[i][0]);
                close(pipes[i][1]);
                return 1;
            }
        }
    }

    if (dup2(realStdout, STDOUT_FILENO) == -1) {
        fprintf(stderr, "Redirecting stdout to terminal failed.\n");
        return 1;
    }

    if (acceptMatrix(matrixCount) == 1) {
        fprintf(stdout, "Sending matrices from stdin failed. Refer to prior messages for cause.\n");
        return 1;
    }

    // Parent process
    for (int i = 0; i < matrixCount; ++i) {
        int wstatus;
        int childPID = wait(&wstatus); // Wait for each child process to end.

        // Create a string that results in PID.out and PID.err
        snprintf(outFile, FILENAME_SIZE, "%d.out", childPID);
        snprintf(errFile, FILENAME_SIZE, "%d.err", childPID);

        outFD = open(outFile, O_WRONLY | O_APPEND | O_DSYNC, 0644);
        errFD = open(errFile, O_WRONLY | O_APPEND | O_DSYNC, 0644);

        // Redirect stdout and stderr to out_file and err_file.
        dup2(outFD, STDOUT_FILENO);
        dup2(errFD, STDERR_FILENO);

        fprintf(stdout, "Finished child %d pid of parent %d\n", childPID, getpid());
        fflush(stdout);

        if (WIFEXITED(wstatus)) {
            int exitStatus = WEXITSTATUS(wstatus); // Store exit code of child process.

            if (exitStatus == 0) {
                fprintf(stdout, "Exited with exitcode = %d\n", exitStatus);
                fflush(stdout);
            } else {
                fprintf(stderr, "Exited with exitcode = %d\n", exitStatus);
            }
        } else if (WIFSIGNALED(wstatus)) {
            fprintf(stderr, "Killed with signal %d\n", WTERMSIG(wstatus));
        }

        fflush(stdout);
        fflush(stderr);

        // Close read end of pipe.
        close(pipes[i][0]);
    }

    if (dup2(realStdout, STDOUT_FILENO) == -1) {
        fprintf(stderr, "Redirecting stdout to terminal failed.\n");
        return 1;
    }

    // Close all file descriptors
    close(outFD);
    close(errFD);

    return 0;
}

// accept matrices from stdin and then passes them off to the child processes.
int acceptMatrix(const int matrixCount) {
    char *aMatrix = NULL;
    size_t bufferLen = 0;

    fprintf(stdout, "Enter file path of a matrix: \n");
    fflush(stdout);

    inputStart = clock();

    while (getline(&aMatrix, &bufferLen, stdin) != -1) {
        inputEnd = clock();
        inputTime += ((double)(inputEnd - inputStart)) / CLOCKS_PER_SEC;
        size_t len = strlen(aMatrix);

        // Remove the trailing newline character.
        if (len > 0 && aMatrix[len - 1] == '\n') {
            aMatrix[len - 1] = '\0';
            len--;
        }

        if (len > 0) {
            for (int i = 0; i < matrixCount; ++i) {
                // Pass the length of the file passed to each child process.
                if (write(pipes[i][1], &len, sizeof(size_t)) == -1) {
                    fprintf(stderr, "Sending length of matrix %s text to child %d failed.\n", aMatrix, i);
                    return 1;
                }

                // Pass the filename to each child process.
                if (write(pipes[i][1], aMatrix, len) == -1) {
                    fprintf(stderr, "Sending matrix %s to child %d failed.\n", aMatrix, i);
                    return 1;
                }
            }
        }

        // Free the space allocated for the filename and reset.
        free(aMatrix);
        aMatrix = NULL;
        bufferLen = 0;
        fprintf(stdout, "Enter file path of a matrix (Ctrl+D to exit): \n");
        inputStart = clock();
    }

    inputEnd = clock();
    inputTime += ((double)(inputEnd - inputStart)) / CLOCKS_PER_SEC;

    for (int i = 0; i < matrixCount; ++i) {
        bufferLen = 0;
        if (write(pipes[i][1], &bufferLen, sizeof(size_t)) == -1) {
            fprintf(stderr, "Sending EOF to child %d failed.\n", i);
            return 1;
        }

        close(pipes[i][1]);
    }

    if (aMatrix != NULL) {
        free(aMatrix);
        aMatrix = NULL;
    }

    return 0;
}

// Frees the memory allocated for the given matrix.
void releaseMemory(char **dynamicMatrix, const int items) {
    for (int i = 0; i < items; ++i) {
        free(dynamicMatrix[i]);
    }
    free(dynamicMatrix);
    dynamicMatrix = NULL;
}
