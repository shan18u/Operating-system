/**
* Description: This module performs matrix multiplication using multithreading. It reads matrices from files specified as command-line arguments, Calculates them in parallel using threads, and prints the resulting matrix.
 
* Last modified date: 12/05/2023
* Creation date: 12/05/2023
**/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_COLUMNS 8
#define MAX_ROWS 8
#define MAX_THREADS 8
#define PRODUCT (MAX_ROWS * MAX_COLUMNS)

int matrixSize;
int input[MAX_ROWS][MAX_COLUMNS];
int *finalResultantMatrix;
int weights[MAX_ROWS][MAX_COLUMNS];
int tempResultant[MAX_ROWS][MAX_COLUMNS];

pthread_mutex_t mutex; // Mutex lock for synchronization

int doMatrixMult(int *aMatrix, int *tempResult);
void rowSum(const int *matrix1, const int *matrix2, int *product, const int row);
void fillRow(const int row, const int *sourceMatrix, int *resultant);
int readAMatrix();
int closeAll(FILE *A, FILE *W, int *toFreeArray);
void printArr(const int *resultant, const int size);
void fillMatrix(int *resultantMatrix, const int rows, const int columns, FILE *file);
void appendToResultant(int *tempResult);
void *matrixMultThread(void *args);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Error: Expecting exactly 3 inputs.\n");
        fprintf(stderr, "Terminating, exit code 1.\n");
        fflush(stderr);
        exit(1);
    }

    // create file pointers
    FILE *A = fopen(argv[1], "r");
    FILE *W = fopen(argv[2], "r");

    if (A == NULL || W == NULL) {
        if (A == NULL)
            fprintf(stderr, "Error: Cannot open file %s\n", argv[1]);

        if (W == NULL)
            fprintf(stderr, "Error: Cannot open file %s\n", argv[2]);

        fprintf(stderr, "Terminating, exit code 1.\n");
        exit(closeAll(A, W, finalResultantMatrix));
    }

    fillMatrix(&(input[0][0]), MAX_ROWS, MAX_COLUMNS, A);
    fillMatrix(&(weights[0][0]), MAX_ROWS, MAX_COLUMNS, W);

    finalResultantMatrix = (int *)malloc(PRODUCT * sizeof(int));
    if (finalResultantMatrix == NULL) {
        fprintf(stderr,
                "Memory allocation failed. Refer to prior messages for exact "
                "details. A matrix %s, W matrix %s.",
                argv[1], argv[2]);
        exit(closeAll(A, W, finalResultantMatrix));
    }
    matrixSize += PRODUCT;

    // Initialize mutex
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        fprintf(stderr, "Mutex initialization failed.\n");
        exit(closeAll(A, W, finalResultantMatrix));
    }

    int tempResultant[MAX_ROWS][MAX_COLUMNS];
    if (doMatrixMult(&(input[0][0]), &(tempResultant[0][0])) == 1) {
        fprintf(stderr, "Matrix Multiplication with CLI args failed.\n");
        exit(closeAll(A, W, finalResultantMatrix));
    }

    appendToResultant(&tempResultant[0][0]);

    if (readAMatrix() == 1) {
        fprintf(stderr, "Matrix Multiplication with passed in A matrix failed.\n");
        exit(closeAll(A, W, finalResultantMatrix));
    }

    fprintf(stdout, "A = %s\n", argv[1]);
    fprintf(stdout, "W = %s\n", argv[2]);
    fprintf(stdout, "R = [ \n");
    printArr(finalResultantMatrix, matrixSize);
    free(finalResultantMatrix);

    // Flush stdout and stderr
    fflush(stdout);
    fflush(stderr);

    // Reset stdout
    if (dup2(atoi(argv[3]), STDOUT_FILENO) == -1) {
        fprintf(stderr, "Error redirecting stdout to terminal.\n");
        exit(closeAll(A, W, finalResultantMatrix));
    }

    // Close the files to prevent memory leaks.
    fclose(A);
    fclose(W);

    // Destroy the mutex
    pthread_mutex_destroy(&mutex);

    return 0;
}


