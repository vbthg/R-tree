// RTree.cpp

#include "../header/r-tree.h"
#include <algorithm>
#include <cmath>

// -------------------------------------------------------------------------
// Cài đặt các hàm của Point
// -------------------------------------------------------------------------

bool Point::operator==(const Point& other) const
{
    // So sánh tọa độ x và y để xem 2 điểm có khớp nhau không
    return (x == other.x && y == other.y);
}

// -------------------------------------------------------------------------
// Cài đặt các hàm của Rect (MBR)
// -------------------------------------------------------------------------

double Rect::perimeter() const
{
    // Chu vi = 2 * (chiều dài + chiều rộng)
    return 2.0 * ((maxX - minX) + (maxY - minY));
}

bool Rect::intersects(const Rect& r) const
{
    // Trả về false nếu hình này nằm hoàn toàn bên trái, phải, trên, hoặc dưới hình kia
    return !(maxX < r.minX || minX > r.maxX || maxY < r.minY || minY > r.maxY);
}

bool Rect::covers(const Point& p) const
{
    // Kiểm tra xem tọa độ của điểm có nằm gọn giữa 4 biên của hình chữ nhật không
    return (p.x >= minX && p.x <= maxX && p.y >= minY && p.y <= maxY);
}

void Rect::expand(const Point& p)
{
    // Nới rộng các biên ra nếu điểm p nằm ngoài giới hạn hiện tại
    minX = std::min(minX, p.x);
    minY = std::min(minY, p.y);
    maxX = std::max(maxX, p.x);
    maxY = std::max(maxY, p.y);
}

void Rect::expand(const Rect& r)
{
    // Nới rộng các biên để bao phủ trọn vẹn một hình chữ nhật r khác
    minX = std::min(minX, r.minX);
    minY = std::min(minY, r.minY);
    maxX = std::max(maxX, r.maxX);
    maxY = std::max(maxY, r.maxY);
}

// -------------------------------------------------------------------------
// Cài đặt các hàm của Node
// -------------------------------------------------------------------------

Node::Node(bool leaf)
{
    isLeaf = leaf;
    parent = nullptr;
}

Rect Node::getMBR() const
{
    if (isLeaf)
    {
        // Nếu nút lá không có dữ liệu, trả về MBR rỗng ở tọa độ 0
        if (points.empty())
        {
            return {0, 0, 0, 0};
        }

        // Khởi tạo MBR ban đầu vừa bằng điểm đầu tiên
        Rect r = {points[0].x, points[0].y, points[0].x, points[0].y};

        // Mở rộng MBR dần dần cho các điểm còn lại
        for (const auto& p : points)
        {
            r.expand(p);
        }
        return r;
    }
    else
    {
        // Nếu nút trong không có con, trả về MBR rỗng
        if (childMBRs.empty())
        {
            return {0, 0, 0, 0};
        }

        // Bắt đầu từ MBR của nút con đầu tiên
        Rect r = childMBRs[0];

        // Mở rộng MBR cho các con còn lại
        for (const auto& mbr : childMBRs)
        {
            r.expand(mbr);
        }
        return r;
    }
}

// -------------------------------------------------------------------------
// Cài đặt các hàm của RTree (Lõi Thuật Toán)
// -------------------------------------------------------------------------

RTree::RTree(int capacity)
{
    B = capacity;
    minCap = std::ceil(0.4 * B); // Mức tối thiểu để không bị coi là thiếu hụt
    root = new Node(true);       // Ban đầu cây chỉ có 1 nút gốc và là nút lá
}

void RTree::insert(Point p)
{
    // Gọi hàm đệ quy bắt đầu từ gốc
    insertInternal(root, p);
}

