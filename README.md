# 🌳 C++ 2D R-Tree Implementation

## 📖 Giới thiệu (Introduction)

Chào mừng bạn đến với kho lưu trữ (repository) **C++ 2D R-Tree**. Đây là một dự án mã nguồn mở triển khai cấu trúc dữ liệu R-Tree cơ bản trong không gian hai chiều (2D). 

Dự án này được thiết kế với mục tiêu **giáo dục và minh họa**, phục vụ cho việc học tập, nghiên cứu và làm đồ án môn học Cấu trúc Dữ liệu. Mã nguồn không phụ thuộc vào bất kỳ thư viện bên thứ ba nào, được viết hoàn toàn bằng C++ tiêu chuẩn và đi kèm với các chú thích tiếng Việt cực kỳ chi tiết từng dòng để bất kỳ ai cũng có thể đọc hiểu logic bên trong.

### Các tính năng chính (Features)
- **Insert (Thêm dữ liệu):** Hỗ trợ thêm điểm ảnh (Point) vào không gian 2D, tự động nới rộng các hộp giới hạn (MBR) và xử lý tràn nút (Overflow) bằng thuật toán chẻ nút (Node Splitting) tối ưu theo chu vi.
- **Search / Range Query (Truy vấn vùng):** Cho phép tìm kiếm tất cả các điểm nằm bên trong một khung hình chữ nhật truy vấn (Window Query) với tốc độ cao nhờ cơ chế cắt tỉa (Pruning).
- **Delete (Xóa dữ liệu):** Hỗ trợ tìm và xóa một điểm khỏi cây, đồng thời xử lý hiện tượng thiếu hụt dữ liệu (Underflow) bằng cách gỡ nút và tái chèn (Re-insertion).

---

## 🧠 Kiến thức cơ bản về R-Tree (About R-Tree)

**R-Tree** (Rectangle-Tree) là một cấu trúc dữ liệu dạng cây cân bằng (tương tự như B-Tree) nhưng được thiết kế đặc biệt để lập chỉ mục (indexing) các dữ liệu đa chiều trong không gian, chẳng hạn như tọa độ địa lý, hình đa giác, hoặc các đối tượng giao diện.

### Các khái niệm cốt lõi:
1. **MBR (Minimum Bounding Rectangle - Hộp bao nhỏ nhất):** Thay vì quản lý trực tiếp các hình khối phức tạp, R-Tree bao bọc mỗi đối tượng bằng một hình chữ nhật nhỏ nhất có cạnh song song với các trục tọa độ. Mọi phép toán hình học sẽ được thực hiện trên các MBR này để tối ưu tốc độ.
2. **Nút lá (Leaf Node):** Chứa các MBR đại diện cho dữ liệu thực tế (trong repo này là các Điểm 2D).
3. **Nút trong (Internal Node):** Chứa các MBR đại diện cho toàn bộ không gian của các nút con bên dưới nó. Nhờ cấu trúc phân cấp này, một R-Tree có thể gom nhóm các điểm gần nhau lại thành các vùng không gian ngày càng lớn.

### Cơ chế hoạt động:
- **Khi tìm kiếm (Search):** Bắt đầu từ gốc, hệ thống kiểm tra xem vùng truy vấn có giao cắt với MBR của các nhánh con hay không. Nếu không giao cắt, hệ thống sẽ bỏ qua toàn bộ nhánh đó (pruning), giúp giảm độ phức tạp từ $O(N)$ xuống trung bình $O(\log N)$.
- **Khi thêm mới (Insert):** Hệ thống tìm kiếm từ trên xuống dưới nhánh con nào cần "phình ra" ít nhất để chứa dữ liệu mới. Nguyên tắc vàng của R-Tree là **giảm thiểu sự gia tăng chu vi / diện tích** của các hộp MBR.
- **Khi chẻ nút (Split):** Nếu một nút chứa quá nhiều phần tử (vượt quá Capacity $B$), nó sẽ bị chẻ làm đôi. Thuật toán cố gắng tìm ra cách chia sao cho tổng chu vi của 2 MBR mới tạo ra là nhỏ nhất.

