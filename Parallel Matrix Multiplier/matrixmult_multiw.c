/**
* Description: This module performs performs parallel matrix multiplication by forking child processes to calculating the product of a matrix A with multiple matrices W using
* executable matrixmult_parallel file, and reporting results and execution times.

* Last modified date: 10/11/2023
* Creation date: 10/02/2023
**/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

/**
 * This function reads a matrix from a file into D array.
 * Assumption: The matrix in the file is represented row-wise.
 * Input parameters: file, matrix, rows, cols
 **/
void readMatrixFromFile(FILE *file, int *matrix, int rows, int cols) {

    // Assume the line will have less than 1024 characters
    char buffer[1024];

    for (int i = 0; i < rows; i++) {
        if (fgets(buffer, sizeof(buffer), file) != NULL) {
            char *token = strtok(buffer, " ");
            for (int j = 0; j < cols; j++) {
                if (token != NULL) {

                    // Convert token to an integer and store it in the matrix
                    matrix[i * cols + j] = atoi(token);
                    token = strtok(NULL, " ");
                } else {
                    matrix[i * cols + j] = 0;
                }
            }
        }
    }
}

/**
 * This function prints a matrix.
 * Assumption: The matrix is represented as a 1D array.
 * Input parameters: matrix, rows, cols
 * Output: None
 **/
void printMatrix(int *matrix, int rows, int cols) {
    printf("[\n");
    for (int i = 0; i < rows; i++) {
        printf(" ");
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i * cols + j]);
        }
        printf("\n");
    }
    printf("]\n");
}


/**
 * This function executes matrix multiplication with the specified input files in parallel.
 * Assumption: A_file is the input matrix file, and W_files contains paths to multiple W matrices.
 * Input parameters: A_file, W_files, numW
 **/
void executeMatrixMultMultiw(const char *A_file, const char *W_files[], int numW) {
    char *command = "./matrixmult_parallel"; // Adjust the path if needed

    // Create file names for output and error files
    char outFileName[256];
    char errFileName[256];

    for (int i = 0; i < numW; i++) {
        pid_t child_pid = fork();

        if (child_pid == -1) {
            perror("Fork error");
            exit(1);
        }

        if (child_pid == 0) {
            sprintf(outFileName, "%d.out", getpid());
            sprintf(errFileName, "%d.err", getpid());

            // Redirect stdout and stderr to output and error files
            freopen(outFileName, "w", stdout);
            freopen(errFileName, "w", stderr);

            // Execute matrixmult_parallel with the specified input files
            execlp(command, command, A_file, W_files[i], NULL);

            // If execlp fails (executing the command), handle the error
            fprintf(stderr, "Error executing %s with arguments %s %s\n", command, A_file, W_files[i]);
            perror("exec error");
            exit(1);
        }
    }

    // Parent process waits for all child processes to finish
    int status;
    pid_t wpid;

    while ((wpid = wait(&status)) > 0) {
        if (WIFEXITED(status)) {
            printf("Finished child %d pid of parent %d\n", wpid, getpid());
            printf("Exited with exitcode = %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            int termSignal = WTERMSIG(status);
            printf("Killed by signal %d: %s\n", termSignal, strsignal(termSignal));

        }
    }
}

int main(int argc, char *argv[]) {


    if (argc < 3) {
        fprintf(stderr, "Usage: %s <A_matrix_file> <W1_matrix_file> [W2_matrix_file] [W3_matrix_file] ...\n", argv[0]);
        exit(1);
    }

    // Loop through additional W matrix files
    for (int i = 2; i < argc; i++) {
        pid_t child_pid = fork();

        if (child_pid == -1) {
            perror("Fork error");
            exit(1);
        }

        // Child process
        if (child_pid == 0) {
            printf("Starting command %d: child %d pid of parent %d\n", i - 1, getpid(), getppid());

            // Read and process matrix A
            int matrixA[8][8] = {0};
            FILE *fileA = fopen(argv[1], "r");
            readMatrixFromFile(fileA, (int *)matrixA, 8, 8);
            fclose(fileA);

            // Read and process matrix Wi
            int matrixWi[8][8] = {0};
            FILE *fileWi = fopen(argv[i], "r");
            readMatrixFromFile(fileWi, (int *)matrixWi, 8, 8);
            fclose(fileWi);

            // Simulate matrix multiplication
            int resultMatrix[8][8] = {0};

            printf("A%s=[\n", argv[i]);
            printMatrix((int *)matrixA, 8, 8);
            printf("]\n");

            printf("W%s=[\n", argv[i]);
            printMatrix((int *)matrixWi, 8, 8);
            printf("]\n");

            printf("Finished child %d pid of parent %d\n", getpid(), getppid());
            exit(0);
        }
    }

    // Parent process waits for all child processes to finish
    int status;
    pid_t wpid;
    while ((wpid = wait(&status)) > 0) {
        if (WIFEXITED(status)) {
            printf("Exited with exitcode = %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Killed with signal %d\n", WTERMSIG(status));
        }
    }

    // Extract the A matrix file and W matrix files
    const char *A_file = argv[1];
    const char *W_files[argc - 2];

    for (int i = 2; i < argc; i++) {
        W_files[i - 2] = argv[i];
    }

    // Execute matrix multiplication with the specified input files in parallel
    executeMatrixMultMultiw(A_file, W_files, argc - 2);

    return 0;
}
