#pragma once
#include "r-Tree.h"
#include <vector>

class Validator
{
public:
    // Ham duyet trau de lam ket qua doi chieu (Reference)
    static std::vector<Point> bruteForceSearch(const std::vector<Point>& points, const Rect& queryBox);

    // Ham so sanh hai tap hop diem (khong quan tam thu tu)
    static bool areResultsIdentical(std::vector<Point> res1, std::vector<Point> res2);

    // Thuc hien kiem dinh va bao cao loi neu co
    static void validate(const std::vector<Point>& points, RTree& tree, const Rect& queryBox);
};
