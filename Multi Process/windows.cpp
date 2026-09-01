#include <iostream>
#include <time.h>
#include <windows.h>
#include <sstream>
#include <string>

using namespace std;

void makeArray(int n);
void print(int n);
long long milliseconds_now();

void insertionSort(int* a, int n);
void mergeSort(int low, int high, const string& mapName, int n, const string& exePath);
void merge(int low, int high, int mid);
bool spawnChildAndWait(const string& exePath, const string& mapName, int n, int low, int high);

int* a = nullptr;

int main(int argc, char *argv[]) {
	srand(time(NULL));

	char exeBuffer[MAX_PATH] = {0};
	if (GetModuleFileNameA(NULL, exeBuffer, MAX_PATH) == 0) {
		cerr << "failed to resolve executable path" << endl;
		return 1;
	}
	string exePath = exeBuffer;

	if (argc == 6 && string(argv[1]) == "--child") {
		string mapName = argv[2];
		int n = atoi(argv[3]);
		int low = atoi(argv[4]);
		int high = atoi(argv[5]);

		HANDLE hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, mapName.c_str());
		if (hMapFile == NULL) {
			cerr << "OpenFileMapping failed" << endl;
			return 1;
		}

		a = (int*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, n * sizeof(int));
		if (a == NULL) {
			cerr << "MapViewOfFile failed" << endl;
			CloseHandle(hMapFile);
			return 1;
		}

		mergeSort(low, high, mapName, n, exePath);

		UnmapViewOfFile(a);
		CloseHandle(hMapFile);
		return 0;
	}

	int n = 0;
	cout << "size of array: ";
	cin >> n;

	if (n <= 0) {
		cout << "elapsed time: 0" << endl;
		return 0;
	}

	string mapName = "Local\\MergeSortMap_" + to_string(GetCurrentProcessId());
	HANDLE hMapFile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, n * sizeof(int), mapName.c_str());
	if (hMapFile == NULL) {
		cerr << "CreateFileMapping failed" << endl;
		return 1;
	}

	a = (int*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, n * sizeof(int));
	if (a == NULL) {
		cerr << "MapViewOfFile failed" << endl;
		CloseHandle(hMapFile);
		return 1;
	}

	makeArray(n);
	//cout << "Orginal Array: ";
	//print(n);

	long long start = milliseconds_now();

	mergeSort(0, n - 1, mapName, n, exePath);
	//cout << "New Array: ";
	//print(n);

	long long elapsed = milliseconds_now() - start;
	cout << "elapsed time: " << elapsed << endl;

	UnmapViewOfFile(a);
	CloseHandle(hMapFile);
	return 0;
}

void mergeSort(int low, int high, const string& mapName, int n, const string& exePath) {
	int len = high - low + 1;
	if (len <= 500) {
		insertionSort(a + low, len);
		return;
	}

	if (low < high) {
		int mid = (low + high) / 2;

		mergeSort(low, mid, mapName, n, exePath);

		if (!spawnChildAndWait(exePath, mapName, n, mid + 1, high)) {
			mergeSort(mid + 1, high, mapName, n, exePath);
		}

		merge(low, high, mid);
	}
}

bool spawnChildAndWait(const string& exePath, const string& mapName, int n, int low, int high) {
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	ostringstream command;
	command << '"' << exePath << '"' << " --child " << mapName << ' ' << n << ' ' << low << ' ' << high;
	string commandLine = command.str();

	if (!CreateProcessA(NULL, &commandLine[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
		cerr << "CreateProcess failed for range [" << low << ", " << high << "]" << endl;
		return false;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);

	DWORD exitCode = 1;
	GetExitCodeProcess(pi.hProcess, &exitCode);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	return exitCode == 0;
}

void merge(int low, int high, int mid) {
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

void insertionSort(int* a, int n) {
	int i, k, j;
	for (i = 1; i < n; i++) {
		k = a[i];
		j = i - 1;
		while (j >= 0 && a[j] > k) {
			a[j + 1] = a[j];
			j = j - 1;
		}
		a[j + 1] = k;
	}
}

void makeArray(int n) {
	for (int i = 0; i < n; i++) {
		a[i] = (rand() % n) + 1;
	}
}

void print(int n) {
	for (int i = 0; i < n; i++) {
		cout << a[i] << " ";
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
