#define n 100

template <typename A, typename F>
class Invoke
{
public:
    A val;
    F func;
    Invoke() {}
    Invoke(A val, F func) : val(val), func(func) {}
};


bool compare_and_set(volatile int *r, int expected, int new_value);


template <typename N>
class Consensus
{
public:
    volatile int FIRST;
    N *winner;
    Consensus();
    void decide(N *node);
};



template <typename A, typename F>
class Node
{
public:
    Invoke<A, F> *invoke;
    Node<A, F> *next;
    Consensus<Node<A, F>> *decideNext;
    int seq;
    Node();
    Node(Invoke<A, F> *invoke);
};


template <typename O, typename A, typename F>
class LFuniversal
{
public:
    Node<A, F> *head[n];
    Node<A, F> *tail;
    LFuniversal();
    void apply(Invoke<A, F> *invoke, O *seqstack, int i);
};

template <typename O, typename A, typename F>
class WFuniversal
{
public:
    Node<A, F> *head[n];
    Node<A, F> *tail;
    WFuniversal();
    void apply(Invoke<A, F> *invoke, O *seqstack, int i);
};




