#include <bits/stdc++.h>
#include <atomic>
#include <thread>
#include <chrono>
#include <omp.h>   
#include "constants.h" 

using namespace std;

class TTASLock {
    atomic<bool> state;
public:
    TTASLock() : state(false) {}

    void lock() {
        while (true) {
            while (state.load()) {}
            if (!state.exchange(true)) {
                return;
            }
        }
    }

    void unlock() {
        state.store(false);
    }
};

TTASLock lock1;
int counter = 0;

// Function to increment counter n times using n threads
void increment(int n) {
    for (int i = 0; i < n; i++) {
        lock1.lock();
        counter++;
        lock1.unlock();
    }
}


int main(){
    int n = N;
    int num_threads = Threads;
    auto start = chrono::high_resolution_clock::now();
    #pragma omp parallel num_threads(num_threads)
    {
        int tid = omp_get_thread_num();
        increment(n);
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "Time taken: " << elapsed.count() << " seconds" << endl;

    cout << "Counter: " << counter << endl;

    return 0;
}