#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <iomanip>
#include <string>

/**
 * R-Tree Dataset Generator
 * N
 * x1 y1
 * x2 y2
 * ...
 */

struct Point {
    double x, y;
};

void generateDataset(const std::string& filename, int n, double minRange, double maxRange, bool clustered = true) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    // Set precision
    outFile << n << "\n";
    outFile << std::fixed << std::setprecision(2);

    std::random_device rd;
    std::mt19937 gen(rd());

    if (!clustered) {
        // Uniform Distribution
        std::uniform_real_distribution<> dis(minRange, maxRange);
        for (int i = 0; i < n; ++i) {
            outFile << dis(gen) << " " << dis(gen) << "\n";
        }
    } else {
        int numClusters = 10;
        std::vector<Point> centers;
        std::uniform_real_distribution<> centerDis(minRange + 500, maxRange - 500);
        
        for (int i = 0; i < numClusters; ++i) {
            centers.push_back({centerDis(gen), centerDis(gen)});
        }

        std::uniform_int_distribution<> clusterPick(0, numClusters - 1);
        std::normal_distribution<> spread(0, (maxRange - minRange) / 20.0);

        for (int i = 0; i < n; ++i) {
            Point center = centers[clusterPick(gen)];
            double x = center.x + spread(gen);
            double y = center.y + spread(gen);

            if (x < minRange) x = minRange;
            if (x > maxRange) x = maxRange;
            if (y < minRange) y = minRange;
            if (y > maxRange) y = maxRange;

            outFile << x << " " << y << "\n";
        }
    }

    outFile.close();
    std::cout << "Successfully generated " << filename << " with " << n << " points." << std::endl;
}

int main() {
    // Dataset for N = 100
    //generateDataset("dataset_0000100.txt", 100, 0.0, 1000.0, false);

    // Dataset for N = 20,000
    //generateDataset("dataset_0020000.txt", 20000, 0.0, 5000.0, true);

    return 0;
}