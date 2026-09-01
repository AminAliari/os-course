#include <iostream>
#include <time.h>
#include <Windows.h>
#include <thread>
#include <algorithm>

using namespace std;

int* makeArray(int n);
void print(int* array, int n);
long long milliseconds_now();

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

	long long start = milliseconds_now();

	mergeSort(array, 0, n - 1);
	//    cout << "New Array: ";
	//    print(array, n);

	long long elapsed = milliseconds_now() - start;
	cout << "elapsed time: " << elapsed << endl;

	return 0;
}

void mergeSort(int *a, int low, int high) {
	if (low < high) {
		int mid = (low + high) / 2;
		if (high - mid > 500) {
			thread t1(mergeSort, ref(a), low, mid);
			mergeSort(a, mid + 1, high);
			t1.join();
		} else {
			mergeSort(a, low, mid);
			mergeSort(a, mid + 1, high);
		}
		merge(a, low, high, mid);
	}
}

void merge(int *a, int low, int high, int mid) {

	int i, j, k;
	int *temp = new int[high - low + 1];
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
		a[i] = temp[i - low];
	}
	delete[] temp;
}

int* makeArray(int n) {
	int* array = new int[n];
	fill_n(array, n, 0);

	for (int i = 0; i<n; i++) {
		array[i] = (rand() % n) + 1;
	}
	return array;
}


void print(int *array, int n) {
	for (int i = 0; i < n; i++) {
		cout << array[i] << " ";
	}
	cout << endl;
}

long long milliseconds_now() {
	static LARGE_INTEGER s_frequency;
	static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	if (s_use_qpc) {
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (1000LL * now.QuadPart) / s_frequency.QuadPart;
	} else {
		return GetTickCount();
	}
}