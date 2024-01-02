/**
* Description: This module performs parallel matrix multiplication of two input matrices (A and W)
* and prints the result matrix along with the runtime in seconds
* Author names: Shivansh Chhabra, Kean lee
* Last modified date: 09/24/2023
* Creation date: 09/19/ 2023
**/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

/**
 * Reads a matrix from a file and stores it in an array.
 *
 * Assumptions:
 * - The file format contains space-separated values.
 * - Each row of the matrix is on a separate line.
 * - The matrix dimensions are defined by rows and cols.
 * - If a row has fewer values than cols, missing values are treated as 0.
 *
 * file: The file pointer for reading.
 * matrix: The target array for storing the matrix.
 * rows: The number of rows in the matrix.
 * cols: The number of columns in the matrix.
 */


// Function to read a matrix from a file into a 1D array
void readMatrixFromFile(FILE *file, int *matrix, int rows, int cols) {

    // Assuming a line will have less than 1024 characters
    char buffer[1024];

    for (int i = 0; i < rows; i++) {
        if (fgets(buffer, sizeof(buffer), file) != NULL) {
            char *token = strtok(buffer, " ");
            for (int j = 0; j < cols; j++) {
                if (token != NULL) {
                    // Convert token to integer and store in matrix
                    // If no more tokens, set matrix element to 0
                    matrix[i * cols + j] = atoi(token);
                    token = strtok(NULL, " ");
                } else {
                    matrix[i * cols + j] = 0;
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
    int rowsA = 8;
    int colsA = 8;
    int colsW = 8;
    int A[rowsA * colsA];
    int W[colsA * colsW];

    readMatrixFromFile(fileA, A, rowsA, colsA);
    readMatrixFromFile(fileW, W, colsA, colsW);

    fclose(fileA);
    fclose(fileW);

    // Result matrix
    int R[rowsA * colsW];

    // Create processes (one for each row of A)
    for (int i = 0; i < rowsA; i++) {
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

            multiplyRow(A, W, R, i, colsA, colsW);

            // Send the result row through the pipe to the parent
            write(pipefd[1], R + i * colsW, colsW * sizeof(int));

            close(pipefd[1]);
            exit(0);
        } else if (pid < 0) {
            fprintf(stderr, "Fork error.\n");
            exit(1);
        }

        // Parent process
        // Close write end of the pipe
        close(pipefd[1]);

        // Read the result row from the child through the pipe
        read(pipefd[0], R + i * colsW, colsW * sizeof(int));

        close(pipefd[0]);
    }

    // Wait for all child processes to finish and report their status
    while (wait(NULL) > -1) {
        int status;

        if (WIFEXITED(status)) {
            printf("Child terminated normally with exit code: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Child terminated abnormally with signal number: %d\n", WTERMSIG(status));
        }
    }

    // Stop measuring execution time
    end_time = clock();
    execution_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    // Print the result matrix
    printf("Result of A*W = [\n");
    for (int i = 0; i < rowsA; i++) {
        for (int j = 0; j < colsW; j++) {
            printf("%d ", R[i * colsW + j]);
        }
        printf("\n");
    }
    printf("]");
    printf("\n");

    // Print runtime in seconds
    printf("Runtime %.4f seconds\n", execution_time);
    return 0;
}
