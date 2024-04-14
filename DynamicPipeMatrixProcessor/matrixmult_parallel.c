/**
* Description: This module performs performs parallel matrix multiplication by forking child processes.
* Redirects its standard output to a pipe, the child writes data to the pipe for the parent to process
* Last modified date: 10/25/2023
* Creation date: 10/20/2023
**/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

#define MAX_ROWS 8
#define MAX_COLS 8

/** This function reads a matrix, fills in the values from the file.
 * Input parameters: file - input, matrix, rows - the number of rows in the matrix,
 * cols - the number of columns in the matrix.
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

/**
 * This function performs the multiplication of a row of matrix A with matrix W
 * and stores the result in matrix R.
 * Input parameters: A, W, R, rowA, colsA, colsW.
 **/

void multiplyRow(int *A, int *W, int *R, int rowA, int colsA, int colsW) {
    for (int j = 0; j < colsW; j++) {
        R[rowA * colsW + j] = 0;
        for (int k = 0; k < colsA; k++) {
            R[rowA * colsW + j] += A[rowA * colsA + k] * W[k * colsW + j];
        }
    }
}

/**
 * The main function of the program reads two matrices from input files, creates child processes
 * and prints the resulting matrix and execution time.
 * Input parameters: argc, argv.
 **/

int main(int argc, char *argv[]) {
    clock_t start_time, end_time;
    double execution_time;

    // Check if the correct number of command-line arguments is provided
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <A_matrix_file> <W_matrix_file>\n", argv[0]);
        exit(1);
    }

    start_time = clock();

    // Creating three matrices objects A,W,R
    int A[MAX_ROWS * MAX_COLS];
    int W[MAX_COLS * MAX_COLS];
    int R[MAX_ROWS * MAX_COLS];

    FILE *fileA = fopen(argv[1], "r");
    FILE *fileW = fopen(argv[2], "r");

    // Check if the files were opened successfully
    if (!fileA || !fileW) {
        fprintf(stderr, "Error opening input file(s).\n");
        exit(1);
    }

    // Read matrices from files
    readMatrixFromFile(fileA, A, MAX_ROWS, MAX_COLS);
    readMatrixFromFile(fileW, W, MAX_COLS, MAX_COLS);

    fclose(fileA);
    fclose(fileW);

    // Create processes (one for each row of A)
    for (int i = 0; i < MAX_ROWS; i++) {
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

            multiplyRow(A, W, R, i, MAX_COLS, MAX_COLS);

            // Send the result row through the pipe to the parent
            write(pipefd[1], R + i * MAX_COLS, MAX_COLS * sizeof(int));

            close(pipefd[1]);
            exit(0);
        } else if (pid < 0) {
            fprintf(stderr, "Fork error.\n");
            exit(1);
        }
        // Parent process close write end of the pipe
        close(pipefd[1]);

        // Read the result row from the child through the pipe
        ssize_t bytes_read = read(pipefd[0], R + i * MAX_COLS, MAX_COLS * sizeof(int));
        if (bytes_read == -1) {
            perror("Read error");
            exit(1);
        }
        close(pipefd[0]);
    }


    // Stop measuring execution time
    end_time = clock();
    execution_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    // Print out the matrix
    for (int i = 0; i < MAX_ROWS; i++) {
        if (R[i * MAX_COLS] != 0)
        {
            for (int j = 0; j < MAX_COLS; j++) {
                printf("%d ", R[i * MAX_COLS + j]);
            }
        }
    }
    printf ("\n");

    fflush(stdout);

    // Print runtime in seconds
    printf("Runtime %.4f seconds\n", execution_time);
    return 0;
}

