
# Virtual Memory Manager

## 👀 Overview

This project implements a Virtual Memory Manager based on concepts from "Operating System Concepts" by Silberschatz, et al., 10th edition, page P-51. The implementation focuses on translating logical addresses into physical addresses using specific requirements and modifications detailed below.

## 🔧  Implementation Details

### Input File Format

The program reads a file containing several 32-bit integer numbers representing logical addresses. It processes each address by masking the rightmost 16 bits to extract an 8-bit page number and an 8-bit page offset.

![image](https://github.com/brunoribeirol/virtual-memory-manager/assets/89156916/4b2f1a74-969e-401f-8fa5-4ccb0fea456b)

### Address Translation

Using a TLB (Translation Lookaside Buffer) and a page table, the program translates each logical address into its corresponding physical address. The virtual address space size is 216 = 65,536 bytes.

![image](https://github.com/brunoribeirol/virtual-memory-manager/assets/89156916/59391b7a-a1a7-46f2-9d8d-e2745b3c6cc1)

### Page Replacement Algorithm

- **TLB (Translation Lookaside Buffer)**:
  - 16 entries with a FIFO (First-In, First-Out) replacement strategy.

- **Page Table**:
  - 256 entries to map logical page numbers to physical frame numbers.

### Output

For each translated physical address, the program retrieves and outputs the byte stored at that address in physical memory.
    
## 📋 Specifications

- **Page Table**:
  - 256 entries
- **Page Size**:
  - 256 bytes
- **TLB Entries**:
  - 16 entries
- **Frame Size**:
  - 256 bytes
- **Physical Memory**:
  - 65,536 bytes (256 frames × 256-byte frame size)

## 🛠️ Build Instructions

1. Ensure you are on a Linux, Unix, or macOS system.
2. Generate the executable `vm` by compiling it using:
    ```bash
    make
    ```
   
## ⚙️ Execution

Run the compiled executable with the input file:
- For First-in First-Out Replacement (fifo):
  ```bash
  ./vm addresses.txt fifo
  ```
- For Least Recently Used (lru):
  ```bash
  ./vm addresses.txt lru
  ```

## 📥 Example Input

For example, given an input file named `addresses.txt` with the following content:

    16916
    62493
    30198
    53683
    40185
    28781
    24462
    48399
    64815
    18295
    12218
    22760
    57982
    27966
    54894
    38929
    32865
    64243
    2315
    
## 📤 Output File

The output for the provided input should be named as `correct.txt` and have the following format:

    Virtual address: 16916 TLB: 0 Physical address: 20 Value: 0
    Virtual address: 62493 TLB: 1 Physical address: 285 Value: 0
    Virtual address: 30198 TLB: 2 Physical address: 758 Value: 29
    Virtual address: 53683 TLB: 3 Physical address: 947 Value: 108
    Virtual address: 40185 TLB: 4 Physical address: 1273 Value: 0
    Virtual address: 28781 TLB: 5 Physical address: 1389 Value: 0
    Virtual address: 24462 TLB: 6 Physical address: 1678 Value: 23
    Virtual address: 48399 TLB: 7 Physical address: 1807 Value: 67
    Virtual address: 64815 TLB: 8 Physical address: 2095 Value: 75
    Virtual address: 18295 TLB: 9 Physical address: 2423 Value: -35
    Virtual address: 12218 TLB: 10 Physical address: 2746 Value: 11
    Virtual address: 22760 TLB: 11 Physical address: 3048 Value: 0
    Virtual address: 57982 TLB: 12 Physical address: 3198 Value: 56
    Virtual address: 27966 TLB: 13 Physical address: 3390 Value: 27
    Virtual address: 54894 TLB: 14 Physical address: 3694 Value: 53
    Virtual address: 38929 TLB: 15 Physical address: 3857 Value: 0
    Virtual address: 32865 TLB: 0 Physical address: 4193 Value: 0
    Virtual address: 64243 TLB: 1 Physical address: 4595 Value: -68
    Virtual address: 2315 TLB: 2 Physical address: 4619 Value: 66
    Number of Translated Addresses = 19
    Page Faults = 19
    Page Fault Rate = 1.000
    TLB Hits = 0
    TLB Hit Rate = 0.000

### 🧪 Test Instructions

- First, ensure the file has execution permissions. You can do this with the `chmod` command:
   ```bash
   chmod +x test.sh
   ```
- Then you need to generate the executable by running the command:
  ```bash
  make
  ```
- After that, you can run the script with the command:
  - If you want to run all tests:
     ```bash
     ./run.sh all
     ```
  - If you want to run a specific test:
    - `./run.sh [number_of_test]`
        ```bash
         ./run.sh 5
         ```
- If there are differences in your result, it will print these errors in the `diff` folder with the name of the file that had the difference. The line starting with < is the received result, and the line starting with > is the expected result.
- The folder should be organized as follows:
   ```
    .
    ├── Makefile
    ├── vm.c
    ├── BACKING_STORE.bin
    ├── run.sh
    ├── test
    │   ├── 1
    │   │   ├── addresses_1.txt
    │   │   ├── addresses_1_fifo.txt
    │   │   ├── addresses_1_lru.txt
    │   ├── N
    │   │   ├── addresses_N.txt
    │   │   ├── addresses_N_fifo.txt
    │   │   ├── addresses_N_lru.txt
    ```
