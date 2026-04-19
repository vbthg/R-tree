#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "r-tree.h"
#include "Benchmark.h"
#include "Validator.h"

using namespace std;

// Dinh nghia cac bien toan cuc de Benchmark.cpp co the truy cap qua extern
long long currentMemory = 0;
long long peakMemory = 0;

// Ghi de operator new de theo doi bo nho
void* operator new(size_t size)
{
    currentMemory += size;
    if(currentMemory > peakMemory)
    {
        peakMemory = currentMemory;
    }
    return malloc(size);
}

// Ghi de operator delete
void operator delete(void* memory, size_t size) noexcept
{
    currentMemory -= size;
    free(memory);
}

void operator delete(void* memory) noexcept
{
    free(memory);
}

int main()
{
    string datasetDir = "datasets/";
    string reportFile = "BENCHMARK_REPORT.md";

    cout << "=========================================" << endl;
    cout << "   R-TREE GROUP PROJECT - MAIN RUNNER    " << endl;
    cout << "=========================================" << endl;

    // BUOC 1: VALIDATION (Chi lam 1 lan voi dataset nho de chung minh code dung)
    cout << "Step 1: Running Integrity Validation..." << endl;

    string testFile = datasetDir + "dataset_1000.txt";
    ifstream inFile(testFile);
    if(inFile.is_open())
    {
        int n;
        inFile >> n;
        vector<Point> points(n);
        RTree tree(16);
        for(int i = 0; i < n; ++i)
        {
            inFile >> points[i].x >> points[i].y;
            tree.insert(points[i]);
        }
        inFile.close();

        Rect testBox = {200.0, 200.0, 400.0, 400.0};

        // Gọi trực tiếp từ class Validator
        Validator::validate(points, tree, testBox);
    }
    else
    {
        cout << "[Skip] Test file dataset_1000.txt not found." << endl;
    }

    cout << "-----------------------------------------" << endl;

    // 2. Buoc danh gia hieu nang (Benchmarking) - Cong viec cua Friend 1
    cout << "\nStep 2: Stress Testing & Benchmarking (Large Datasets)" << endl;
    Benchmark::runFullSuite(datasetDir, reportFile);

    cout << "-----------------------------------------" << endl;
    cout << "ALL TASKS COMPLETED!" << endl;
    cout << "Peak Memory Usage: " << (double)peakMemory / 1024.0 / 1024.0 << " MB" << endl;
    cout << "Detailed report saved to: " << reportFile << endl;
    cout << "=========================================" << endl;

    return 0;
}
