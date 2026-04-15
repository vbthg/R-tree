# 🌳 C++ 2D R-Tree Implementation

## 📖 Introduction

Welcome to the **C++ 2D R-Tree** repository. This is an open-source project implementing a fundamental R-Tree data structure within a two-dimensional (2D) coordinate system.

This project is specifically designed for **educational and illustrative purposes**, serving as a comprehensive reference for students, researchers, and developers working on Data Structure assignments. The source code is entirely self-contained with **zero third-party dependencies**, written in standard C++, and features extremely detailed line-by-line documentation (originally in Vietnamese) to ensure the internal logic is accessible to everyone.

### Key Features
- **Insert:** Supports adding 2D points into the spatial index, automatically expanding Minimum Bounding Rectangles (MBR) and handling node overflows using a perimeter-optimized **Node Splitting** algorithm.
- **Search / Range Query (Window Query):** Enables high-speed retrieval of all points located within a rectangular query window, utilizing a **Pruning** mechanism to skip irrelevant subtrees.
- **Delete:** Supports point removal with dynamic structural integrity management, handling underflow via the standard **Re-insertion** strategy.
- **Performance Benchmarking:** Includes a built-in comparison suite to evaluate speed and memory against the Naive (Brute Force) approach.

---

## 🧠 About R-Trees

An **R-Tree** (Rectangle-Tree) is a balanced, height-integrated data structure similar to a B-Tree but specialized for indexing multi-dimensional spatial data, such as geographic coordinates, polygons, or UI components.

### Core Concepts:
1. **MBR (Minimum Bounding Rectangle):** Instead of processing complex shapes directly, the R-Tree wraps every object in the smallest axis-parallel rectangle possible. All geometric operations are performed on these MBRs to maximize computational efficiency.
2. **Leaf Node:** Stores the MBRs representing the actual data points (in this repository, 2D Points).
3. **Internal Node:** Stores MBRs that encompass the entire spatial extent of their children. This hierarchical structure allows the R-Tree to group nearby points into progressively larger spatial regions.

### Operational Mechanisms:
- **Search (Pruning):** Starting from the root, the system checks if the query window intersects the MBR of child branches. If there is no intersection, the entire branch is skipped (**pruning**), reducing search complexity from $O(N)$ to an average of $O(\log N)$.
- **Insert:** The system traverses the tree to find the branch that requires the **minimum perimeter/area expansion** to accommodate the new data.
- **Split:** If a node exceeds its capacity $B$, it is split into two. The algorithm finds a split that minimizes the total perimeter sum of the two resulting MBRs to maintain tree quality.

---

## 💻 Code Structure

The repository is organized into modular files following C++ best practices:

- `RTree.h`: Contains all `struct` declarations (`Point`, `Rect`, `Node`) and the public `RTree` class API.
- `RTree.cpp`: The core implementation of the R-Tree. Algorithms such as `ChooseSubtree`, `SplitNode`, `Overflow/Underflow` handling, and tree condensation are implemented here with detailed comments.
- `main.cpp`: The test and demo suite. It provides examples of tree initialization, data insertion, and performance benchmarking.

---

## 🚀 Installation & Usage

### 1. Prerequisites
- A C++ compiler supporting **C++11** or later (GCC/MinGW, Clang, or MSVC).

### 2. Compilation

If you are using a **Terminal / Command Prompt**, navigate to the source directory and run:

```bash
g++ main.cpp RTree.cpp -o RTreeApp -std=c++11
```

Then run the executable:
- **Windows:** `RTreeApp.exe`
- **Linux / macOS:** `./RTreeApp`

If you are using an IDE like **Visual Studio** or **Code::Blocks**:
1. Create a new "Console Application" project.
2. Add `RTree.h`, `RTree.cpp`, and `main.cpp` to the project.
3. Build and Run (usually F5 or F9).

---

## 📂 Benchmarking and Performance Evaluation

This project integrates an automated performance evaluation system. The program reads coordinate datasets, executes **30,000 Window Queries**, and directly compares both **Execution Time** and **Memory Usage** between the R-Tree and the Brute Force method.

### 1. Directory Structure
Before running the benchmark, ensure your automatically generated datasets are placed in the `datasets/` folder:

```text
📦 Project_Folder
 ┣ 📂 datasets
 ┃ ┣ 📜 dataset_10.txt
 ┃ ┣ 📜 dataset_1000.txt
 ┃ ┣ 📜 dataset_10000.txt
 ┃ ┣ 📜 dataset_100000.txt
 ┃ ┗ 📜 dataset_1000000.txt
 ┣ 📜 RTree.h
 ┣ 📜 RTree.cpp
 ┗ 📜 main.cpp
```

### 2. Configuring Test Scale
By default, `main.cpp` is configured for `dataset_10.txt`. To evaluate larger scales, modify the `numStr` variable in the `main()` function:

```cpp
// Change "10" to the desired dataset scale 
// (e.g., "1000", "100000", "1000000")
string numStr = "1000000"; 
string inputPath = "datasets/dataset_" + numStr + ".txt";
```

### 3. Execution
Compile and link both files:
```bash
g++ main.cpp RTree.cpp -o benchmark -std=c++11
./benchmark
```

### 4. Reading Results
The program suppresses console output for speed and writes all analysis to `answer.txt`. Open this file to view:
- **Build Time (Insert):** The initial overhead for constructing the spatial index.
- **Memory Usage (RAM):** Precise measurement of R-Tree's footprint vs. a linear array (tracked via `new/delete` overrides).
- **Search Time:** Total time for 30,000 queries.
- **Speedup Factor:** How many times faster the R-Tree is compared to Brute Force at the current scale.

---

## 📊 Time & Space Complexity

Let $N$ be the total number of points stored:
- **Space Complexity:** $O(N)$
- **Search / Range Query:** Average $O(\log N + K)$ (where $K$ is the number of results). Worst case is $O(N)$ if MBRs overlap excessively.
- **Insert:** Average $O(\log N)$.
- **Delete:** Average $O(\log N)$.

---

## 🛠 Production-Ready Implementation

While this project is optimized for learning, professional C++ environments often use industry-standard libraries. For production-ready R-Trees, we recommend exploring:
- **[Boost.Geometry](https://www.boost.org/doc/libs/release/libs/geometry/doc/html/index.html):** Features a high-performance `bgi::rtree` with R*-Tree support.
- **libspatialindex:** A robust C++ library for complex spatial indexing tasks.

---
*This project serves as a reference for Computer Science students exploring Spatial Data Structures and Algorithm optimization.*