void RTree::insertInternal(Node* u, Point p)
{
    if (u->isLeaf)
    {
        // 1. Nếu chạm đến nút lá, trực tiếp nhét điểm vào đây
        u->points.push_back(p);

        if (u->points.size() > B)
        {
            // 2. Nếu sau khi nhét mà sức chứa vượt quá B, tiến hành chẻ nút
            handleOverflow(u);
        }
        else
        {
            // 3. Nếu không bị tràn, phải chạy ngược lên trên để cập nhật lại MBR của các cha
            Node* curr = u;
            while (curr->parent != nullptr)
            {
                Node* pNode = curr->parent;
                for (size_t i = 0; i < pNode->children.size(); ++i)
                {
                    if (pNode->children[i] == curr)
                    {
                        pNode->childMBRs[i] = curr->getMBR();
                        break;
                    }
                }
                curr = pNode;
            }
        }
    }
    else
    {
        // 4. Nếu u là nút trong, dùng thuật toán để tìm xem nên đi xuống nhánh con nào
        Node* v = chooseSubtree(u, p);

        // 5. Đệ quy đi xuống nhánh con đã chọn
        insertInternal(v, p);
    }
}

Node* RTree::chooseSubtree(Node* u, Point p)
{
    double minIncrease = 1e9;
    double minPerimeter = 1e9;
    int bestIndex = 0;

    // Duyệt qua tất cả các con của u để đánh giá
    for (size_t i = 0; i < u->childMBRs.size(); ++i)
    {
        Rect currMBR = u->childMBRs[i];
        double oldPerimeter = currMBR.perimeter();

        // Tính thử xem nếu nhét điểm p vào nhánh này thì MBR phình ra bao nhiêu
        currMBR.expand(p);
        double newPerimeter = currMBR.perimeter();
        double increase = newPerimeter - oldPerimeter;

        // Ưu tiên chọn nhánh có độ phình chu vi ít nhất
        // Nếu độ phình bằng nhau, chọn nhánh có chu vi gốc nhỏ hơn
        if (increase < minIncrease || (increase == minIncrease && oldPerimeter < minPerimeter))
        {
            minIncrease = increase;
            minPerimeter = oldPerimeter;
            bestIndex = i;
        }
    }

    // Trả về nút con tối ưu nhất
    return u->children[bestIndex];
}

void RTree::handleOverflow(Node* u)
{
    Node* uPrime = nullptr;

    // 1. Chẻ nút bị quá tải làm đôi, phần mới tách ra được gán vào uPrime
    if (u->isLeaf)
    {
        splitLeaf(u, uPrime);
    }
    else
    {
        splitInternal(u, uPrime);
    }

    if (u == root)
    {
        // 2. Nếu nút vừa bị chẻ là nút gốc, cây sẽ tăng thêm 1 tầng
        Node* newRoot = new Node(false); // Gốc mới phải là nút trong

        // Móc nút gốc cũ vào làm con thứ nhất
        newRoot->children.push_back(u);
        newRoot->childMBRs.push_back(u->getMBR());
        u->parent = newRoot;

        // Móc nửa mới tách (uPrime) vào làm con thứ hai
        newRoot->children.push_back(uPrime);
        newRoot->childMBRs.push_back(uPrime->getMBR());
        uPrime->parent = newRoot;

        root = newRoot;
    }
    else
    {
        // 3. Nếu không phải gốc, ta báo cáo sự thay đổi lên nút cha
        Node* w = u->parent;

        // Cập nhật lại MBR của u trong danh sách của cha (vì u vừa bị cắt mất một nửa dữ liệu)
        for (size_t i = 0; i < w->children.size(); ++i)
        {
            if (w->children[i] == u)
            {
                w->childMBRs[i] = u->getMBR();
                break;
            }
        }

        // Nhét thêm nửa mới (uPrime) vào làm một nhánh con mới của cha
        w->children.push_back(uPrime);
        w->childMBRs.push_back(uPrime->getMBR());
        uPrime->parent = w;

        // 4. Vì cha vừa nhận thêm 1 nhánh con, cha có thể lại bị tràn, nếu thế đệ quy chẻ tiếp cha
        if (w->children.size() > B)
        {
            handleOverflow(w);
        }
    }
}

