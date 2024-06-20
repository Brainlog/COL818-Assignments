#include <bits/stdc++.h>
#include <omp.h>
#include <chrono>
#include <time.h>
using namespace std;

int main(){

    string filename = "Driver_STL_Mutex_vector.txt";
    ofstream file;
    file.open(filename, ios::app);
    vector<int> v;

    int total_ops = 1000000;
    int push_perc = 350000;
    int pop_perc = 0;
    int write_perc = 350000;
    int read_perc = 300000;

    omp_lock_t mutex;
    omp_init_lock(&mutex);

    const int num_threads = 32;

    // start timer
    auto start_time = chrono::high_resolution_clock::now();

    #pragma omp parallel for num_threads(num_threads)
    for(int i = 0; i < total_ops; i++){
        int rand_num = i;
        if(rand_num < push_perc){
            omp_set_lock(&mutex);
            v.push_back(i);
            omp_unset_lock(&mutex);
        }
        else if(rand_num < push_perc + pop_perc){
            omp_set_lock(&mutex);
            if(v.size() > 0){
                v.pop_back();
            }
            omp_unset_lock(&mutex);
        }
        else if(rand_num < push_perc + pop_perc + write_perc){
            omp_set_lock(&mutex);
            if(v.size() > 0){
                v[rand() % v.size()] = rand();
            }
            omp_unset_lock(&mutex);
        }
        else{
            omp_set_lock(&mutex);
            if(v.size() > 0){
                int x = v[rand() % v.size()];
            }
            omp_unset_lock(&mutex);
        }
    }

    auto end_time = chrono::high_resolution_clock::now();

    double time_taken = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

    file << "Num Threads" << num_threads << " Time taken :" << time_taken  << endl;
    cout << "Check output in Driver_STL_Mutex_vector.txt\n";

    file.close();
    return 0;

}