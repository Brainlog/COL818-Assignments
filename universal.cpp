#include <bits/stdc++.h>
#include <pthread.h>
#include <fstream>
#include <omp.h>

using namespace std;
const int n = 100; // number of threads
map<int,ofstream> mp;


// void read(int id, auto data){
//     if(mp.count(id)==0){
//         mp[id] = ofstream("thread"+to_string(id)+".txt");
//     }
//     mp[id] << data << endl;
//     return;
// }

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
    N *winner;
    N* mp[n];
    Consensus()
    {
        winner = NULL;
        FIRST = -1;
    }
    N* decide(N *node, int myid)
    {
        // pthread_t tid = pthread_self();
        // int myid = static_cast<int>(tid);
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
    LFuniversal()
    {
        tail = new Node<A, F>();
        tail->seq = 1;
        for (int i = 0; i < n; i++)
        {
            head[i] = tail;
        }
    }
    void apply(Invoke<A, F> *invoke, O *object, int i)
    {
        Node<A, F> *prefer = new Node<A, F>(invoke);
        while (prefer->seq == 0)
        {
            Node<A, F> *before = tail->max(head);
            Consensus<Node<A, F>> *decideNext = before->decideNext;
            // before->decideNext->decide(prefer);
            Node<A, F> *after = before->decideNext->decide(prefer, i);
            before->next = after;
            if(after == NULL){
                cout << "Its null\n";
            }
            after->seq = before->seq + 1;
            head[i] = after;
        }
        Node<A, F> *current = tail->next;
        string s = to_string(i) + " : " + to_string(tail->max(head)->seq) + " \n";
        cout << s;
        while (current != prefer)
        {   
            string s = to_string(i) + " f: " + to_string(current->seq) + " " + to_string(current->invoke->val) + " ";
            cout << s << endl;
            if(current->set == 0){
                cout << "NULL\n";
            }
            cout << "I am here\n";
            object->apply(current->invoke);
            current = current->next;
        }
        // s += to_string(current->seq) + " ";
        // mutex cout_mutex;
        // cout_mutex.lock();
        // cout << s << endl;
        // cout_mutex.unlock();
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
            // before->decideNext->decide(prefer);
            Node<A,F> *after = before->decideNext->decide(prefer, i);
            if(before->decideNext->winner == NULL){
                cout << "Its null\n";
            }
            // cout << after->seq << " hello " << "\n";
            before->next = after;
            int a = before->seq + 1;
            after->seq = a;
            head[i] = after;
        }
        Node<A, F> *current = tail->next;
        string s = to_string(i) + " : ";
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
