#include <bits/stdc++.h>
#include <omp.h>
#include <time.h>
#include <chrono>
using namespace std;

bool compare_and_set(volatile long long *r, long expected, long new_value)
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

class WriteDescriptor
{
public:
    int oldval, newval;
    int pos;
    bool pending;
    WriteDescriptor(int pos, int oldval, int newval) : oldval(oldval), newval(newval), pos(pos), pending(true) {}
};

class Descriptor
{
public:
    int size;
    WriteDescriptor *writeop;
    Descriptor(int size, WriteDescriptor *writeop) : size(size), writeop(writeop) {}
};

class lockfree_vector
{
public:
    void completeWrite(WriteDescriptor *writeoperation);
    void allocateBucket(int bucketidx);
    int *At(int pos);
    int highestBit(int n);
    int First_Bucket_Size = 2;
    Descriptor *desc;
    int **data;
    lockfree_vector(int size);
    lockfree_vector();
    ~lockfree_vector();
    // void reserve(int n);
    void push_back(int value);
    int pop_back();
    void write(int pos, int value);
    int read(int pos);
    int size();
};

lockfree_vector::lockfree_vector()
{
    Descriptor *temp_desc = new Descriptor(0, 0);
    int *temp_val = new int[First_Bucket_Size];
    data = new int *[32];
    data[0] = temp_val;
    desc = temp_desc;
}

lockfree_vector::~lockfree_vector()
{
}

int lockfree_vector::highestBit(int n)
{
    int index = -1;
    while (n)
    {
        n >>= 1;
        index++;
    }
    return index;
}

int *lockfree_vector::At(int pos)
{
    int i = pos + First_Bucket_Size;
    int hibit = highestBit(i);
    int id = i ^ (int)pow(2, hibit);
    return &(data[hibit - highestBit(First_Bucket_Size)][id]);
}

void lockfree_vector::completeWrite(WriteDescriptor *writeoperation)
{
    if (writeoperation == 0)
    {
        return;
    }
    if (writeoperation->pending)
    {
        int *addr = At(writeoperation->pos);
        compare_and_set((long long *)addr, writeoperation->oldval, writeoperation->newval);
        writeoperation->pending = false;
    }
}

int lockfree_vector::size()
{
    Descriptor *desc1 = desc;
    int size = desc1->size;
    if (desc1->writeop == 0)
    {
        return size;
    }
    if (desc1->writeop->pending)
    {
        size--;
    }

    return size;
}

void lockfree_vector::allocateBucket(int bucket)
{
    int bucketsiz = pow(2, bucket + highestBit(First_Bucket_Size));
    int *data1 = new int[bucketsiz];
    if (!compare_and_set((long long *)&data[bucket], 0, (long long)data1))
    {
        delete[] data1;
    }
    return;
}

int lockfree_vector::read(int pos)
{
    return *At(pos);
}

void lockfree_vector::write(int pos, int value)
{
    *At(pos) = value;
}

// template <typename T>
void lockfree_vector::push_back(int value)
{
    Descriptor *desc_new = new Descriptor(0, 0);
    Descriptor *desc_curr = new Descriptor(0, 0);
    do
    {
        desc_curr = desc;
        // cout << desc_curr->size << "In pop" << endl;
        completeWrite(desc_curr->writeop);
        int bucket = highestBit(desc_curr->size + First_Bucket_Size) - highestBit(First_Bucket_Size);
        if (data[bucket] == 0)
        {
            allocateBucket(bucket);
        }
        WriteDescriptor *writeop = new WriteDescriptor(desc_curr->size, *At(desc_curr->size), value);
        Descriptor *temp = new Descriptor(desc_curr->size + 1, writeop);
        desc_new = temp;
    } while (!compare_and_set((long long *)&desc, (long long)desc_curr, (long long)desc_new));
    completeWrite(desc_new->writeop);
    // cout << desc->size  << " push " << endl;
    return;
}

int lockfree_vector::pop_back()
{
    int elm;
    Descriptor *desc_new = new Descriptor(0, 0);
    Descriptor *desc_curr = new Descriptor(0, 0);
    do
    {
        // cout << desc->size << " pop2\n";
        desc_curr = desc;
        completeWrite(desc_curr->writeop);
        if (desc_curr->size == 0)
        {
            return -1;
        }
        elm = *At(desc_curr->size - 1);
        Descriptor *temp = new Descriptor(desc_curr->size - 1, 0);
        desc_new = temp;
    } while (!compare_and_set((long long *)&desc, (long long)desc_curr, (long long)desc_new));
    // cout << desc_curr->size << " pop " << endl;
    // cout << "value of elm : " << elm << endl;
    return elm;
}


// Driver Code

void solve(lockfree_vector* v)
{
    // Set work load (it shows the percentage of each operation)
    // For analysis, workload was chosen such that sequence of operations of different types are uniformly random
    for (int i = 1; i < 1000; i++)
    {
        v->push_back(i);
    }
    for (int i = 0; i < 100; i++)
    {
        v->pop_back();
    }
    for (int i = 0; i < 50; i++)
    {
        v->write(i, 10);
    }
    for (int i = 0; i < 50; i++)
    {
        v->read(i);
    }
}

int main()
{
    lockfree_vector* v = new lockfree_vector();
    // time
    auto start_time = chrono::high_resolution_clock::now();
    #pragma omp parallel num_threads(4)
    {
        solve(v);
    }
    cout << v->size() << "\n";
    auto end_time = chrono::high_resolution_clock::now();
    double time_taken = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
    string filename = "Driver_LockFree_vector.txt";
    ofstream file;
    file.open(filename, ios::app);
    file << "Num Threads " << 4 << " Time Taken " << time_taken << endl;
    file.close();
    cout << "Check output in Driver_LockFree_vector.txt\n";
    return 0;
}