void RTree::splitLeaf(Node* u, Node*& uPrime)
{
    int m = u->points.size();

    double bestPerimeterSum = 1e9;
    int bestDim = 0; // 0 là cắt dọc trục X, 1 là cắt dọc trục Y
    int bestI = 0;   // Chỉ số cắt tốt nhất

    std::vector<Point> originalPoints = u->points;

    // ----- THỬ CẮT THEO TRỤC X -----
    // Sắp xếp các điểm theo tọa độ x tăng dần
    std::sort(u->points.begin(), u->points.end(), [](Point a, Point b) { return a.x < b.x; });

    // Thử đặt nhát cắt từ vị trí minCap đến (m - minCap) để đảm bảo không ai bị thiếu hụt
    for (int i = minCap; i <= m - minCap; ++i)
    {
        Rect r1 = {u->points[0].x, u->points[0].y, u->points[0].x, u->points[0].y};
        for (int j = 1; j < i; ++j) r1.expand(u->points[j]);

        Rect r2 = {u->points[i].x, u->points[i].y, u->points[i].x, u->points[i].y};
        for (int j = i + 1; j < m; ++j) r2.expand(u->points[j]);

        // Tính tổng chu vi 2 khối hộp, tìm cách cắt nhỏ nhất
        double periSum = r1.perimeter() + r2.perimeter();
        if (periSum < bestPerimeterSum)
        {
            bestPerimeterSum = periSum;
            bestDim = 0;
            bestI = i;
        }
    }

    // ----- THỬ CẮT THEO TRỤC Y -----
    // Sắp xếp các điểm theo tọa độ y tăng dần
    std::sort(u->points.begin(), u->points.end(), [](Point a, Point b) { return a.y < b.y; });

    for (int i = minCap; i <= m - minCap; ++i)
    {
        Rect r1 = {u->points[0].x, u->points[0].y, u->points[0].x, u->points[0].y};
        for (int j = 1; j < i; ++j) r1.expand(u->points[j]);

        Rect r2 = {u->points[i].x, u->points[i].y, u->points[i].x, u->points[i].y};
        for (int j = i + 1; j < m; ++j) r2.expand(u->points[j]);

        double periSum = r1.perimeter() + r2.perimeter();
        if (periSum < bestPerimeterSum)
        {
            bestPerimeterSum = periSum;
            bestDim = 1;
            bestI = i;
        }
    }

    // ----- THỰC HIỆN CẮT THẬT -----
    // Sắp xếp lại danh sách gốc theo đúng trục đã chọn là tối ưu nhất
    if (bestDim == 0)
    {
        std::sort(originalPoints.begin(), originalPoints.end(), [](Point a, Point b) { return a.x < b.x; });
    }
    else
    {
        std::sort(originalPoints.begin(), originalPoints.end(), [](Point a, Point b) { return a.y < b.y; });
    }

    // Gán nửa đầu (từ 0 đến bestI) giữ lại ở nút cũ u
    u->points.assign(originalPoints.begin(), originalPoints.begin() + bestI);

    // Đẩy nửa sau (từ bestI đến cuối) sang nút mới uPrime
    uPrime = new Node(true);
    uPrime->points.assign(originalPoints.begin() + bestI, originalPoints.end());
}

