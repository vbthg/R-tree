#include <iostream>
#include "R-tree.h"

int main()
{
    // Khởi tạo cây với B = 4 (Max capacity)
    RTree tree(4);

    // Chèn một số điểm test nghiệm thu
    tree.insert({1.0, 5.0}); // a
    tree.insert({4.0, 4.0}); // b
    tree.insert({7.0, 6.0}); // c
    tree.insert({2.0, 3.0}); // d
    tree.insert({5.0, 3.0}); // e
    tree.insert({8.0, 2.0}); // f
    tree.insert({3.0, 1.0}); // g

    // Tạo khung truy vấn r (Window Query)
    Rect queryBox = {1.5, 0.5, 6.0, 4.0};

    // Thực hiện truy vấn
    std::vector<Point> results = tree.rangeQuery(queryBox);

    std::cout << "So luong diem nam trong vung truy van: " << results.size() << "\n";
    for (const auto& p : results)
    {
        std::cout << "Diem (" << p.x << ", " << p.y << ")\n";
    }

    return 0;
}
