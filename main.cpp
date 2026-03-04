#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <string>
#include "R-Tree.h"

using namespace std;
using namespace std::chrono;

long long currentMemory = 0;
long long peakMemory = 0;

void* operator new(size_t size)
{
    currentMemory += size;
    if (currentMemory > peakMemory)
    {
        peakMemory = currentMemory;
    }
    return malloc(size);
}

void operator delete(void* memory, size_t size) noexcept
{
    currentMemory -= size;
    free(memory);
}

void operator delete(void* memory) noexcept
{
    free(memory);
}

vector<Point> bruteForceSearch(const vector<Point>& points, const Rect& r)
{
    vector<Point> res;
    for (const auto& p : points)
    {
        if (r.covers(p))
        {
            res.push_back(p);
        }
    }
    return res;
}

int main()
{
    string numStr = "100000";
    string inputPath = "datasets/dataset_" + numStr + ".txt";

    // Sử dụng ifstream để đọc file
    ifstream inFile(inputPath);
    if (!inFile.is_open())
    {
        cerr << "Loi: Khong tim thay file " << inputPath << endl;
        return 1;
    }

    // Sử dụng ofstream để ghi file
    ofstream outFile("answer.txt");
    if (!outFile.is_open())
    {
        cerr << "Loi: Khong the tao file answer.txt" << endl;
        return 1;
    }

    int n;
    if (!(inFile >> n))
    {
        return 1;
    }

    vector<Point> points(n);
    for (int i = 0; i < n; ++i)
    {
        inFile >> points[i].x >> points[i].y;
    }
    inFile.close(); // Đóng file sau khi đọc xong

    long long memBefore = currentMemory;
    RTree tree(16);

    auto startInsert = high_resolution_clock::now();
    for (int i = 0; i < n; ++i)
    {
        tree.insert(points[i]);
    }
    auto endInsert = high_resolution_clock::now();

    long long memAfter = currentMemory;
    auto durationInsert = duration_cast<milliseconds>(endInsert - startInsert);

    int numQueries = 30000;
    Rect queryBox = {260.0, 290.0, 350.0, 350.0};

    auto startBrute = high_resolution_clock::now();
    for(int i = 0; i < numQueries; ++i)
    {
        vector<Point> res = bruteForceSearch(points, queryBox);
    }
    auto endBrute = high_resolution_clock::now();
    auto durationBrute = duration_cast<milliseconds>(endBrute - startBrute);

    auto startRTree = high_resolution_clock::now();
    for(int i = 0; i < numQueries; ++i)
    {
        vector<Point> res = tree.rangeQuery(queryBox);
    }
    auto endRTree = high_resolution_clock::now();
    auto durationRTree = duration_cast<microseconds>(endRTree - startRTree);

    double bruteForceMemoryMB = (double)(n * sizeof(Point)) / 1024.0 / 1024.0;
    double rTreeMemoryMB = (double)(memAfter - memBefore) / 1024.0 / 1024.0;

    // Ghi kết quả ra file bằng outFile thay vì cout
    outFile << "=========================================" << endl;
    outFile << "   SO SANH HIEU NANG TRUY VAN (" << numQueries << " LAN)" << endl;
    outFile << "=========================================" << endl;
    outFile << "So luong diem: " << n << endl;
    outFile << "Thoi gian xay dung cay (Insert): " << durationInsert.count() << " ms" << endl;
    outFile << "-----------------------------------------" << endl;
    outFile << "BO NHO TIEU THU (RAM):" << endl;
    outFile << fixed << setprecision(2);
    outFile << " - Duyet trau (Brute Force): " << bruteForceMemoryMB << " MB" << endl;
    outFile << " - R-Tree: " << rTreeMemoryMB << " MB" << endl;
    outFile << "-----------------------------------------" << endl;
    outFile << "TONG THOI GIAN " << numQueries << " LUOT SEARCH:" << endl;
    outFile << " - Duyet trau (Brute Force): " << durationBrute.count() << " ms" << endl;
    outFile << " - R-Tree: " << durationRTree.count() / 1000.0 << " ms (" << durationRTree.count() << " us)" << endl;
    outFile << "-----------------------------------------" << endl;

    if (durationRTree.count() > 0)
    {
        double speedup = (double)(durationBrute.count() * 1000.0) / durationRTree.count();
        outFile << "=> R-Tree nhanh hon Duyet trau x" << speedup << " lan" << endl;
    }
    outFile << "=========================================" << endl;

    outFile.close(); // Đóng file sau khi ghi xong

    // Báo ra màn hình console để biết chương trình đã chạy xong
    cout << "Da chay xong! Ket qua do dac da duoc luu vao file answer.txt" << endl;

    return 0;
}
