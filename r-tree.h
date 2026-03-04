// RTree.h

#ifndef RTREE_H
#define RTREE_H

#include <vector>

// Cấu trúc đại diện cho một điểm 2D trên mặt phẳng
struct Point
{
    double x, y;

    // Toán tử so sánh xem hai điểm có tọa độ trùng nhau hay không
    bool operator==(const Point& other) const;
};

// Cấu trúc đại diện cho Hình chữ nhật bao nhỏ nhất (MBR - Minimum Bounding Rectangle)
struct Rect
{
    double minX, minY, maxX, maxY;

    // Tính chu vi của hình chữ nhật bao này (dùng để tối ưu R-Tree)
    double perimeter() const;

    // Kiểm tra xem hình chữ nhật này có giao cắt (đè lên nhau) với hình chữ nhật r khác không
    bool intersects(const Rect& r) const;

    // Kiểm tra xem hình chữ nhật này có bao phủ hoàn toàn điểm p hay không
    bool covers(const Point& p) const;

    // Phóng to hình chữ nhật để nó bao trọn thêm điểm p
    void expand(const Point& p);

    // Phóng to hình chữ nhật để nó bao trọn thêm một hình chữ nhật r khác
    void expand(const Rect& r);
};

// Cấu trúc Nút (Node) của cây R-Tree
struct Node
{
    bool isLeaf;                     // Cờ đánh dấu: true nếu là nút lá, false nếu là nút trong
    std::vector<Point> points;       // Chứa các điểm dữ liệu (chỉ có ý nghĩa khi isLeaf = true)
    std::vector<Node*> children;     // Chứa con trỏ tới các nút con (chỉ có ý nghĩa khi isLeaf = false)
    std::vector<Rect> childMBRs;     // Chứa các hộp MBR tương ứng bao bọc các nút con ở trên
    Node* parent;                    // Con trỏ trỏ ngược lên nút cha (dùng khi xử lý tràn/thiếu hụt dữ liệu)

    // Hàm khởi tạo nút, xác định ngay từ đầu nó là lá hay nút trong
    Node(bool leaf);

    // Tính toán lại MBR bao trọn toàn bộ dữ liệu (nếu là lá) hoặc toàn bộ con (nếu là nút trong)
    Rect getMBR() const;
};

// Lớp chính quản lý toàn bộ cây R-Tree
class RTree
{
private:
    Node* root;                      // Con trỏ trỏ tới gốc của cây
    int B;                           // Sức chứa tối đa (Capacity) của một nút
    int minCap;                      // Sức chứa tối thiểu (thường = 0.4 * B) để không bị Underflow

    // Hàm đệ quy hỗ trợ thêm điểm p vào nhánh con bắt đầu từ nút u
    void insertInternal(Node* u, Point p);

    // Thuật toán chọn nhánh con phù hợp nhất để nhét dữ liệu vào (nhánh có MBR phình ra ít nhất)
    Node* chooseSubtree(Node* u, Point p);

    // Xử lý khi nút u bị quá tải (vượt quá B phần tử), tiến hành chẻ nút và đẩy lên cha
    void handleOverflow(Node* u);

    // Hàm chuyên dụng để chẻ một nút lá thành 2 nút lá mới
    void splitLeaf(Node* u, Node*& newNode);

    // Hàm chuyên dụng để chẻ một nút trong thành 2 nút trong mới
    void splitInternal(Node* u, Node*& newNode);

    // Hàm đệ quy hỗ trợ truy vấn tìm các điểm nằm trong vùng r
    void rangeQueryInternal(Node* u, const Rect& r, std::vector<Point>& result) const;

    // Hàm đệ quy đi tìm nút lá đang chứa điểm p (phục vụ cho thao tác xóa)
    Node* findLeaf(Node* u, Point p);

    // Thuật toán xử lý khi một nút bị thiếu hụt dữ liệu (dưới minCap), gỡ nút đó ra khỏi cây
    void condenseTree(Node* u, std::vector<Node*>& eliminatedNodes);

    // Rút toàn bộ các điểm dữ liệu bên trong một nút (và các con của nó) ra ngoài
    void extractAllPoints(Node* u, std::vector<Point>& extracted);

public:
    // Khởi tạo cây với sức chứa tối đa quy định trước
    RTree(int capacity);

    // API chính: Thêm một điểm mới vào hệ thống
    void insert(Point p);

    // API chính: Xóa một điểm khỏi hệ thống, trả về true nếu xóa thành công
    bool remove(Point p);

    // API chính: Truy vấn tìm mọi điểm rơi vào trong hình chữ nhật r
    std::vector<Point> rangeQuery(const Rect& r) const;
};

#endif
