/**
* Description: This module performs performs parallel matrix multiplication by forking child processes.

* Last modified date: 10/17/2023
* Creation date: 10/02/2023
**/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

// Define ROWS and COLS here
#define ROWS 8
#define COLS 8


/**
 * This function reads a matrix from a file into a D array.
 * Assumption: The matrix in the file is represented row-wise.
 * Input parameters: file, matrix, rows, cols
 **/
void readMatrixFromFile(FILE *file, int *matrix, int rows, int cols) {
    // Initialize the matrix with zeros
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i * cols + j] = 0;
        }
    }

    // Read the values from the file and fill in the matrix
    char buffer[1024];

    for (int i = 0; i < rows; i++) {
        if (fgets(buffer, sizeof(buffer), file) != NULL) {
            char *token = strtok(buffer, " ");
            for (int j = 0; j < cols; j++) {
                if (token != NULL) {
                    
                    // Convert token to integer and store in matrix
                    matrix[i * cols + j] = atoi(token);
                    token = strtok(NULL, " ");
                } else {
                    break;
                }
            }
        }
    }
}


// Multiplies a single row of matrix A by matrix W and stores the result in matrix R.
void multiplyRow(int *A, int *W, int *R, int rowA, int colsA, int colsW) {
    for (int j = 0; j < colsW; j++) {
        R[rowA * colsW + j] = 0;
        for (int k = 0; k < colsA; k++) {
            R[rowA * colsW + j] += A[rowA * colsA + k] * W[k * colsW + j];
        }
    }
}

int main(int argc, char *argv[]) {
    clock_t start_time, end_time;
    double execution_time;

    if (argc != 3) {
        fprintf(stderr, "error: expecting exactly 2 files as input\nTerminating, exit code 1.\n");
        exit(1);
    }

    // Start measuring execution time
    start_time = clock();

    // Open the input files and check if the files could be opened or not
    FILE *fileA = fopen(argv[1], "r");
    FILE *fileW = fopen(argv[2], "r");

    bool error = false;

    if (!fileA) {
        fprintf(stderr, "Error opening file %s.\n", argv[1]);
        error = true;
    }

    if (!fileW) {
        fprintf(stderr, "Error opening file %s.\n", argv[2]);
        error = true;
    }

    // If any error occurred, terminate with exit code
    if (error) {
        printf("Terminating, exit code 1.\n");
        exit(1);
    }

    // Read matrices from files using readMatrixFromFile
    int A[ROWS * COLS];
    int W[COLS * COLS];

    readMatrixFromFile(fileA, A, ROWS, COLS);
    readMatrixFromFile(fileW, W, COLS, COLS);

    fclose(fileA);
    fclose(fileW);

    // Result matrix
    int R[ROWS * COLS];

    // Create processes (one for each row of A)
    for (int i = 0; i < ROWS; i++) {
        pid_t pid;
        int pipefd[2];

        if (pipe(pipefd) == -1) {
            perror("Pipe creation failed");
            exit(1);
        }

        pid = fork();
        // Child process
        if (pid == 0) {
            close(pipefd[0]);

            multiplyRow(A, W, R, i, COLS, COLS);

            // Send the result row through the pipe to the parent
            write(pipefd[1], R + i * COLS, COLS * sizeof(int));

            close(pipefd[1]);
            exit(0);
        } else if (pid < 0) {
            fprintf(stderr, "Fork error.\n");
            exit(1);
        }
        // Parent process close write end of the pipe
        close(pipefd[1]);

        // Read the result row from the child through the pipe
        ssize_t bytes_read = read(pipefd[0], R + i * COLS, COLS * sizeof(int));
        if (bytes_read == -1) {
            perror("Read error");
            exit(1);
        }
        close(pipefd[0]);
    }

    // Wait for all child processes to finish and report their status
    int status;
    while (waitpid(-1, &status, 0) > 0) {
        if (WIFEXITED(status)) {
            printf("Child terminated normally with exit code: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Child terminated abnormally with signal number: %d\n", WTERMSIG(status));
        }
    }


    // Stop measuring execution time
    end_time = clock();
    execution_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    // Print the result matrix and include input file names
    printf("Result of %s * %s = [\n", argv[1], argv[2]);
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%d ", R[i * COLS + j]);
        }
        printf("\n");
    }
    printf("]");
    printf("\n");

    // Print runtime in seconds
    printf("Runtime %.4f seconds\n", execution_time);
    return 0;
}
