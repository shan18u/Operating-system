# Authors

Modified:

12/05/23

Creation Date:

12/05/23

## Description

The provided code handle dynamically allocated memory for matrices. The parent process continuously reads new 8x8 matrices from stdin, sends them to child processes through pipes, and each child multiplies the received matrices with a pre-existing matrix Wi, dynamically reallocating memory to store the results. The children don't exit and keep waiting for more matrices. Each child writes the input filenames (A and Wi) to PID.out and prints the final resulting matrix Ri. The program utilizes fork/exec for parallel execution and employs dynamic memory allocation (malloc and realloc) for handling a number of matrices.


## How to Compile and Run

To compile the program, use the following command: <br>

Please Use CLion

```

gcc -o matrixmult_parallel matrixmult_parallel.c
gcc -0 matrixmult_multiwa matrixmult_multiwa.c

```

To run the program, provide the input file names as command-line arguments.

##  In Terminal<br>


./matrixmult_multiwa A1.txt W1.txt W2.txt W3.txt<br>
A2.txt<br>
A3.txt<br>
A1.txt<br>
A2.txt<br>
A3.txt<br>
...<br>
^D <--Ctrl-D is the EOF value, which terminates your input
<br>

Creates PID.out files
````
Starting command 1: child 5397 pid of parent 5396
A = A1.txt
W = W1.txt
R = [
110 190 390 50 10 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
70 150 190 50 10 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
11 19 39 5 1 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
110 190 390 50 10 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
70 150 190 50 10 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
11 19 39 5 1 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 ]
Finished child 5397 pid of parent 5396
Exited with exitcode = 0

````

<br>

````
Starting command 2: child 5398 pid of parent 5396
A = A1.txt
W = W2.txt
R = [
1600 190 390 50 10 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
700 150 190 50 10 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
280 19 39 5 1 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
1600 190 390 50 10 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
700 150 190 50 10 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
280 19 39 5 1 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 ]
Finished child 5398 pid of parent 5396
Exited with exitcode = 0

````

<br>

````
Starting command 3: child 5399 pid of parent 5396
A = A1.txt
W = W3.txt
R = [
110 190 390 550 550 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
70 150 190 110 70 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
11 19 39 55 55 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
110 190 390 550 550 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
70 150 190 110 70 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
11 19 39 55 55 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 ]
Finished child 5399 pid of parent 5396
Exited with exitcode = 0

````
<br>

valgrind --leak-check=full ./matrixmult_multiwa A1.txt W1.txt W2.txt W3.txt<br>

````
cs149@cs149-VirtualBox:~/Desktop/cs149/BONUS$ valgrind --leak-check=full ./matrixmult_multiwa A1.txt W1.txt W2.txt W3.txt
==4524== Memcheck, a memory error detector
==4524== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==4524== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==4524== Command: ./matrixmult_multiwa A1.txt W1.txt W2.txt W3.txt
==4524==
Enter file path of a matrix:
A2.txt
Enter file path of a matrix (Ctrl+D to exit):
A3.txt            
Enter file path of a matrix (Ctrl+D to exit):
A1.txt
Enter file path of a matrix (Ctrl+D to exit):
A2.txt
Enter file path of a matrix (Ctrl+D to exit):
A3.txt
Enter file path of a matrix (Ctrl+D to exit):

Runtime: 0.058189 secs
==4524==
==4524== HEAP SUMMARY:
==4524==     in use at exit: 0 bytes in 0 blocks
==4524==   total heap usage: 12 allocs, 12 frees, 2,813 bytes allocated
==4524==
==4524== All heap blocks were freed -- no leaks are possible
==4524==
==4524== For lists of detected and suppressed errors, rerun with: -s
==4524== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)

````
<br>
## Error Handling

If an input file cannot be opened, the program will display an error message and exit with code 1. Inside .err