void RTree::splitInternal(Node* u, Node*& uPrime)
{
    // Để sắp xếp các nhánh con dễ dàng, ta gom tạm con trỏ và MBR của nó vào một struct
    struct Entry
    {
        Node* child;
        Rect mbr;
    };

    int m = u->children.size();
    std::vector<Entry> entries(m);
    for (int i = 0; i < m; ++i)
    {
        entries[i] = {u->children[i], u->childMBRs[i]};
    }

    double bestPerimeterSum = 1e9;
    int bestSortType = 0; // 0: theo biên trái X, 1: biên phải X, 2: biên dưới Y, 3: biên trên Y
    int bestI = 0;

    // Lambda (hàm ẩn) để duyệt thử mọi nhát cắt
    auto evaluateSplit = [&](int sortType)
    {
        for (int i = minCap; i <= m - minCap; ++i)
        {
            Rect r1 = entries[0].mbr;
            for (int j = 1; j < i; ++j) r1.expand(entries[j].mbr);

            Rect r2 = entries[i].mbr;
            for (int j = i + 1; j < m; ++j) r2.expand(entries[j].mbr);

            double periSum = r1.perimeter() + r2.perimeter();
            if (periSum < bestPerimeterSum)
            {
                bestPerimeterSum = periSum;
                bestSortType = sortType;
                bestI = i;
            }
        }
    };

    // Thử sắp xếp và cắt theo 4 loại biên khác nhau
    std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) { return a.mbr.minX < b.mbr.minX; });
    evaluateSplit(0);

    std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) { return a.mbr.maxX < b.mbr.maxX; });
    evaluateSplit(1);

    std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) { return a.mbr.minY < b.mbr.minY; });
    evaluateSplit(2);

    std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) { return a.mbr.maxY < b.mbr.maxY; });
    evaluateSplit(3);

    // Sắp xếp lại danh sách theo kiểu tốt nhất
    if (bestSortType == 0)
    {
        std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) { return a.mbr.minX < b.mbr.minX; });
    }
    else if (bestSortType == 1)
    {
        std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) { return a.mbr.maxX < b.mbr.maxX; });
    }
    else if (bestSortType == 2)
    {
        std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) { return a.mbr.minY < b.mbr.minY; });
    }
    else
    {
        std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) { return a.mbr.maxY < b.mbr.maxY; });
    }

    // Xóa sạch u cũ để nạp lại đúng nửa phần đầu
    u->children.clear();
    u->childMBRs.clear();
    for (int j = 0; j < bestI; ++j)
    {
        u->children.push_back(entries[j].child);
        u->childMBRs.push_back(entries[j].mbr);
    }

    // Gán nửa phần sau cho uPrime mới tạo
    uPrime = new Node(false);
    for (int j = bestI; j < m; ++j)
    {
        uPrime->children.push_back(entries[j].child);
        uPrime->childMBRs.push_back(entries[j].mbr);
        entries[j].child->parent = uPrime; // Rất quan trọng: nhận diện cha mới
    }
}

// -------------------------------------------------------------------------
// Nhóm hàm cho thao tác XÓA (Delete)
// -------------------------------------------------------------------------

Node* RTree::findLeaf(Node* u, Point p)
{
    if (u->isLeaf)
    {
        // Quét cạn trong nút lá xem có điểm p không
        for (const Point& pt : u->points)
        {
            if (pt == p)
            {
                return u;
            }
        }
        return nullptr;
    }

    // Nếu là nút trong, đi xuống TẤT CẢ các nhánh con có MBR bao phủ p
    for (size_t i = 0; i < u->children.size(); ++i)
    {
        if (u->childMBRs[i].covers(p))
        {
            Node* result = findLeaf(u->children[i], p);
            if (result != nullptr)
            {
                return result; // Tìm thấy thì dừng lại và trả về ngay
            }
        }
    }
    return nullptr;
}