// Fills the given matrix with the number of rows and columns with values from file
void fillMatrix(int *resultantMatrix, const int rows, const int columns, FILE *file) {
    const int product = rows * columns;
    int *preOp = resultantMatrix; // Store the memory address
    int index = 0;
    char line[500];
    while (index < product && (!feof(file))) {
        // Only get in if the line isn't NULL.
        if (fgets(line, sizeof(line), file) != NULL) {
            // Check if the last character of the string is actually a newline
            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0'; // Get rid of newline character.
            }

            char *fetch = strtok(line, " "); // Read in a number separated by spaces in the file.

            // Keeps track of how many columns have been traversed.
            int columnCounter = columns;

            while (fetch != NULL && index < product) {
                columnCounter--;
                int number = atoi(fetch);
                *(resultantMatrix) = number;
                resultantMatrix++;
                index++;
                fetch = strtok(NULL, " ");
            }
            resultantMatrix += columnCounter;
        }
    }

    // return pointer to its valid memory location if the resultantMatrix pointer is incremented
    if (resultantMatrix > preOp)
        resultantMatrix = preOp;
}

// Multiplies the first and second matrix parallely using threads
int doMatrixMult(int *aMatrix, int *tempResult) {
    pthread_t threads[MAX_THREADS];

    // Create threads to calculate dot product of each row of the matrix
    for (int i = 0; i < MAX_THREADS; ++i) {
        int *threadArgs = (int *)malloc(sizeof(int));
        if (threadArgs == NULL) {
            fprintf(stderr, "Memory allocation failed for thread arguments.\n");
            return 1;
        }
        *threadArgs = i;

        if (pthread_create(&threads[i], NULL, matrixMultThread, (void *)threadArgs) != 0) {
            fprintf(stderr, "Error creating thread %d.\n", i + 1);
            free(threadArgs);
            return 1;
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < MAX_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

// Thread function to calculate dot product of a row
void *matrixMultThread(void *args) {
    int threadIndex = *((int *)args);
    free(args);

    for (int i = threadIndex; i < MAX_ROWS; i += MAX_THREADS) {
        rowSum(&(input[i][0]), &(weights[i][0]), tempResultant[i], i);
        pthread_mutex_lock(&mutex);
        appendToResultant(tempResultant[i]);
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}
// Sums the ith row of the first matrix with the second matrix
void rowSum(const int *matrix1, const int *matrix2, int *product, const int row) {
    for (int i = 0; i < MAX_COLUMNS; i++) {
        *(product + row * MAX_COLUMNS + i) = *(matrix1 + row * MAX_COLUMNS + i) +
                                             *(matrix2 + row * MAX_COLUMNS + i);
    }
}

// Prints the array
void printArr(const int *resultant, const int size) {
    for (int i = 0; i < size; i++) {
        fprintf(stdout, "%d ", *(resultant + i));
        if ((i + 1) % MAX_COLUMNS == 0)
            fprintf(stdout, "\n");
    }
    fprintf(stdout, "];\n");
}

// Fills in the resultant array at the end of the parallel matrix multiplication
void appendToResultant(int *tempResult) {
    for (int i = 0; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLUMNS; j++) {
            *(finalResultantMatrix + i * MAX_COLUMNS + j) = *(tempResult + i * MAX_COLUMNS + j);
        }
    }
}

// Reads from A matrix passed in through the terminal
int readAMatrix() {
    char line[500];
    int *resultant = (int *)malloc(PRODUCT * sizeof(int));
    if (resultant == NULL) {
        fprintf(stderr, "Memory allocation failed for A matrix.");
        return 1;
    }
    int index = 0;
    while (!feof(stdin)) {
        if (fgets(line, sizeof(line), stdin) != NULL) {
            // Check if the last character of the string is actually a newline
            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0'; // Get rid of newline character.
            }

            char *fetch = strtok(line, " "); // Read in a number separated by spaces in the file.

            while (fetch != NULL && index < matrixSize) {
                int number = atoi(fetch);
                *(resultant + index) = number;
                index++;
                fetch = strtok(NULL, " ");
            }
        }
    }

    // Close the file to prevent memory leaks.
    free(finalResultantMatrix);
    finalResultantMatrix = resultant;
    matrixSize += PRODUCT;

    return 0;
}

// Close all files and free all memory.
int closeAll(FILE *A, FILE *W, int *toFreeArray) {
    fclose(A);
    fclose(W);
    free(toFreeArray);
    return 1;
}
