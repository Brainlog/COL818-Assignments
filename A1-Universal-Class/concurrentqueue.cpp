#include <bits/stdc++.h>
#include <pthread.h>
#include <omp.h>
#include "universal.cpp"

using namespace std;

template <typename A, typename F>
class SeqQueue
{
public:
    queue<A> q;
    SeqQueue() {}
    void apply(Invoke<A, F> *invoke)
    {
        invoke->func(q, invoke->val);
    }
};

void push(queue<int> &q, int val)
{
    q.push(val);
}

void pop(queue<int> &q, int val)
{
    q.pop();
}

template <typename O, typename A, typename F>
class ConcurrentQueue_LockFree
{
public:
    LFuniversal<O, A, F> luniversal;
    void push(Invoke<A, F> *invoke, O *seqqueue, int i)
    {
        luniversal.apply(invoke, seqqueue, i);
    }
    void pop(Invoke<A, F> *invoke, O *seqqueue, int i)
    {
        luniversal.apply(invoke, seqqueue, i);
    }
};

template <typename O, typename A, typename F>
class ConcurrentQueue_WaitFree
{
public:
    WFuniversal<O, A, F> wuniversal;
    void push(Invoke<A, F> *invoke, O *seqqueue, int i)
    {
        wuniversal.apply(invoke, seqqueue, i);
    }
    void pop(Invoke<A, F> *invoke, O *seqqueue, int i)
    {
        wuniversal.apply(invoke, seqqueue, i);
    }
};

int main()
{
    int a = 10;
    ConcurrentQueue_LockFree<SeqQueue<int, function<void(queue<int> &, int)>>, int, function<void(queue<int> &, int)>> lf_queue;
    ConcurrentQueue_WaitFree<SeqQueue<int, function<void(queue<int> &, int)>>, int, function<void(queue<int> &, int)>> wf_queue;

cout << "Lock Free Universal Class Used for Concurrent Queue" << endl; 
#pragma omp parallel num_threads(n)  // Barrier
{
    #pragma omp for schedule(static)
    for (int i = 0; i < n; i++)
    {
        class Invoke<int, function<void(queue<int> &, int)>> invoke1(i, push);
        class Invoke<int, function<void(queue<int> &, int)>> invoke2(i, pop);
        class Invoke<int, function<void(queue<int> &, int)>> *invoke1Ptr = &invoke1;
        class Invoke<int, function<void(queue<int> &, int)>> *invoke2Ptr = &invoke2;
        class SeqQueue<int, function<void(queue<int> &, int)>> *seqqueue = new SeqQueue<int, function<void(queue<int> &, int)>>();
        lf_queue.push(invoke1Ptr, seqqueue, i);
        lf_queue.pop(invoke2Ptr, seqqueue, i);
        string s = "thread" + to_string(i) + " : " + to_string(seqqueue->q.size()) + "\n";
        cout << s;
    }
}

cout << "Wait Free Universal Class Used for Concurrent Queue" << endl;
#pragma omp parallel num_threads(n)  // Barrier
{
    #pragma omp for schedule(static)
    for (int i = 0; i < n; i++)
    {
        class Invoke<int, function<void(queue<int> &, int)>> invoke1(a, push);
        class Invoke<int, function<void(queue<int> &, int)>> invoke2(a, pop);
        class Invoke<int, function<void(queue<int> &, int)>> *invoke1Ptr = &invoke1;
        class Invoke<int, function<void(queue<int> &, int)>> *invoke2Ptr = &invoke2;
        class SeqQueue<int, function<void(queue<int> &, int)>> *seqqueue = new SeqQueue<int, function<void(queue<int> &, int)>>();
        wf_queue.push(invoke1Ptr, seqqueue, i);
        wf_queue.pop(invoke2Ptr, seqqueue, i);
        string s = "thread" + to_string(i) + " : " + to_string(seqqueue->q.size()) + "\n";
        cout << s;
    }
}

    return 0;
}
