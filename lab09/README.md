# Lab09 - Virtual Address Translation and Page Tables

#### By @GabGP and @guillermo-martinez42

This project implements a software simulation that models paged address translation for a single loaded process. It explores how an operating system maps virtual addresses to physical memory by splitting virtual addresses into a Virtual Page Number (VPN) and an offset, managing a linear page table, and allocating physical frames from a fixed-size RAM pool.

## 0. How to Run?

You can run the simulation script directly from your terminal using Python. The program reads a text file containing unsigned virtual addresses, with one address per line.

`python3 memory_management.py`

### Libraries Used

* `sys` and `math` for handling address calculations and argument parsing. No external libraries are required.

## 1. Memory Management Concepts

### Virtual Address Format

The system utilizes a fixed 16-bit unsigned virtual address space ranging from 0x0000 to 0xFFFF.

* Page / Frame Size: 256 bytes.
* Offset: Bits [7:0] (8 bits) representing the byte index inside the page. It is extracted via virtual_address & 0xFF.
* VPN: Bits [15:8] (8 bits) representing the Virtual Page Number. It is extracted via (virtual_address >> 8) & 0xFF.

### Physical Memory (RAM) Layout

* Total Frames: The simulated physical memory contains 100 frames with indices from 0 to 99.
* Frame States: Each frame is marked as either FREE or OCCUPIED.
* Random Initialization: Before loading a process, a random subset of 10 to 60 frames is marked as OCCUPIED to simulate pre-existing memory holes. The initialization loop ensures that the total FREE frame count remains $\ge \max(10, V)$ where $V$ is the number of virtual pages.

### Page Table Structure

* Linear Page Table: The process maintains a single linear page table with NUM_VIRTUAL_PAGES entries.
* Page Table Entry (PTE): Each entry tracks a valid boolean and the corresponding Physical Frame Number (pfn) when valid.

## 2. Simulation Logic

The software models the exact behavior of an operating system's memory allocation and translation process:

1. RAM Initialization & Map Display: Generates the random physical RAM layout and immediately prints a summary (FREE vs OCCUPIED counts) followed by a per-frame grid listing.
2. Process Loading: Iterates from $i = 0$ to $V-1$, allocating a free frame for each page and updating the page table entry. If allocation fails mid-load, it aborts with a clear message.
3. Batch Address Translation: Translates each address parsed from the text file using the formula $PA = PTE[vpn].pfn \times 256 + offset$.
4. Error Handling: The system validates and throws explicit error codes for invalid accesses:
   * VA_OUT_OF_RANGE: If an input address is strictly greater than 0xFFFF.
   * VPN_OUT_OF_RANGE: If the parsed vpn $\ge$ NUM_VIRTUAL_PAGES.
   * PAGE_NOT_MAPPED: If the PTE[vpn].valid bit is false.

## 3. Extras Implemented

### 2-D RAM Grid Visualization

Displays the 100 physical frames in a compact, scannable 2-dimensional grid format to clearly visualize memory holes before process allocation.
