#include "Benchmark.h"
#include "R-Tree.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace std::chrono;
namespace fs = std::filesystem;

// Gia su cac bien nay duoc khai bao extern tu file chua Memory Tracker
extern long long currentMemory;
extern long long peakMemory;

void Benchmark::runFullSuite(const string& directoryPath, const string& reportPath)
{
    vector<TestResult> results;

    if(!fs::exists(directoryPath))
    {
        cerr << "Error: Directory " << directoryPath << " not found!" << endl;
        return;
    }

    for(const auto& entry : fs::directory_iterator(directoryPath))
    {
        if(entry.path().extension()==".txt")
        {
            string inputPath = entry.path().string();
            ifstream inFile(inputPath);
            if(!inFile.is_open()) continue;

            int n;
            inFile >> n;
            vector<Point> points(n);
            for(int i = 0; i < n; ++i)
            {
                inFile >> points[i].x >> points[i].y;
            }
            inFile.close();

            // Do bo nho va thoi gian xay dung cay
            long long memBefore = currentMemory;
            auto startInsert = high_resolution_clock::now();

            RTree tree(16);
            for(int i = 0; i < n; ++i)
            {
                tree.insert(points[i]);
            }

            auto endInsert = high_resolution_clock::now();
            long long memAfter = currentMemory;

            // Do thoi gian Query (Range Query)
            Rect queryBox = {260.0, 290.0, 350.0, 350.0};
            int numQueries = 1000; // So luong query de lay trung binh

            auto startRTree = high_resolution_clock::now();
            for(int i = 0; i < numQueries; ++i)
            {
                volatile auto res = tree.rangeQuery(queryBox);
            }
            auto endRTree = high_resolution_clock::now();

            // Luu ket qua
            TestResult res;
            res.filename = entry.path().filename().string();
            res.n = n;
            res.buildTimeMs = duration_cast<milliseconds>(endInsert-startInsert).count();
            res.rtreeQueryUs = duration_cast<microseconds>(endRTree-startRTree).count() / numQueries;
            res.memoryMb = (double)(memAfter-memBefore) / 1024.0 / 1024.0;

            results.push_back(res);
            cout << "Finished benchmarking: " << res.filename << endl;
        }
    }
}
