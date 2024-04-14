/**
* Description: This module performs Compute Rsum in the parent by summing up results from children for
* each line of W matrices. When the input of W matrices from stdin stops (Ctrl-D), print the final Rsum to stdout and exit,
* ensuring to flush standard streams. Uses pipes to run the program with a file or standard input.
* Last modified date: 10/25/2023
* Creation date: 10/20/2023
**/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_ROWS 8
#define MAX_COLS 8

/**
 * This function updates a matrix based on the input string.
 * Input parameters: matrix, input.
 **/

void updateMatrix(int matrix[MAX_ROWS][MAX_COLS],  char* input) {
    char *token = strtok(input, " ");
    int col = 0;
    while (token != NULL && col < MAX_COLS) {
        matrix[0][col++] += atoi(token);
        token = strtok(NULL, " ");
    }
}

/**
 * This function executes matrix multiplication with multiple W files using child processes,
 * communicates with the child processes through pipes, and computes and stores the Rsum matrix.
 * Input parameters: A_file, W_files, numW - the number of W files, rsum - the Rsum matrix.
 **/
void executeMatrixMultMultiw(const char *A_file, char *W_files[], int numW, int (*rsum)[MAX_COLS]) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("Pipe error"); // Create a pipe to communicate between parent and child processes
        exit(1);
    }

    for (int i = 0; i < numW; i++) {
        pid_t child_pid = fork(); // Fork a child process

        if (child_pid == -1) {
            perror("Fork error"); // Check for fork failure
            exit(1);
        }

        if (child_pid == 0) {
            close(pipefd[0]); // Close the read end of the pipe
            dup2(pipefd[1], STDOUT_FILENO); // Redirect child process's standard output to the pipe
            execlp("./matrixmult_parallel", "./matrixmult_parallel", A_file, W_files[i], NULL);
            perror("exec error");
            exit(1);
        }
    }

    close(pipefd[1]); // Close the write end of the pipe in the parent process

    char buffer[1024];
    ssize_t bytesRead;

    printf("Children Resulting Matrices:\n");

    // Read the results from the children processes through the pipe
    while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
        int result;
        buffer[bytesRead] = '\0';
        printf("%s", buffer);

        updateMatrix(rsum,buffer); // Update the Rsum matrix with the received data
    }

    printf("\n");
    close(pipefd[0]); // Close the read end of the pipe in the parent process

    FILE *outfile = fopen(A_file, "w"); // Open A_file in write mode
    if (outfile == NULL) {
        perror("Error opening file for writing");
        exit(1);
    }

    // Write the resulting rsum array to the file
    for (int i = 0; i < 8; i++) {
        fprintf(outfile, "%d ", rsum[0][i]);
    }
    fprintf(outfile, "\n");
    fclose(outfile); // Close the file

    printf("Resulting Matrix Rsum in A1:\n");

    // Print the resulting Rsum matrix and set to 0s after printing
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%d ", rsum[i][j]);
            rsum[i][j] = 0;
        }
        printf("\n");
    }

    printf("\n");
}

/**
 * This function reads command-line arguments, processes matrix multiplication with A and multiple W files,
 * and perform more matrix multiplications.
 **/

int main(int argc, char *argv[]) {
    // Check if the correct number of command-line arguments is provided
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <A_matrix_file> <W1_matrix_file> [W2_matrix_file] [W3_matrix_file] ...\n", argv[0]);
        exit(1);
    }

    const char *A_file = argv[1]; // Set A_file to the first command-line argument
    char *W_files[argc - 2]; // Create an array to store W filenames

    for (int i = 2; i < argc; i++) {
        W_files[i - 2] = argv[i]; // Populate the W_files array with command-line arguments
    }

    int rsum[MAX_ROWS][MAX_COLS] = {{0}}; // Initialize the Rsum matrix with zeros
    executeMatrixMultMultiw(A_file, W_files, argc - 2, rsum); // Perform matrix multiplication with A and W files

    char** FILES = NULL; // Initialize to NULL initially
    char line[100];
    int numFILES = 0;

    // Read input file names from standard input
    while (fgets(line,100, stdin)) {
        char* token = strtok(line, " \t\n");
        while (token != NULL) {
            numFILES++;
            FILES = realloc(FILES, numFILES * sizeof(char*));
            FILES[numFILES - 1] = malloc(strlen(token) + 1);
            strcpy(FILES[numFILES - 1], token);
            token = strtok(NULL, " \t\n");
        }

        if (feof(stdin)) {
            break; // Reached end of input (Ctrl+D)
        }
        executeMatrixMultMultiw(A_file, FILES, numFILES, rsum);

        // Free allocated memory for the next line
        for (int i = 0; i < numFILES; i++) {
            free(FILES[i]);
        }
        free(FILES);
        FILES = NULL;
        numFILES = 0;
    }

    return 0;
}
