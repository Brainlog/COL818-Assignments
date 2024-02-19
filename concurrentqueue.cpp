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

int main()
{
    stack<int> s;
    int a = 10;
    int b = 5;
    pthread_t threads[n];
    LFuniversal<SeqQueue<int, function<void(queue<int> &, int)>>, int, function<void(queue<int> &, int)>> lfuniversal;
     
#pragma omp parallel for
    for (int i = 0; i < 10; i++)
    {
        class Invoke<int, function<void(queue<int> &, int)>> invoke1(a, push);
        class Invoke<int, function<void(queue<int> &, int)>> *invoke1Ptr = &invoke1;
        class SeqQueue<int, function<void(queue<int> &, int)>> *seqstack = new SeqQueue<int, function<void(queue<int> &, int)>>();
        lfuniversal.apply(invoke1Ptr, seqstack, i);
        cout << seqstack->q.size() << "size" << endl;
    }
     
    return 0;
}
