# Authors

Modified:

10/25/23

Creation Date:

10/20/23

## Description

This program performs multiply matrices in parallel, using fork and exec, while maintaining a cumulative sum of the results, and ensuring there are no memory leaks. Expand upon a previous code solution to create a program that multiplies matrices with multiple child processes, updating the cumulative result matrix as you receive new matrices, and you should use valgrind to check for memory leaks.


## How to Compile and Run

To compile the program, use the following command: <br>

```

gcc -o matrixmult_parallel matrixmult_parallel.c
gcc -o matrixmult_multiw_deep matrixmult_multiw_deep.c

```

To run the program, provide the input file names as command-line arguments. Here are some commands:

In Terminal<br>
````

./matrixmult_multiw_deep A1.txt W1.txt W2.txt W3.txt
W4.txt W5.txt W6.txt W7.txt
W8.txt
...
^D <--Ctrl-D is the EOF value, which terminates your input

````

./matrixmult_multiw_deep A1.txt W1.txt W2.txt W3.txt<br>

````
Children Resulting Matrices:
1600 190 390 50 10 0 0 0
110 190 390 50 10 0 0 0
110 190 390 550 550 0 0 0

Resulting Matrix Rsum in A1:
1820 570 1170 650 570 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
Runtime: 0.0027 seconds
````
W4.txt W5.txt W6.txt W7.txt<br>
````

Children Resulting Matrices:
3560 9590 11980 10920 0 0 0 0
5900 7720 9540 16380 14560 9100 0 0
3560 3560 2390 2390 1820 1820 1820 1820
3560 2390 1820 1820 0 0 0 1170

Resulting Matrix Rsum in A1:
16580 23260 25730 31510 16380 10920 1820 2990
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0

Runtime: 0.0028 seconds
````
W8.txt <br>
````

Children Resulting Matrices:
71350 54770 54770 54770 54770 54770 54770 97080

Resulting Matrix Rsum in A1:
71350 54770 54770 54770 54770 54770 54770 97080
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0

Runtime: 0.0025 seconds
````
<br> ^D <--Ctrl-D is the EOF value, which terminates your input


## Calculate Average Runtime

The average runtime we got for 4 runs on the test files that were given was ...

Average time:<br>

````
Execution time: 0.0026 seconds
````

## Valgrind Passed

Screenshot links <br>
https://paste.pics/c77bf3b083e30349722d8d3934582a2f <br>
https://paste.pics/858636d6f3fdc0fe3389348dd9b11ddf


## Error Handling

If an input file cannot be opened, the program will display an error message and exit with code 1.
