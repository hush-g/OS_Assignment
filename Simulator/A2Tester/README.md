# Scheduler Code Tester

This repository contains a tester code that runs your scheduler code written in C against 29 test cases with specific parameters. Before running the tester, please make sure you follow the instructions below to set up your code correctly.

## Clarifications

### MLFQ Scheduling Algorithm Behavior

The tester code assumes that you are implementing the MLFQ (Multi-Level Feedback Queue) scheduling algorithm. In the case where a process is running and a priority boost is required, the following steps are taken:

1. If a process is currently running and a priority boost occurs, the currently running process  is put in front of the highest priority queue and all other processes are boosted in the order first mid queue and then the lowest priority queue. After boosting is completed, the MLFQ functions normally then.

2. Once the running process completes its time quantum or burst time, it will be moved to a lower priority queue.


### Result Precision

The tester code evaluates the results produced by your scheduler code with a precision of up to 3 decimal places. 

## Instructions

1. Place your scheduler code in this folder.
2. Rename your `.c` file to `scheduler.c`.
3. Ensure that your `scheduler.c` file contains the necessary functions and logic to implement the scheduling algorithms with the behavior described above.
4. Open a terminal in this folder and compile your `tester.py` code using a python compiler 

5. The tester code will execute your `scheduler.c` code against 29 test cases with the following parameters: 6, 5, 3, 2, 8.

6. After running the tests, the output will indicate whether your scheduler code produced the expected results for each test case.

## Disclaimer

This tester code is provided as-is and may not cover all possible edge cases or scenarios. It is recommended to thoroughly test your scheduler code on various inputs beyond the provided test cases to ensure its correctness and reliability.

Feel free to modify the tester code or add more test cases according to your needs.

GGEZ

PS- Please share if there is some discrepancy in the expected output.



