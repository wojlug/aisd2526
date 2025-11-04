#include <iostream>
#include <climits>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <string>
#include <vector>

using namespace std;

// -------------------- Insertion Sort --------------------
void InsertionSort(int A[], int n, long long &comp, long long &sign) {
    comp = 0;
    sign = 0;
    for (int i = 2; i <= n; i++) {
        int x = A[i];
        sign++;
        int j = i - 1;
        while (j >= 1 && A[j] > x) {
            comp++;
            A[j + 1] = A[j];
            sign++;
            j--;
        }
        if (j >= 1) comp++;
        A[j + 1] = x;
        sign++;
    }
}

// -------------------- Modified Insertion Sort --------------------
void ModInsertionSort(int A[], int n, long long &comp, long long &sign) {
    comp = 0;
    sign = 0;
    for (int i = 1; i <= n - 1; i += 2) {
        int x1 = A[i];
        sign++;
        int x2 = A[i + 1];
        sign++;
        if (x1 > x2) {
            comp++;
            swap(x1, x2);
            sign++;
        }

        int j = i - 1;
        while (j >= 1 && A[j] > x1) {
            comp++;
            A[j + 1] = A[j];
            sign++;
            j--;
        }
        if (j >= 1) comp++;
        A[j + 1] = x1;
        sign++;

        j = i;
        while (j >= 1 && A[j] > x2) {
            comp++;
            A[j + 1] = A[j];
            sign++;
            j--;
        }
        if (j >= 1) comp++;
        A[j + 1] = x2;
        sign++;
    }

    if (n % 2 == 1) {
        int x = A[n];
        sign++;
        int j = n - 1;
        while (j >= 1 && A[j] > x) {
            comp++;
            A[j + 1] = A[j];
            sign++;
            j--;
        }
        if (j >= 1) comp++;
        A[j + 1] = x;
        sign++;
    }
}

// -------------------- Merge Sort --------------------
void Merge(int A[], int p, int s, int k, long long &comp, long long &sign) {
    int n1 = s - p + 1;
    int n2 = k - s;
    int *L = new int[n1 + 1];
    int *R = new int[n2 + 1];
    for (int i = 0; i < n1; i++) {
        L[i] = A[p + i];
        sign++;
    }
    for (int j = 0; j < n2; j++) {
        R[j] = A[s + j + 1];
        sign++;
    }
    L[n1] = INT_MAX;
    R[n2] = INT_MAX;

    int i = 0, j = 0;
    for (int l = p; l <= k; l++) {
        comp++;
        if (L[i] <= R[j]) {
            A[l] = L[i];
            sign++;
            i++;
        } else {
            A[l] = R[j];
            sign++;
            j++;
        }
    }

    delete[] L;
    delete[] R;
}

void MergeSort(int A[], int p, int k, long long &comp, long long &sign) {
    if (p < k) {
        int s = (p + k) / 2;
        MergeSort(A, p, s, comp, sign);
        MergeSort(A, s + 1, k, comp, sign);
        Merge(A, p, s, k, comp, sign);
    }
}

// -------------------- Modified Merge Sort --------------------
void MergeThree(int A[], int p, int s1, int s2, int k, long long &comp, long long &sign) {
    int nTemp = s2 - p + 1;
    int *temp = new int[nTemp];
    int i = p, j = s1 + 1, t = 0;

    // Merging the first two subarrays into temp
    while (i <= s1 && j <= s2) {
        comp++;
        if (A[i] <= A[j]) {
            temp[t] = A[i];
            sign++;
            i++;
            t++;
        }
        else {
            temp[t] = A[j];
            sign++;
            j++;
            t++;
        }
    }
    while (i <= s1) {
        temp[t] = A[i];
        sign++;
        i++;
        t++;
    }
    while (j <= s2) {
        temp[t] = A[j];
        sign++;
        j++;
        t++;
    }

    // Merging temp with the third subarray A[s2+1..k]
    i = 0; j = s2 + 1; t = p;
    while (i < nTemp && j <= k) {
        comp++;
        if (temp[i] <= A[j]) {
            A[t] = temp[i];
            sign++;
            i++;
            t++;
        }
        else {
            A[t] = A[j];
            sign++;
            j++;
            t++;
        }
    }
    while (i < nTemp) {
        A[t] = temp[i];
        sign++;
        i++;
        t++;
    }
    while (j <= k) {
        A[t] = A[j];
        sign++;
        j++;
        t++;
    }

    delete[] temp;
}

void ModMergeSort(int A[], int p, int k, long long &comp, long long &sign) {
    if (p < k) {
        int s1 = p + (k - p) / 3;
        int s2 = p + 2 * (k - p) / 3;

        ModMergeSort(A, p, s1, comp, sign);
        ModMergeSort(A, s1 + 1, s2, comp, sign);
        ModMergeSort(A, s2 + 1, k, comp, sign);

        MergeThree(A, p, s1, s2, k, comp, sign);
    }
}

