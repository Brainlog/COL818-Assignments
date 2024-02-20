#include <bits/stdc++.h>
#include <pthread.h>
#include <omp.h>
#include "universal.cpp"
#include <fstream> 

using namespace std;

template <typename A, typename F>
class SeqStack
{
public:
    stack<A> s;
    SeqStack() {}
    void apply(Invoke<A, F> *invoke)
    {
        invoke->func(s, invoke->val);
    }
};

void push(stack<int> &s, int val)
{
    s.push(val);
}

void pop(stack<int> &s, int val)
{
    s.pop();
}

template <typename O, typename A, typename F>
class ConcurrentStack_LockFree
{
public:
    LFuniversal<O, A, F> luniversal;
    void push(Invoke<A, F> *invoke, O *seqstack, int i)
    {
        luniversal.apply(invoke, seqstack, i);
    }
    void pop(Invoke<A, F> *invoke, O *seqstack, int i)
    {
        luniversal.apply(invoke, seqstack, i);
    }
};


template <typename O, typename A, typename F>
class ConcurrentStack_WaitFree
{
public:
    WFuniversal<O, A, F> wuniversal;
    void push(Invoke<A, F> *invoke, O *seqstack, int i)
    {
        wuniversal.apply(invoke, seqstack, i);
    }
    void pop(Invoke<A, F> *invoke, O *seqstack, int i)
    {
        wuniversal.apply(invoke, seqstack, i);
    }
};


// Testing the concurrent stack
int main()
{
    int a = 10;
    ConcurrentStack_LockFree<SeqStack<int, function<void(stack<int> &, int)>>, int, function<void(stack<int> &, int)>> lf_stack;
    ConcurrentStack_WaitFree<SeqStack<int, function<void(stack<int> &, int)>>, int, function<void(stack<int> &, int)>> wf_stack;

    cout << "Lock Free Universal Class Used for Concurrent Stack" << endl;

    #pragma omp parallel num_threads(n) // Don't directly use parallel for, otherwise if one thread exits, all its data will be lost
    {
        #pragma omp for schedule(static)
        for (int i = 0; i < n; i++)
        {
            class Invoke<int, function<void(stack<int> &, int)>> invoke1(i, push);
            class Invoke<int, function<void(stack<int> &, int)>> invoke2(i, pop);
            class Invoke<int, function<void(stack<int> &, int)>> *invoke1Ptr = &invoke1;
            class Invoke<int, function<void(stack<int> &, int)>> *invoke2Ptr = &invoke2;
            class SeqStack<int, function<void(stack<int> &, int)>> *seqstack = new SeqStack<int, function<void(stack<int> &, int)>>();
            lf_stack.push(invoke1Ptr, seqstack, i);
            lf_stack.pop(invoke2Ptr, seqstack, i);
            string s = "thread" + to_string(i) + " : " + to_string(seqstack->s.size()) + "\n";
            cout << s;
        }
    }

    cout << "Wait Free Universal Class Used for Concurrent Stack" << endl;

    #pragma omp parallel num_threads(n) // Don't directly use parallel for, otherwise if one thread exits, all its data will be lost
    {
        #pragma omp for schedule(static)
        for (int i = 0; i < n; i++)
        {
            class Invoke<int, function<void(stack<int> &, int)>> invoke1(i, push);
            class Invoke<int, function<void(stack<int> &, int)>> invoke2(i, pop);
            class Invoke<int, function<void(stack<int> &, int)>> *invoke1Ptr = &invoke1;
            class Invoke<int, function<void(stack<int> &, int)>> *invoke2Ptr = &invoke2;
            class SeqStack<int, function<void(stack<int> &, int)>> *seqstack = new SeqStack<int, function<void(stack<int> &, int)>>();
            wf_stack.push(invoke1Ptr, seqstack, i);
            wf_stack.pop(invoke2Ptr, seqstack, i);
            string s = "thread" + to_string(i) + " : " + to_string(seqstack->s.size()) + "\n";
            cout << s;
        }
    }

    return 0;
}
