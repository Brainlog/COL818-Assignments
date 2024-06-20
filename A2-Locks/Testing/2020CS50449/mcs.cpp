#include <bits/stdc++.h>
#include <atomic>
#include <thread>
#include <chrono>
#include <omp.h>
#include "constants.h" 

using namespace std;

class QNode
{
public:
    bool locked;
    QNode *next = NULL;
    QNode() : locked(false) {}
};

class ThreadLocal
{
public:
    QNode *node;
    ThreadLocal() : node(new QNode()) {}
    QNode* get()
    {
        return node;
    }
};

class MCSLock
{
public:
    atomic<QNode*> tail;
    MCSLock()
    {
        tail.store(nullptr);
    }
    void lock(ThreadLocal *myNode, int tid)
    {
        QNode *qnode = myNode->node;
        QNode *pred = tail.exchange(qnode);
        if (pred != nullptr)
        {
            qnode->locked = true;
            pred->next = qnode; 
            int count = 0;
            while (qnode->locked){
            }
        }
        return;
    }
    void unlock(ThreadLocal *myNode, int tid)
    {
        QNode *qnode = myNode->node;
        if (qnode->next == nullptr)
        {
            QNode* dummy = qnode;
            if (tail.compare_exchange_strong(qnode, nullptr, memory_order_acq_rel))
            {
                return;
            }
            qnode = dummy;
            while (qnode->next == nullptr)
            {
            }
        }
        qnode->next->locked = false;
        qnode->next = nullptr;
    }
};

int counter = 0;
MCSLock lock1;
string s = "";
void increment(int n, ThreadLocal *myNode, int tid)
{
    for (int i = 0; i < n; i++)
    {
        lock1.lock(myNode, tid);
        counter++;
        lock1.unlock(myNode, tid);
    }
}

int main()
{
    int n = N;
    int num_threads = Threads;
    vector<ThreadLocal*> myNodes(num_threads);
    for(int i = 0; i < num_threads; i++)
    {
        myNodes[i] = new ThreadLocal();
    }
    auto start = chrono::high_resolution_clock::now();
#pragma omp parallel num_threads(num_threads)
    {
        int tid = omp_get_thread_num();
        increment(n, myNodes[tid], tid);
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "Time taken: " << elapsed.count() << " seconds" << endl;

    cout << "Counter: " << counter << endl;

    return 0;
}
