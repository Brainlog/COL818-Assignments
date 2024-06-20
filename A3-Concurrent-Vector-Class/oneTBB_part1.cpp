#include <iostream>
#include <tbb/parallel_vector.h>

int main() {
    // Create a parallel vector of integers
    tbb::parallel_vector<int> pv;

    // Push some elements into the parallel vector
    for (int i = 0; i < 10; ++i) {
        pv.push_back(i);
    }

    // Iterate over the elements of the parallel vector
    // Note: This will run in parallel
    pv.for_each([](int& x) {
        x *= 2; // Double each element
    });

    // Print the modified parallel vector
    for (const auto& elem : pv) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;

    return 0;
}
