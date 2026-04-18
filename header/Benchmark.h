#include <string>
#include <vector>

// Cau truc de luu tru ket qua cua tung dataset
struct TestResult
{
    std::string filename;
    int n;
    long long buildTimeMs;
    long long rtreeQueryUs;
    long long bruteQueryMs;
    double memoryMb;
};

class Benchmark
{
public:
    // Ham chay tat ca dataset trong thu muc va xuat bao cao ra file Markdown
    static void runFullSuite(const std::string& directoryPath, const std::string& reportPath);
};