void RTree::condenseTree(Node* u, std::vector<Node*>& eliminatedNodes)
{
    Node* curr = u;

    // Leo dần từ lá lên đến gốc
    while (curr != root)
    {
        Node* pNode = curr->parent;

        // Đếm số phần tử hiện tại (dùng list điểm nếu là lá, list con nếu là nút trong)
        int currentSize = curr->isLeaf ? curr->points.size() : curr->children.size();

        // Xử lý Underflow: Nếu số lượng tụt xuống dưới mức cho phép
        if (currentSize < minCap)
        {
            // Tìm và gỡ nút này ra khỏi danh sách con của cha nó
            for (size_t i = 0; i < pNode->children.size(); ++i)
            {
                if (pNode->children[i] == curr)
                {
                    pNode->children.erase(pNode->children.begin() + i);
                    pNode->childMBRs.erase(pNode->childMBRs.begin() + i);
                    break;
                }
            }
            // Không xóa ngay, mà bỏ vào danh sách chờ để tái sinh (chèn lại)
            eliminatedNodes.push_back(curr);
        }
        else
        {
            // Nếu không bị thiếu hụt, ta chỉ cần bóp nhỏ lại MBR của nó trên cha
            for (size_t i = 0; i < pNode->children.size(); ++i)
            {
                if (pNode->children[i] == curr)
                {
                    pNode->childMBRs[i] = curr->getMBR();
                    break;
                }
            }
        }
        curr = pNode; // Tiếp tục leo lên cha
    }
}

void RTree::extractAllPoints(Node* u, std::vector<Point>& extracted)
{
    if (u->isLeaf)
    {
        // Rút toàn bộ điểm ra
        for (const Point& p : u->points)
        {
            extracted.push_back(p);
        }
    }
    else
    {
        // Đệ quy moi ruột từng đứa con
        for (Node* child : u->children)
        {
            extractAllPoints(child, extracted);
        }
    }
}

bool RTree::remove(Point p)
{
    // 1. Tìm nút lá chứa p
    Node* leaf = findLeaf(root, p);
    if (leaf == nullptr)
    {
        return false; // Điểm không tồn tại
    }

    // 2. Bắn bỏ điểm p khỏi nút lá
    for (size_t i = 0; i < leaf->points.size(); ++i)
    {
        if (leaf->points[i] == p)
        {
            leaf->points.erase(leaf->points.begin() + i);
            break;
        }
    }

    // 3. Xử lý dọn dẹp cây: Gỡ bỏ các nút bị thiếu hụt, bóp MBR các nút còn lại
    std::vector<Node*> eliminatedNodes;
    condenseTree(leaf, eliminatedNodes);

    // 4. Nếu gốc bị tụt xuống chỉ còn 1 con duy nhất, ta hạ bậc cây xuống
    if (!root->isLeaf && root->children.size() == 1)
    {
        Node* oldRoot = root;
        root = root->children[0];
        root->parent = nullptr; // Lên chức làm gốc mới
        delete oldRoot;         // Giải phóng gốc cũ
    }

    // 5. Gom dữ liệu côi cút từ các nút bị gỡ bỏ, ném lại vào cây từ trên xuống
    for (Node* elimNode : eliminatedNodes)
    {
        std::vector<Point> orphanedPoints;
        extractAllPoints(elimNode, orphanedPoints);
        for (const Point& pt : orphanedPoints)
        {
            insert(pt); // Tái hòa nhập cộng đồng
        }
        delete elimNode; // Hủy xác nút đã lấy hết dữ liệu
    }

    return true;
}

// -------------------------------------------------------------------------
// Cài đặt thao tác Truy vấn (Search)
// -------------------------------------------------------------------------

std::vector<Point> RTree::rangeQuery(const Rect& r) const
{
    std::vector<Point> result;
    rangeQueryInternal(root, r, result);
    return result;
}

void RTree::rangeQueryInternal(Node* u, const Rect& r, std::vector<Point>& result) const
{
    if (u->isLeaf)
    {
        // Chạm tới lá: Soi từng điểm xem có nằm gọn trong hộp tìm kiếm r không
        for (const auto& p : u->points)
        {
            if (r.covers(p))
            {
                result.push_back(p);
            }
        }
    }
    else
    {
        // Nếu là nút trong: Chỉ đi xuống những nhánh con mà MBR của nó có dính dáng tới hộp r
        for (size_t i = 0; i < u->children.size(); ++i)
        {
            // Bước Lọc (Pruning) ma thuật của R-Tree ở đây
            if (u->childMBRs[i].intersects(r))
            {
                rangeQueryInternal(u->children[i], r, result);
            }
        }
    }
}
