#include <iostream>
#include <time.h>
#include <algorithm>
#include <vector>

using namespace std;

int* makeArray (int n);
void print(int* array, int n);

void mergeSort(int *a, int low, int high);
void merge(int *a, int low, int high, int mid);

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
    return 0;
}

void mergeSort(int *a, int low, int high) {
    if (low < high) {
        int mid = (low + high) / 2;

        mergeSort(a, low, mid);
        mergeSort(a, mid + 1, high);

        merge(a, low, high, mid);
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

int* makeArray (int n) {
    int* array = new int[n];
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
