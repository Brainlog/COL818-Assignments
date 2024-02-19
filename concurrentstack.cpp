#include <bits/stdc++.h>
#include <pthread.h>
#include <omp.h>
#include "universal.cpp"

using namespace std;

template <typename A, typename F>
class SeqStack
{
public:
    stack<A> s;
    SeqStack() {}
    void apply(Invoke<A, F> *invoke)
    {
        F funcu = invoke->func;
        A val = invoke->val;
        if(val != 10){
            cout << "val is not 10" << endl;
        }
        funcu(s, invoke->val);
    }
};

void push(stack<int> &s, int val)
{
    // cout << s.size() << "size" << endl;
    s.push(val);
}

void pop(stack<int> &s, int val)
{
    s.pop();
}

int main()
{
    stack<int> s;
    int a = 10;
    int b = 5;
    pthread_t threads[n];
    WFuniversal<SeqStack<int, function<void(stack<int> &, int)>>, int, function<void(stack<int> &, int)>> universal;
     
#pragma omp parallel num_threads(100) // Don't directly use parallel for, otherwise if one thread exits, all its data will be lost
{
    #pragma omp for schedule(static)
    for (int i = 0; i < 100; i++)
    {
        class Invoke<int, function<void(stack<int> &, int)>> invoke1(a, push);
        class Invoke<int, function<void(stack<int> &, int)>> *invoke1Ptr = &invoke1;
        class SeqStack<int, function<void(stack<int> &, int)>> *seqstack = new SeqStack<int, function<void(stack<int> &, int)>>();
        universal.apply(invoke1Ptr, seqstack, i);
        string s = "thread" + to_string(i) + " : " + to_string(seqstack->s.size()) + "\n";
        #pragma omp critical
        {
        cout << s;
        cout << flush;
        }
    }
}
     
    return 0;
}
