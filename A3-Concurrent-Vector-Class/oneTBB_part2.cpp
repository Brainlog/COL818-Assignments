#include <tbb/concurrent_vector.h>
#include <iostream>
#include <vector>
#include <thread>

void push_vector(tbb::concurrent_vector<int>& cv, int start, int end) {
    for (int i = start; i < end; ++i) {
        cv.push_back(i);
    }
}

int main() {
    tbb::concurrent_vector<int> cv;

    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        int start = i * 1000000;
        int end = start + 1000000;
        threads.emplace_back(push_vector, std::ref(cv), start, end);
    }

    for (auto& th : threads) {
        th.join();
    }

    std::cout << "Concurrent vector size: " << cv.size() << std::endl;

    return 0;
}