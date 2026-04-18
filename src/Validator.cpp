#include "Validator.h"
#include <iostream>
#include <algorithm>
#include <chrono>

using namespace std;

vector<Point> Validator::bruteForceSearch(const vector<Point>& points, const Rect& queryBox)
{
    vector<Point> res;
    for(const auto& p : points)
    {
        if(queryBox.covers(p))
        {
            res.push_back(p);
        }
    }
    return res;
}

bool Validator::areResultsIdentical(vector<Point> res1, vector<Point> res2)
{
    if(res1.size()!=res2.size())
    {
        return false;
    }

    // Lambda so sanh de sap xep cac diem theo toa do x, y
    auto pointComp = [](const Point& a, const Point& b)
    {
        if(a.x!=b.x) return a.x < b.x;
        return a.y < b.y;
    };

    sort(res1.begin(), res1.end(), pointComp);
    sort(res2.begin(), res2.end(), pointComp);

    for(size_t i = 0; i < res1.size(); ++i)
    {
        // Kiem tra sai so nho neu toa do la kieu float/double
        if(res1[i].x!=res2[i].x||res1[i].y!=res2[i].y)
        {
            return false;
        }
    }
    return true;
}

void Validator::validate(const vector<Point>& points, RTree& tree, const Rect& queryBox)
{
    // Chi thuc hien kiem dinh neu dataset du nho (< 100,000 diem)
    // Hoac chi thuc hien dung 1 lan duy nhat de check logic
    if(points.size() > 100000)
    {
        cout << "[Validator] Dataset too large for full brute-force check. Running single sample check..." << endl;
    }

    auto startBrute = chrono::high_resolution_clock::now();
    vector<Point> bruteRes = bruteForceSearch(points, queryBox);
    auto endBrute = chrono::high_resolution_clock::now();

    vector<Point> rtreeRes = tree.rangeQuery(queryBox);

    if(areResultsIdentical(bruteRes, rtreeRes))
    {
        cout << "[OK] Integrity Check Passed. (Points found: " << rtreeRes.size() << ")" << endl;
        auto duration = chrono::duration_cast<chrono::milliseconds>(endBrute - startBrute).count();
        cout << "[Info] Brute force took: " << duration << " ms for this single query." << endl;
    }
    else
    {
        cerr << "[ERROR] Results Mismatch!" << endl;
    }
}
