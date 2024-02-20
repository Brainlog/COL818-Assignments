#include <bits/stdc++.h>
#include <pthread.h>
#include <fstream>
#include <omp.h>
// #define Log 1  // To Log comment Onpop Line


#define n 100

using namespace std;
map<int,ofstream> mp;


template <typename A, typename F>
class Invoke
{
public:
    A val;
    F func;
    Invoke() {}
    Invoke(A val, F func) : val(val), func(func) {}
};


bool compare_and_set(volatile int *r, int expected, int new_value)
{
    unsigned char result;
    asm volatile(
        "lock cmpxchg %3, %1\n\t" // Compare and swap
        "sete %0"                 // Set result based on zero flag
        : "=q"(result), "+m"(*r)
        : "a"(expected), "r"(new_value)
        : "memory");
    return result;
}


template <typename N>
class Consensus
{
public:
    volatile int FIRST;
    N* mp[n];
    Consensus()
    {
        FIRST = -1;
    }
    N* decide(N *node, int myid)
    {
        mp[myid] = node;
        bool success = compare_and_set(&this->FIRST, -1, myid);
        if (success) // Error : If winner switched, winner will be NULL, Now corrected
        {
            return mp[myid];
        }
        else{
            return mp[this->FIRST];
        }
    }
};


template <typename A, typename F>
class Node
{
public:
    Invoke<A, F> *invoke;
    Node<A, F> *next;
    Consensus<Node<A, F>> *decideNext;
    int set;
    int seq;
    Node()
    {
        decideNext = new Consensus<Node<A, F>>();
        set = 0;
    }
    Node(Invoke<A, F> *invoke) : invoke(invoke), next(NULL), seq(0)
    {
        decideNext = new Consensus<Node<A, F>>();
        set = 1;
    }
    Node<A, F> *max(Node<A, F> *arr[])
    {
        Node<A, F> *max = arr[0];
        for (int i = 1; i < n; i++)
        {
            if (arr[i]->seq > max->seq)
            {
                max = arr[i];
            }
        }
        return max;
    }
};


template <typename O, typename A, typename F>
class LFuniversal
{
public:
    Node<A, F> *head[n];
    Node<A, F> *tail;
    map<int, ofstream> mplockfree, mpwaitfree;
    LFuniversal()
    {
        tail = new Node<A, F>();
        tail->seq = 1;
        for (int i = 0; i < n; i++)
        {
            head[i] = tail;
        }
        #ifdef Log
        for (int i = 0; i < n; i++)
        {
            mplockfree[i].open("./Logs/Object/lockfree" + to_string(i) + ".txt");
            mpwaitfree[i].open("./Logs/Object/waitfree" + to_string(i) + ".txt");
        }
        #endif
    }
    void apply(Invoke<A, F> *invoke, O *object, int i)
    {
        Node<A, F> *prefer = new Node<A, F>(invoke);
        while (prefer->seq == 0)
        {
            Node<A, F> *before = tail->max(head);
            Node<A, F> *after = before->decideNext->decide(prefer, i);
            before->next = after;
            after->seq = before->seq + 1;
            head[i] = after;
        }
        Node<A, F> *current = tail->next;
        #ifdef Log
        string output = "";
        #endif
        while (current != prefer)
        {   
            #ifdef Log
            output += to_string(current->invoke->val) + " ";
            #endif
            object->apply(current->invoke);
            current = current->next;
        }
        #ifdef Log
        mplockfree[i] << output; 
        #endif
        object->apply(current->invoke);
        return;
    }
};


template <typename O, typename A, typename F>
class WFuniversal
{
public:
    Node<A, F> *announce[n];
    Node<A, F> *head[n];
    Node<A, F> *tail;
    WFuniversal()
    {
        tail = new Node<A, F>();
        tail->seq = 1;
        for (int i = 0; i < n; i++)
        {
            head[i] = tail;
            announce[i] = tail;
        }
    }
    void apply(Invoke<A, F> *invoke, O *object, int i)
    {
        Node<A, F> *prefer = new Node<A, F>(invoke);
        announce[i] = new Node<A, F>(invoke);
        head[i] = tail->max(head);
        while(announce[i]->seq == 0){
            Node<A, F> *before = head[i];
            Node<A,F> *help = announce[(before->seq+1) % n];
            if(help->seq == 0){
                prefer = help;
            }
            else{
                prefer = announce[i];
            }
            Node<A,F> *after = before->decideNext->decide(prefer, i);
            before->next = after;
            int a = before->seq + 1;
            after->seq = a;
            head[i] = after;
        }
        Node<A, F> *current = tail->next;
        while (current != announce[i])
        {
            object->apply(current->invoke);
            current = current->next;
        }
        head[i] = announce[i];
        object->apply(current->invoke);
        return;
    } 
};
