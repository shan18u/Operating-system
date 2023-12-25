/**
* Description: This module reads three matrices from input files, performs  matrix
* multiplication and addition and prints the resulting matrix. Also handles file input errors.
* Author names: Shivansh Chhabra, Kean lee
* Last modified date: 09/11/2023
* Creation date: 09/09/ 2023
**/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Function to read a matrix from a file into D array
void readMatrixFromFile(FILE *file, int *matrix, int rows, int cols) {

    char buffer[1024]; // Assuming a line will have less than 1024 characters

    for(int i = 0; i < rows; i++) {
        if(fgets(buffer, sizeof(buffer), file) != NULL) {
            char *token = strtok(buffer, " ");
            for(int j = 0; j < cols; j++) {
                if(token != NULL) {
                    matrix[i * cols + j] = atoi(token); // Convert token to integer and store in matrix
                    token = strtok(NULL, " ");
                } else {
                    matrix[i * cols + j] = 0; // If no more tokens, set matrix element to 0
                }
            }
        }
    }
}

// Function to print a matrix
void printMatrix(int *matrix, int rows, int cols) {
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            printf("%d ", matrix[i * cols + j]);
        }
        printf("\n");
    }
}

// Function to perform matrix multiplication and addition
void multiplyAndAddMatrices(int *A, int *W, int *B, int *R) {
    // Perform matrix multiplication: R = A * W
    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < 5; j++) {
            R[i * 5 + j] = 0;
            for (int k = 0; k < 3; k++) {
                R[i * 5 + j] += A[i * 3 + k] * W[k * 5 + j];
            }
        }
    }

    // Perform matrix addition: R = R + B
    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < 5; j++) {
            R[i * 5 + j] += B[i * 5 + j];
        }
    }
}


// Check if the correct number of command-line arguments is provided
int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "error: expecting exactly 3 files as input\nTerminating, exit code 1.\n");
        exit(1);
    }

    // Open the input files
    FILE *fileA = fopen(argv[1], "r");
    FILE *fileW = fopen(argv[2], "r");
    FILE *fileB = fopen(argv[3], "r");

    bool error = false;
    // Check if the files could be opened or not
    if (!fileA) {
        fprintf(stderr, "Error opening file %s.\n", argv[1]);
        error = true;
    }

    if (!fileW) {
        fprintf(stderr, "Error opening file %s.\n", argv[2]);
        error = true;
    }

    if (!fileB) {
        fprintf(stderr, "Error opening file %s.\n", argv[3]);
        error = true;
    }
    // If any error occured, Terminate with exit code
    if (error) {
        printf("Terminating, exit code 1.\n");
        exit(1);
    }

    // Matrices and Result matrix
    int A[3] = {0};
    int W[15] = {0};
    int B[5] = {0};
    int R[5] = {0}; // Result matrix

    // Read matrices from files using readMatrixFromFile
    readMatrixFromFile(fileA, A, 1, 3);
    readMatrixFromFile(fileW, W, 3, 5);
    readMatrixFromFile(fileB, B, 1, 5);

    fclose(fileA);
    fclose(fileW);
    fclose(fileB);

    // Print matrices
    printf("Matrix A:\n");
    printMatrix(A, 1, 3);

    printf("\nMatrix W:\n");
    printMatrix(W, 3, 5);

    printf("\nMatrix B:\n");
    printMatrix(B, 1, 5);

    // Perform matrix multiplication and addition
    multiplyAndAddMatrices(A, W, B, R);

    // Print the result matrix
    printf("\nResult Matrix R:\n");
    printMatrix(R, 1, 5);

    return 0;
}