# Authors

Name:

Shivansh Chhabra, Kean Onn Lee

Emails: shivansh.chhabra@sjsu.edu, keanonn.lee@sjsu.edu


Modified:

09/24/23

Creation Date:

09/19/23

## Description

This program performs matrix multiplication in parallel using multiple processes. It takes two input files, A.txt and W.txt, as command-line arguments, and computes the product A*W. This README provides instructions on running test cases and the expected output.

## How to Compile and Run

To compile the program, use the following command:

```

gcc -o matrixmult_parallel matrixmult_parallel.c

```

To run the program, provide the input file names as command-line arguments. Here are some sample commands:

```

Input

./matrixmult_parallel A.txt W.txt

```

Expected output for A2:

````

Result of A*W =

11 7 0 0 0 11 0 0

21 13 0 0 0 21 0 0

16 10 0 0 0 16 0 0

16 10 0 0 0 16 0 0

21 13 0 0 0 21 0 0

26 16 0 0 0 26 0 0

26 16 0 0 0 26 0 0

16 10 0 0 0 16 0 0

Runtime 0.01 seconds

````

Our output for A2:

````
Child terminated normally with exit code: 0
Child terminated normally with exit code: 0
Child terminated normally with exit code: 0
Child terminated normally with exit code: 0
Child terminated normally with exit code: 0
Child terminated normally with exit code: 0
Child terminated normally with exit code: 0
Child terminated normally with exit code: 0
Result of A*W = [
11 7 0 0 0 11 0 0 
21 13 0 0 0 21 0 0 
16 10 0 0 0 16 0 0 
16 10 0 0 0 16 0 0 
21 13 0 0 0 21 0 0 
26 16 0 0 0 26 0 0 
26 16 0 0 0 26 0 0 
16 10 0 0 0 16 0 0 
]
Runtime 0.0153 seconds


````

## Calculate Average Runtime

To determine the average duration across several executions, you can employ the time command within Unix-like operating systems. For instance, if you wish to execute the program three times and compute the mean runtime, utilize the subsequent command:

run 4 times...

After running this command, you'll see the real-time for each run.

````

time (./matrixmult_parallel A.txt W.txt && ./matrixmult_parallel A.txt W.txt && ./matrixmult_parallel A.txt W.txt && ./matrixmult_parallel A.txt W.txt)

````

Expected output for A1:

````

Result of A*W =

11 7 0 0 0 11 0 0

21 13 0 0 0 21 0 0

16 10 0 0 0 16 0 0

16 10 0 0 0 16 0 0

21 13 0 0 0 21 0 0

26 16 0 0 0 26 0 0

26 16 0 0 0 26 0 0

16 10 0 0 0 16 0 0 

Runtime 0.0043 seconds

````

````

Result of A*W =

11 7 0 0 0 11 0 0

21 13 0 0 0 21 0 0

16 10 0 0 0 16 0 0

16 10 0 0 0 16 0 0

21 13 0 0 0 21 0 0

26 16 0 0 0 26 0 0

26 16 0 0 0 26 0 0

16 10 0 0 0 16 0 0 

Runtime 0.0027 seconds

````

````

Result of A*W =

11 7 0 0 0 11 0 0

21 13 0 0 0 21 0 0

16 10 0 0 0 16 0 0

16 10 0 0 0 16 0 0

21 13 0 0 0 21 0 0

26 16 0 0 0 26 0 0

26 16 0 0 0 26 0 0

16 10 0 0 0 16 0 0 

Runtime 0.0023 seconds

````

````

Result of A*W =

11 7 0 0 0 11 0 0

21 13 0 0 0 21 0 0

16 10 0 0 0 16 0 0

16 10 0 0 0 16 0 0

21 13 0 0 0 21 0 0

26 16 0 0 0 26 0 0

26 16 0 0 0 26 0 0

16 10 0 0 0 16 0 0 

Runtime 0.0047 seconds

````

````

avg 0.0035s

````

Error Handling

If an input file cannot be opened, the program will display an error message and exit with code 1.

if the program does not receive exactly 2 input files as arguments, it will display an error message.

