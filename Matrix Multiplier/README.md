# Authors
Name:
Kean Onn Lee,
Shivansh Chhabra


Modified:
09/11/23

Creation Date:
09/09/23

## Description
This C program reads three matrices from input files, performs matrix multiplication and addition, and prints the resulting matrix. It follows specific input file format expectations and handles file input errors.

## How to Compile and Run
To compile the program, use the following command:

```
gcc -o matrixmult matrixmult.c 
```
To run the program, provide the input file names as command-line arguments. Here are some sample commands:
```
Input 1
./matrixmult test/A1.txt test/W1.txt test/B1.txt

Input 2
./matrixmult test/A2.txt test/W2.txt test/B2.txt

Input 3
./matrixmult test/A3.txt test/W3.txt test/B3.txt
```

Expected outputs for input A1:
````
10 30 40
````

Expected outputs for input W1:
````
1 3 4 5 1 
2 4 5 0 0
1 1 5 0 0
````


OutputExpected outputs for input b1:
````
1 1 1 1 1 
````
Expected Result output for A1 W1 B1:
````
111 191 391 51 11

````
Expected Result output A2 W2 B2:
````
701 151 191 51 11
````

Expected Result output A3 W3 B3:
````
11 19 39 55 55
````
Error Handling
If an input file cannot be opened, the program will display an error message and exit with code 1.

if the program does not recive exactly 3 input files as argument, it will display an error message and exit code 1.

