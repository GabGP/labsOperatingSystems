# Lab10 - Page Replacement Algorithms

#### By @GabGP and @guillermo-martinez42

This project aims to simulate a normal user-space operating system environment to evaluate how a sequence of virtual page memory accesses behaves under different classic page replacement policies.

Functions:

- Read and parse configuration dynamically from a dataset text file.
- Validate memory constraints to prevent invalid frame configurations.
- Implement fixed-frame algorithms: First-In First-Out (FIFO), Optimal (MIN), and Least Recently Used (LRU).
- Calculate performance metrics including total hits, total misses, and hit rate percentages.
- Generate step-by-step execution traces showing memory states and page evictions.

![Alt text](https://github.com/guillermo-martinez42/labsOperatingSystems/blob/main/lab10/Screenshots/Output1.png "Start")

![Alt text](https://github.com/guillermo-martinez42/labsOperatingSystems/blob/main/lab10/Screenshots/Output2.png "FIFO")

![Alt text](https://github.com/guillermo-martinez42/labsOperatingSystems/blob/main/lab10/Screenshots/Output3.png "MIN")

![Alt text](https://github.com/guillermo-martinez42/labsOperatingSystems/blob/main/lab10/Screenshots/Output4.png "LRU")

![Alt text](https://github.com/guillermo-martinez42/labsOperatingSystems/blob/main/lab10/Screenshots/Output5.png "Comparison Summary")

## 0. How to Run?

You can run the main simulation directly from your terminal:

`python3 page_replacement.py`

Note: The script currently expects an input file located at ../dataset.txt relative to the script's execution directory. The file must contain the number of frames as the first token, followed by the reference sequence.

### Libraries Used

`sys` for handling argument parsing and graceful error exits. No external libraries are required

## 1. Page Replacement Simulation

The system simulates an operating system that allocates a limited number of physical frames ($N \ge 1$) to a process. At any given time, at most $N$ distinct page numbers may reside in memory.

### Memory Access Mechanics

When the simulated CPU references a page, the system checks the frames:

- Hit: If the page is already resident in one of the frames, no replacement is necessary.
- Miss (Page Fault): If the page is not in memory, the system must bring it in. If free slots exist, the page is installed directly. If memory is full, the active policy dictates which page must be evicted to make room.

### Trace Logging

The program utilizes a robust tracking system that visually logs every step. Each output row represents a single reference, displaying the step number, the referenced page, the result (HIT or MISS), the current state of the array, and the victim page evicted (if applicable).

## 2. Algorithms Implemented

### FIFO (First-In, First-Out)

A policy that treats loaded pages as a strict queue ordered by arrival time into memory. When an eviction is required, the oldest resident page is removed.

### MIN (Optimal)

A forward-looking policy that evaluates the future reference string. On eviction, it removes the page whose next appearance occurs farthest in the future. If a resident page never appears again, its distance is considered infinite. Ties are deterministically broken by choosing the smallest page ID.

### LRU (Least Recently Used)

A historical policy that tracks the recency of page usage. Whenever a page is accessed, its "last-use" status is updated. When memory is full, the policy removes the page that has been left unreferenced for the longest period of time.

## 3. Output and Statistics

### Logging System

All outputs are managed by the logging.py file to maintain a clean standard out interface. Empty memory frames are represented with a - character to keep the columns aligned and readable.

### Metrics Calculation

- Hits and Misses: Raw counts are incremented dynamically within each algorithm's loop based on whether the referenced item already exists in the local frame list.

- Hit Rate: Calculated at the end of execution as a percentage with two decimal points to quickly compare the efficiency of FIFO, MIN, and LRU.

## 4. Main Program

The page_replacement.py file serves as the dispatcher. It reads the dataset, parses the integer tokens, and sequentially invokes the high-level runner functions:

- `simulate_fifo()`
- `simulate_min()`
- `simulate_lru()`

The core logic is completely isolated. Runner functions and list manipulations are centralized in `algorithms.py`, keeping the main entry point clean and focused strictly on the simulation pipeline and reporting the final comparison summary block.