---

## 💻 Giới thiệu về Mã nguồn (Code Structure)

Repo này được tổ chức thành các file tách biệt theo chuẩn C++ để dễ dàng tích hợp vào dự án của bạn:

- `RTree.h`: Chứa toàn bộ các khai báo `struct` (Point, Rect, Node) và khai báo `class RTree`. File này mang đến cái nhìn tổng quan về kiến trúc dữ liệu và các API công khai.
- `RTree.cpp`: Nơi chứa phần "linh hồn" của R-Tree. Các thuật toán phức tạp như tìm nhánh (Choose Subtree), chẻ nút (Split), và xử lý tràn/thiếu hụt dữ liệu (Overflow/Underflow) được code tại đây. Mọi dòng code quan trọng đều được chú thích tiếng Việt rõ ràng.
- `main.cpp`: File chạy thử nghiệm, cung cấp một ví dụ mẫu (demo) về cách khởi tạo cây, chèn dữ liệu và thực hiện truy vấn.

---

## 🚀 Cách cài đặt và Sử dụng (Installation & Usage)

### 1. Yêu cầu hệ thống (Prerequisites)
- Trình biên dịch C++ (Hỗ trợ C++11 trở lên). Bạn có thể dùng GCC (MinGW trên Windows), Clang (macOS), hoặc MSVC (Visual Studio).

### 2. Biên dịch mã nguồn (Compilation)

Nếu bạn sử dụng **Terminal / Command Prompt**, hãy di chuyển tới thư mục chứa mã nguồn và chạy lệnh sau:

```bash
g++ main.cpp RTree.cpp -o RTreeApp
```

Sau đó chạy file thực thi:
- **Windows:** `RTreeApp.exe`
- **Linux / macOS:** `./RTreeApp`

Nếu bạn sử dụng các IDE như **Code::Blocks** hoặc **Visual Studio**:
1. Tạo một dự án (Project) dạng Console Application.
2. Thêm 3 file `RTree.h`, `RTree.cpp` và `main.cpp` vào dự án.
3. Bấm `Build and Run` (thường là phím F9 hoặc F5).

### 3. Ví dụ sử dụng (Quick Start)

Dưới đây là trích đoạn cách bạn có thể sử dụng lớp `RTree` trong project của mình:

```cpp
#include <iostream>
#include "RTree.h"

int main() {
    // 1. Khởi tạo cây R-Tree với sức chứa tối đa của mỗi nút là 4
    RTree tree(4);

    // 2. Chèn các điểm dữ liệu (tọa độ x, y)
    tree.insert({1.0, 5.0});
    tree.insert({4.0, 4.0});
    tree.insert({7.0, 6.0});
    
    // 3. Xóa một điểm
    tree.remove({4.0, 4.0});

    // 4. Tạo một khung truy vấn hình chữ nhật (minX, minY, maxX, maxY)
    Rect queryBox = {0.0, 0.0, 5.0, 6.0};
    
    // 5. Tìm tất cả các điểm nằm trong khung này
    std::vector<Point> results = tree.rangeQuery(queryBox);
    
    std::cout << "So luong diem tim thay: " << results.size() << "\n";
    for (const auto& p : results) {
        std::cout << "-> Diem (" << p.x << ", " << p.y << ")\n";
    }

    return 0;
}
```

---

## 📊 Độ phức tạp thuật toán (Time & Space Complexity)

Gọi $N$ là tổng số điểm đang lưu trữ trong R-Tree:
- **Không gian lưu trữ (Space Complexity):** $O(N)$
- **Truy vấn (Search / Range Query):** Trung bình $O(\log N + K)$ (với $K$ là số lượng kết quả thỏa mãn). Trường hợp xấu nhất (các MBR đè lên nhau quá nhiều) là $O(N)$.
- **Thêm mới (Insert):** Trung bình $O(\log N)$.
- **Xóa (Delete):** Trung bình $O(\log N)$.

---
*Dự án này là tài liệu tham khảo cho sinh viên ngành Khoa học Máy tính tìm hiểu về Cấu trúc dữ liệu không gian (Spatial Data Structures).*
