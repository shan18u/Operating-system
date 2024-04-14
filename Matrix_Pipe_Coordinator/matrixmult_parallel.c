/**
* Description: This module performs matrix multiplication, utilizing forked processes for parallel computation.
* It reads input matrices from files specified as command-line arguments, conducts parallel computations using pipes, and outputs the result to the standard output or a redirected terminal.
* Last modified date: 12/05/2023
* Creation date: 12/05/2023
**/


#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_COLUMNS 8
#define MAX_ROWS 8
#define MAX_PROCESSES 8
#define PRODUCT (MAX_ROWS * MAX_COLUMNS)

int matrixSize;
int input[MAX_ROWS][MAX_COLUMNS];	
int *finalResultantMatrix;
int weights[MAX_ROWS][MAX_COLUMNS]; 


int doMatrixMult(int *aMatrix, int *tempResult);
void rowSum(const int *matrix1, const int *matrix2, int *product, const int row);
void fillRow(const int row, const int *sourceMatrix, int *resultant);
int readAMatrix();
int closeAll(FILE *A, FILE *W, int *toFreeArray);
void printArr(const int *resultant, const int size);
void fillMatrix(int *resultantMatrix, const int rows, const int columns, FILE *file);
void appendToResultant(int *tempResult);



int main(int argc, char *argv[]){

	if (argc != 4){
		fprintf(stderr, "error: expecting exactly 3 inputs.\n");
		fprintf(stderr, "Terminating, exit code 1.\n");
		fflush(stderr);
		exit(1);
	}

    // create file pointers
	FILE *A = fopen(argv[1], "r");
	FILE *W = fopen(argv[2], "r");

	
	if (A == NULL || W == NULL){
		if (A == NULL) fprintf(stderr, "error: cannot open file %s\n", argv[1]);
		

		if (W == NULL) fprintf(stderr, "error: cannot open file %s\n", argv[2]);
		
		fprintf(stderr, "Terminating, exit code 1.\n");
		exit(closeAll(A, W, finalResultantMatrix));
	}

	fillMatrix(&(input[0][0]), MAX_ROWS, MAX_COLUMNS, A);
	fillMatrix(&(weights[0][0]), MAX_ROWS, MAX_COLUMNS, W);

	finalResultantMatrix = (int *)malloc(PRODUCT * sizeof(int));
	if (finalResultantMatrix == NULL){
		fprintf(stderr,
				"Memory allocation failed. Refer to prior messages for exact "
				"details. A matrix %s, W matrix %s.",
				argv[1], argv[2]);
		exit(closeAll(A, W, finalResultantMatrix));
	}
	matrixSize += PRODUCT;

	int tempResultant[MAX_ROWS][MAX_COLUMNS];
	if (doMatrixMult(&(input[0][0]), &(tempResultant[0][0])) == 1){
		fprintf(stderr, "Matrix Multiplication with CLI args failed.\n");
		exit(closeAll(A, W, finalResultantMatrix));
	}

	appendToResultant(&tempResultant[0][0]);

	if (readAMatrix() == 1){
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
	if (dup2(atoi(argv[3]), STDOUT_FILENO) == -1){
		fprintf(stderr, "Error redirecting stdout to terminal.\n");
		exit(closeAll(A, W, finalResultantMatrix));
	}

	// Close the files to prevent memory leaks.
	fclose(A);
	fclose(W);

	return 0;
}

// Fills the given matrix with the number of rows and columns with values from file
void fillMatrix(int *resultantMatrix, const int rows, const int columns, FILE *file){
	const int product = rows * columns;
	int *preOp = resultantMatrix; // Store the memory address 
	int index = 0; 
	char line[500]; 
	while (index < product && (!feof(file))){
		// Only get in if the line isn't NULL.
		if (fgets(line, sizeof(line), file) != NULL){

			// Check if the last character of the string is actually a newline
			if (line[strlen(line) - 1] == '\n'){
				line[strlen(line) - 1] = '\0'; // Get rid of newline character.
			}

			char *fetch = strtok(line, " "); // Read in a number separated by spaces in the file.

			// Keeps track of how many columns have been traversed.
			int columnCounter = columns;

			while (fetch != NULL && index < product){
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

    //return pointer to its valid memory location if the resultantMatrix pointer is incremented
	if (resultantMatrix > preOp) resultantMatrix = preOp;
	
}

//Multiplies the first and second matrix parallely
int doMatrixMult(int *aMatrix, int *tempResult){
	// Creates read and write pipes for each child process.
	int fd[MAX_PROCESSES][2];

	// Create pipes to read and write for all processes.
	for (int i = 0; i < MAX_PROCESSES; i++){
		if (pipe(fd[i]) == -1){
			fprintf(stderr, "Error creating pipes.\n");
			exit(1);
		}
	}

	// Calculate dot product of each row of the matrix 
	for (int i = 0; i < MAX_PROCESSES; ++i){

		// Store the PID of each process. 
		pid_t pid = fork(); // Hold PIDs for child process.

		if (pid < 0){
			fprintf(stderr, "fork() failed.\n");
			exit(1);
		}
		else if (pid == 0){
			// Close unnecessary read and write ends of the pipe.
			for (int j = 0; j < MAX_PROCESSES; ++j){
				close(fd[j][0]); 
				if (j != i) close(fd[j][1]); // Close all write ends except for the current.
				
			}

            // ends the process if passing the row number to parent process fails
			if (write(fd[i][1], &i, sizeof(int)) == -1){
				fprintf(stderr,
						"Error while writing row number. Problematic child: %d. "
						"Iteration: %d.\n",
						getpid(), i);
				exit(1);
			}

			// Calculate and store the dot product of specific row.
			int rowResult[MAX_COLUMNS];
			rowSum(aMatrix, &(weights[0][0]), rowResult, i);

			if (write(fd[i][1], rowResult, sizeof(int) * MAX_COLUMNS) == -1){
				fprintf(stderr,
						"Error while writing array. Problematic child: %d. Iteration: "
						"%d.\n",
						getpid(), i);
				exit(1);
			}

			close(fd[i][1]); // Close write pipe once written.
			exit(0);		 // End the child process so it doesn't fork itself.
		}
	}

	// Parent process.
	for (int i = 0; i < MAX_PROCESSES; ++i){
		int wstatus;
		int childPID = wait(&wstatus); // Wait for each child process to end.

		if (WIFEXITED(wstatus)){
			// Check if the child process exited normally.
			int exitStatus =
				WEXITSTATUS(wstatus); // Store exit code of child process.
			if (exitStatus == 0){
				// If the child process exited with code 0 (success)
				// Holds the row values returned by the child process.
				int rowResult[MAX_COLUMNS];

				int rowCompleted; // The row that the child process calculated.

				// Reads in the row that the child process calculated.
				if (read(fd[i][0], &rowCompleted, sizeof(int)) == -1){
					fprintf(stderr, "Error while reading value. Problematic child: %d\n",
							childPID);
					exit(1);
				}

				// Reads in the row values that the child process calculated.
				if (read(fd[i][0], rowResult, sizeof(int) * MAX_COLUMNS) == -1){
					fprintf(stderr, "Error while reading value. Problematic child: %d\n",
							childPID);
					exit(1);
				}

				close(fd[i][0]); // Close the read pipe after use.
				fillRow(rowCompleted, rowResult, tempResult);
			}
			else{
				// In case the child process failed.
				fprintf(stderr, "Child %d exited abnormally with code %d.\n", childPID,
						exitStatus);
				exit(1);
			}
		}
	}
	return 0;
}

int readAMatrix(){
	while (1){
		size_t bufferLen;

		if (read(STDIN_FILENO, &bufferLen, sizeof(size_t)) == 0) break;

		if (bufferLen == 0) break;
		
		char aMatrixFile[bufferLen + 1];

		if (read(STDIN_FILENO, aMatrixFile, bufferLen) == -1){
			fprintf(stderr, "Error copying A matrix filename from pipe.\n");
			return 1;
		}

		aMatrixFile[bufferLen] = '\0';

		FILE *aMatrix = fopen(aMatrixFile, "r");
		if (aMatrix == NULL){
			fprintf(stderr, "error: cannot open file %s read in from stdin\n",
					aMatrixFile);
			fprintf(stderr, "Terminating, exit code 1.\n");
			return 1;
		}

		int tempA[MAX_ROWS][MAX_COLUMNS];
		memset(tempA, 0, sizeof(tempA));

		fillMatrix(&(tempA[0][0]), MAX_ROWS, MAX_COLUMNS, aMatrix);

		int tempAResult[0][0];
		memset(tempAResult, 0, sizeof(tempAResult));

		if (doMatrixMult(&(tempA[0][0]), &(tempAResult[0][0]))){
			fprintf(stderr, "Matrix Multiplication with stdin args failed.\n");
			return 1;
		}

		matrixSize += PRODUCT;
		int *tempResultArray = (int *)realloc(finalResultantMatrix, matrixSize * sizeof(int));
		if (tempResultArray == NULL){
			fprintf(stderr, "realloc() failed for matrix %s.", aMatrixFile);
			matrixSize -= PRODUCT;
			return 1;
		}
		finalResultantMatrix = tempResultArray;

		appendToResultant(&(tempAResult[0][0]));
	}

	return 0;
}

// Appends the value of the first array to the end of the second
void appendToResultant(int *tempResult){
	
    int *resultStart = finalResultantMatrix + (matrixSize - PRODUCT);
	for (int i = 0; i < PRODUCT; ++i){
		*(resultStart + i) = *(tempResult + i);
	}
}

// Calculates the dot product of the specified row in matrix1 with the entirety of matrix2, and stores the result in the product result.
void rowSum(const int *matrix1, const int *matrix2, int *product, const int row) {
    const int rowStart = row * MAX_COLUMNS;

    for (int i = 0; i < MAX_COLUMNS; ++i) {
        int result = 0;

        for (int j = 0; j < MAX_COLUMNS; ++j) {
            const int rowIncrement = rowStart + j;
            const int columnIncrement = j * MAX_COLUMNS + i;
            result += matrix1[rowIncrement] * matrix2[columnIncrement];
        }

        *(product + i) = result;
    }
}


// Fills the specified row in the resultant matrix with the values from the source matrix.
void fillRow(const int row, const int *sourceMatrix, int *resultant){
	const int rowStart = row * MAX_ROWS;
	for (int i = 0; i < MAX_COLUMNS; ++i){
		*(resultant + (rowStart + i)) = *(sourceMatrix + i);
	}
}

//Flushes stdout and stderr, and then closes the passed in files.
int closeAll(FILE *A, FILE *W, int *toFreeArray){
	fflush(stdout);
	fflush(stderr);

	// Close the files to prevent memory leaks.
	if (A == NULL || W == NULL)
	{
		if (A != NULL) fclose(A);
		

		if (W != NULL) fclose(W);
		
	}

	// Free allocated memory
	if (toFreeArray != NULL){
		free(toFreeArray);
		toFreeArray = NULL;
	}

	return 1;
}

//Prints the contens of the array.
void printArr(const int *resultant, const int size){
	for (int i = 0; i < size; ++i){
		if ((i != 0) && (i % MAX_COLUMNS == 0)) fprintf(stdout, "\n");
		
		fprintf(stdout, "%d ", *(resultant + i));
	}
	fprintf(stdout, "]\n");
}
