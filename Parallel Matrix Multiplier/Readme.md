# Authors

Modified:

10/10/23

Creation Date:

10/05/23

## Description

This program performs matrix multiplication in parallel using multiple processes. It takes 4 input files, A1.txt and W(i).txt, as command-line arguments, and computes the product A1.txt * W(i).txt and after that processes get executed in parallel, keep logs, track exit codes and signals, and duplicate file descriptors, by using executable of matrixmult_parallel.c.

## How to Compile and Run

To compile the program, use the following command:

```

gcc -o matrixmult_parallel matrixmult_parallel.c
gcc -o matrixmult_multiw matrixmult_multiw.c

```

To run the program, provide the input file names as command-line arguments. Here are some sample commands:

Input
````
./matrixmult_multiw A1.txt W1.txt W2.txt W3.txt
````

Expected output for A3:

````

Starting command 1: child 12165 pid of parent 12164
Starting command 2: child 12166 pid of parent 12164
Starting command 3: child 12167 pid of parent 12164
AW1.txt=[
[
 10 30 40 50 0 ....]
AW.txt[.....]
Finished child 12165 pid of parent 12164
Finished child 12167 pid of parent 12164
Finished child 12166 pid of parent 12164
Exited with exitcode = 0
Finished child 12169 pid of parent 12164
Exited with exitcode = 0
Finished child 12170 pid of parent 12164
Exited with exitcode = 0
Finished child 12168 pid of parent 12164
Exited with exitcode = 0

````

Our output in 12168.out:

````
Child terminated normally with exit code: 0

Result of A1.txt * W1.txt = [
110 190 390 50 10 0 0 0 
....
]
Runtime 0.0030 seconds

````

Our output in 12169.out:

````
Child terminated normally with exit code: 0
Result of A1.txt * W2.txt = [
1600 190 390 50 10 0 0 0 
....
]
Runtime 0.0030 seconds

````

Our output in 12170.out:

````
Child terminated normally with exit code: 0
Result of A1.txt * W3.txt = [
110 190 390 550 550 0 0 0 
....
]
Runtime 0.0029 seconds

````
## Calculate Average Runtime

To determine the average duration across several executions, you can employ the time command within Unix-like operating systems. 

For 4 times...

Run this command:

````
time (./matrixmult_multiw A1.txt W1.txt W2.txt W3.txt && ./matrixmult_multiw A1.txt W1.txt W2.txt W3.txt && ./matrixmult_multiw A1.txt W1.txt W2.txt W3.txt && ./matrixmult_multiw A1.txt W1.txt W2.txt W3.txt)
````
Average runtime Output will be 
````
0.01s user 0.03s system 53% cpu 0.079 total
````
Average runtime: 0.079 seconds

## Error Handling

If an input file cannot be opened, the program will display an error message and exit with code 1.


