#include <bits/stdc++.h>
#include <atomic>
#include <thread>
#include <chrono>
#include <omp.h>    
#include "constants.h" 

using namespace std;

class Threadlocal {
public:
    int slot;
    Threadlocal() : slot(0) {}
    int get_slot() {
        return this->slot;
    }   
};

class ALock {
public:
    atomic<int> tail;
    bool* flag;
    int flag_size;
    int cache_size = CACHE_LINE_SIZE;
    ALock(int num_threads) : flag(new bool[num_threads * cache_size]) {
        flag_size = num_threads * cache_size;  
        tail.store(0);
        flag[0] = true;
    }
    void lock(Threadlocal *local) {
        int slot = tail.fetch_add(cache_size) % flag_size;
        local->slot = slot;
        while (!flag[slot]) {}
    }
    void unlock(Threadlocal *local){
        int slot = local->get_slot();
        flag[slot] = false;
        flag[((slot + cache_size) % flag_size)] = true;
    }
};

ALock lock1(Threads);
int counter = 0;

void increment(int n, Threadlocal* local) {
    for (int i = 0; i < n; i++) {
        lock1.lock(local);
        counter++;
        // cout << counter << endl;
        lock1.unlock(local);
    }
}


int main(){
    int n = N;
    int num_threads = Threads;
    vector<Threadlocal> locals(num_threads);

    auto start = chrono::high_resolution_clock::now();
    #pragma omp parallel num_threads(num_threads)
    {
        int tid = omp_get_thread_num();
        increment(n, &locals[tid]);
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "Time taken: " << elapsed.count() << " seconds" << endl;

    cout << "Counter: " << counter << endl;

    return 0;

}


