#include <bits/stdc++.h>
#include <atomic>
#include <thread>
#include <chrono>
#include <omp.h>    
#include "constants.h" 

using namespace std;
class QNode {
    public:
    bool locked;
    QNode() : locked(false) {}
};

class ThreadLocal {
    public:
    QNode* node;
    ThreadLocal() : node(new QNode()) {}
    QNode* get(){
        return node;
    }
};

class CLHLock {
    public:
    atomic<QNode*> tail;
    CLHLock() {
        tail.store(new QNode());
    }
    void lock(ThreadLocal* myNode, ThreadLocal* myPred){
        QNode* qnode  = myNode->node;
        qnode->locked = true;   
        QNode* pred = tail.exchange(qnode);
        myPred->node = pred;
        while(pred->locked) {}
    }
    void unlock(ThreadLocal* myNode, ThreadLocal* myPred){
        QNode* qnode = myNode->node;
        qnode->locked = false;
        myNode->node = myPred->node;
    }
};



int counter = 0;
CLHLock lock1;  

void increment(int n, ThreadLocal* myNode, ThreadLocal* myPred) {
    for (int i = 0; i < n; i++) {
        lock1.lock(myNode, myPred);
        counter++;
        lock1.unlock(myNode, myPred);
    }
}

int main(){
    int n = N;
    int num_threads = Threads;
    vector<ThreadLocal> myNodes(num_threads);
    vector<ThreadLocal> myPreds(num_threads);
    auto start = chrono::high_resolution_clock::now();
    #pragma omp parallel num_threads(num_threads)
    {
        int tid = omp_get_thread_num();
        increment(n, &myNodes[tid], &myPreds[tid]);
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "Time taken: " << elapsed.count() << " seconds" << endl;

    cout << "Counter: " << counter << endl;

    return 0;
}