// -------------------- Heap Sort Binary --------------------
int LEFT(int i) { return 2 * i; }
int RIGHT(int i) { return 2 * i + 1; }

void Heapify(int A[], int n, int i, long long &comp, long long &sign) {
    int l = LEFT(i);
    int r = RIGHT(i);
    int largest = i;

    if (l <= n) {
        comp++;
        if (A[l] > A[largest]) largest = l;
    }
    if (r <= n) {
        comp++;
        if (A[r] > A[largest]) largest = r;
    }

    if (largest != i) {
        swap(A[i], A[largest]);
        sign++;
        Heapify(A, n, largest, comp, sign);
    }
}

void Build_Heap(int A[], int n, long long &comp, long long &sign) {
    for (int i = n / 2; i >= 1; i--) Heapify(A, n, i, comp, sign);
}

void Heap_Sort(int A[], int n, long long &comp, long long &sign) {
    Build_Heap(A, n, comp, sign);
    for (int i = n; i >= 2; i--) {
        swap(A[1], A[i]);
        sign++;
        Heapify(A, i - 1, 1, comp, sign);
    }
}

// -------------------- Heap Sort Ternary --------------------
int Part_1(int i) { return 3 * i - 1; }
int Part_2(int i) { return 3 * i; }
int Part_3(int i) { return 3 * i + 1; }

void HeapifyTernary(int A[], int n, int i, long long &comp, long long &sign) {
    int p1 = Part_1(i);
    int p2 = Part_2(i);
    int p3 = Part_3(i);
    int largest = i;

    if (p1 <= n) {
        comp++;
        if (A[p1] > A[largest]) largest = p1;
    }
    if (p2 <= n) {
        comp++;
        if (A[p2] > A[largest]) largest = p2;
    }
    if (p3 <= n) {
        comp++;
        if (A[p3] > A[largest]) largest = p3;
    }

    if (largest != i) {
        swap(A[i], A[largest]); sign++;
        HeapifyTernary(A, n, largest, comp, sign);
    }
}

void BuildHeapTernary(int A[], int n, long long &comp, long long &sign) {
    for (int i = n / 3; i >= 1; i--) HeapifyTernary(A, n, i, comp, sign);
}

void HeapSortTernary(int A[], int n, long long &comp, long long &sign) {
    BuildHeapTernary(A, n, comp, sign);
    for (int i = n; i >= 2; i--) {
        swap(A[1], A[i]);
        sign++;
        HeapifyTernary(A, i - 1, 1, comp, sign);
    }
}


// -------------------- Testing Functions --------------------

void copyArray(int sou[], int dest[], int n) {
    for (int i = 1; i <= n; i++) dest[i] = sou[i];
}

template <typename Func>
void testSort(Func sortFunc, int A[], int n, const string &name, const string &dataType) {
    int *B = new int[n + 1];
    copyArray(A, B, n);

    long long comp = 0, sign = 0;
    clock_t start = clock();
    sortFunc(B, n, comp, sign);
    clock_t end = clock();

    double time_ms = 1000.0 * (end - start) / CLOCKS_PER_SEC;

    cout << name << "," << n << "," << dataType << "," << time_ms << "," << comp << "," << sign << endl;

    delete[] B;
}

int main() {
    srand(time(0));

    vector<int> n_values = {10, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000, 100000};
    vector<string> data_types = {"Losowe", "Posortowane", "Odwrotnie"};

    int *A;

    cout << "Algorytm,N,TypDanych,Czas_ms,Porownania,Przypisania" << endl;

    for (int n : n_values) {
        A = new int[n + 1];

        for (const string& type : data_types) {

            if (type == "Losowe") {
                for (int i = 1; i <= n; i++) A[i] = rand() % 100000;
            } else if (type == "Posortowane") {
                for (int i = 1; i <= n; i++) A[i] = i;
            } else if (type == "Odwrotnie") {
                for (int i = 1; i <= n; i++) A[i] = n - i + 1;
            }

            if (n <= 20000) {
                testSort([](int B[], int n, long long &c, long long &s){ InsertionSort(B, n, c, s); }, A, n, "InsertionSort", type);
                testSort([](int B[], int n, long long &c, long long &s){ ModInsertionSort(B, n, c, s); }, A, n, "ModInsertionSort", type);
            }

            testSort([](int B[], int n, long long &c, long long &s){ MergeSort(B, 1, n, c, s); }, A, n, "MergeSort", type);
            testSort([](int B[], int n, long long &c, long long &s){ ModMergeSort(B, 1, n, c, s); }, A, n, "ModMergeSort", type);
            testSort([](int B[], int n, long long &c, long long &s){ Heap_Sort(B, n, c, s); }, A, n, "HeapSort", type);
            testSort([](int B[], int n, long long &c, long long &s){ HeapSortTernary(B, n, c, s); }, A, n, "HeapSortTernary", type);
        }

        delete[] A;
    }

    return 0;
}