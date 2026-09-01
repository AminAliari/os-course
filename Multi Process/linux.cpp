#include <iostream>
#include <time.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <algorithm>
#include <vector>

using namespace std;

int* makeArray (int n);
void print(int* array, int n);

void sort(int* a, int n);
void mergeSort(int *a, int low, int high);
void merge(int *a, int low, int high, int mid);

int shm_id;

int main(int argc, char *argv[]) {
    srand(time(NULL));

    int n = 0;
    cout << "size of array: ";
    cin >> n;

    int* array = makeArray(n);
//    cout << "Orginal Array: ";
//    print(array, n);

    struct timespec start, end;
    double elapsed;
    clock_gettime(CLOCK_MONOTONIC, &start);

    mergeSort(array, 0, n-1);

//    cout << "New Array: ";
//    print(array, n);

    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = end.tv_sec-start.tv_sec;
    elapsed+= (end.tv_nsec-start.tv_nsec)/1000000000.0;
    cout << "elapsed time: " << elapsed << endl;

    shmdt(array);
    shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}

void mergeSort(int *a, int low, int high) {
    int len = high - low + 1;
    if (len <= 500) {
        sort(a + low, len);
        return;
    }
    if (low < high) {
        int mid = (low + high) / 2;

        pid_t highProcess = fork();
        pid_t lowProcess;

        if (highProcess == 0) {
            mergeSort(a, mid + 1, high);
            exit(0);
        } else if (highProcess > 0) {
            lowProcess = fork();
            if (lowProcess == 0) {
                mergeSort(a, low, mid);
                exit(0);
            }else if (lowProcess > 0) {
                int status;
                waitpid(lowProcess, &status, 0);
                waitpid(highProcess, &status, 0);

                merge(a, low, high, mid);
            }else if (lowProcess < 0) {
                cout << "error\n";
            }
        } else {
            cout << "error\n";
        }
    }
}

void merge(int *a, int low, int high, int mid) {

    int i, j, k;
    vector<int> temp(high - low + 1);
    i = low;
    k = 0;
    j = mid + 1;

    while (i <= mid && j <= high) {
        if (a[i] < a[j]) {
            temp[k] = a[i];
            k++;
            i++;
        } else {
            temp[k] = a[j];
            k++;
            j++;
        }
    }

    while (i <= mid) {
        temp[k] = a[i];
        k++;
        i++;
    }

    while (j <= high) {
        temp[k] = a[j];
        k++;
        j++;
    }

    for (i = low; i <= high; i++) {
        a[i] = temp[i-low];
    }
}

void sort(int* a, int n) {
    int i, k, j;
    for (i=1; i < n; i++) {
        k = a[i];
        j = i-1;
        while (j>=0 && a[j] > k) {
            a[j+1] = a[j];
            j = j -1;
        }
        a[j+1] = k;
    }
}

int* makeArray (int n) {
    shm_id = shmget(IPC_PRIVATE, n*sizeof(int), IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("shmget");
        exit(1);
    }
    int *array = (int*)shmat(shm_id, NULL, 0);
    if (array == (void*)-1) {
        perror("shmat");
        shmctl(shm_id, IPC_RMID, NULL);
        exit(1);
    }
    fill_n(array, n, 0);

    for (int i=0; i<n; i++) {
        array[i] = (rand() % n) + 1;
    }
    return array;
}


void print(int *array, int n) {
    for (int i=0; i < n; i++) {
        cout << array[i] << " ";
    }
    cout << endl;
}
