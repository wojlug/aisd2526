#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <climits>

using namespace std;

struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(NULL) {}
};

// --- INSERTION SORT ON LIST ---
void sortedInsert(ListNode** head_ref, ListNode* new_node, long long &comp, long long &sign) {
    ListNode* current;
    comp++;
    if (*head_ref == NULL || (*head_ref)->val >= new_node->val) {
        new_node->next = *head_ref;
        *head_ref = new_node;
        sign += 2;
    }
    else {
        current = *head_ref;
        sign++;
        while (current->next != NULL && current->next->val < new_node->val) {
            comp++;
            current = current->next;
            sign++;
        }
        comp++;
        new_node->next = current->next;
        current->next = new_node;
        sign += 2;
    }
}

void InsertionSortList(ListNode** head_ref, long long &comp, long long &sign) {
    ListNode *sorted = NULL;
    ListNode *current = *head_ref;
    sign += 2;
    while (current != NULL) {
        ListNode *next = current->next;
        sign++;
        sortedInsert(&sorted, current, comp, sign);
        current = next;
        sign++;
    }
    *head_ref = sorted;
    sign++;
}

ListNode* arrayToList(int A[], int n) {
    if (n == 0) return NULL;
    ListNode* head = new ListNode(A[0]);
    ListNode* curr = head;
    for (int i = 1; i < n; i++) {
        curr->next = new ListNode(A[i]);
        curr = curr->next;
    }
    return head;
}

void deleteList(ListNode* head) {
    while (head) {
        ListNode* temp = head;
        head = head->next;
        delete temp;
    }
}

// --- QUICK SORT ---

int Partition(int A[], int p, int k, long long &comp, long long &sign) {
    int x = A[p]; sign++;
    int i = p;
    for (int j = p + 1; j <= k; j++) {
        comp++;
        if (A[j] <= x) {
            i++;
            swap(A[i], A[j]); sign += 3;
        }
    }
    swap(A[i], A[p]); sign += 3;
    return i;
}

void QuickSort(int A[], int p, int k, long long &comp, long long &sign) {
    if (p < k) {
        int m = Partition(A, p, k, comp, sign);
        QuickSort(A, p, m - 1, comp, sign);
        QuickSort(A, m + 1, k, comp, sign);
    }
}

// --- DUAL PIVOT QUICK SORT ---
pair<int, int> PartitionDual(int A[], int s, int e, long long &comp, long long &sign) {
    comp++;
    if (A[s] > A[e]) { swap(A[s], A[e]); sign += 3; }

    int p1 = A[s], p2 = A[e]; sign += 2;
    int i = s + 1, k = e - 1, j = s + 1;

    while (j <= k) {
        comp++;
        if (A[j] < p1) {
            swap(A[j], A[i]); sign += 3;
            i++; j++;
        } else {
            comp++;
            if (A[j] > p2) {
                swap(A[j], A[k]); sign += 3;
                k--;
            } else {
                j++;
            }
        }
    }

    swap(A[s], A[i - 1]);
    swap(A[e], A[k + 1]);
    sign += 6;

    return {i - 1, k + 1};
}

void DualPivotQuickSort(int A[], int s, int e, long long &comp, long long &sign) {
    if (s < e) {
        auto p = PartitionDual(A, s, e, comp, sign);
        DualPivotQuickSort(A, s, p.first - 1, comp, sign);
        DualPivotQuickSort(A, p.first + 1, p.second - 1, comp, sign);
        DualPivotQuickSort(A, p.second + 1, e, comp, sign);
    }
}

// --- RADIX SORT ---
void RadixSort(int A[], int n, long long &comp, long long &sign, int base) {
    int maxVal = A[0]; sign++;
    for (int i = 1; i < n; i++) {
        comp++;
        if (A[i] > maxVal) { maxVal = A[i]; sign++; }
    }

    for (long long exp = 1; maxVal / exp > 0; exp *= base) {
        vector<int> output(n);
        vector<int> count(base, 0);

        for (int i = 0; i < n; i++) count[(A[i] / exp) % base]++;
        for (int i = 1; i < base; i++) count[i] += count[i - 1];

        for (int i = n - 1; i >= 0; i--) {
            int digit = (A[i] / exp) % base;
            output[--count[digit]] = A[i]; sign++;
        }

        for (int i = 0; i < n; i++) { A[i] = output[i]; sign++; }
    }
}

// --- MODIFIED RADIX SORT ---
void ModRadixSort(int A[], int n, long long &comp, long long &sign, int base) {
    int minVal = A[0], maxVal = A[0]; sign += 2; // Start od A[0]

    for (int i = 1; i < n; i++) {
        comp++; if (A[i] < minVal) { minVal = A[i]; sign++; }
        comp++; if (A[i] > maxVal) { maxVal = A[i]; sign++; }
    }

    if (minVal < 0) {
        for (int i = 0; i < n; i++) { A[i] -= minVal; sign++; }
        maxVal -= minVal;
    }

    for (long long exp = 1; maxVal / exp > 0; exp *= base) {
        vector<int> output(n);
        vector<int> count(base, 0);

        for (int i = 0; i < n; i++) count[(A[i] / exp) % base]++;
        for (int i = 1; i < base; i++) count[i] += count[i - 1];

        for (int i = n - 1; i >= 0; i--) {
            int digit = (A[i] / exp) % base;
            output[--count[digit]] = A[i]; sign++;
        }

        for (int i = 0; i < n; i++) { A[i] = output[i]; sign++; }
    }

    if (minVal < 0) {
        for (int i = 0; i < n; i++) { A[i] += minVal; sign++; }
    }
}

// --- BUCKET SORT ---
void InsertionSortForBucket(vector<double>& A, long long &comp, long long &sign) {
    int n = A.size();
    for (int i = 1; i < n; i++) {
        double x = A[i]; sign++;
        int j = i - 1;
        while (j >= 0) {
            comp++;
            if (A[j] > x) { A[j + 1] = A[j]; sign++; j--; }
            else break;
        }
        A[j + 1] = x; sign++;
    }
}

void BucketSort(int A[], int n, long long &comp, long long &sign) {
    int maxVal = A[0];
    for (int i = 1; i < n; i++) { comp++; if (A[i] > maxVal) maxVal = A[i]; }
    maxVal++;

    vector<vector<double>> buckets(n);
    for (int i = 0; i < n; i++) {
        double val = A[i];
        int idx = (int)((val / maxVal) * n);
        if (idx >= n) idx = n - 1;
        buckets[idx].push_back(val); sign++;
    }

    int index = 0;
    for (int i = 0; i < n; i++) {
        InsertionSortForBucket(buckets[i], comp, sign);
        for (double val : buckets[i]) { A[index++] = (int)val; sign++; }
    }
}

// --- MODIFIED BUCKET SORT ---
void ModBucketSort(int A[], int n, long long &comp, long long &sign) {
    int minVal = A[0], maxVal = A[0]; sign += 2;
    for (int i = 1; i < n; i++) {
        comp++; if (A[i] < minVal) { minVal = A[i]; sign++; }
        comp++; if (A[i] > maxVal) { maxVal = A[i]; sign++; }
    }

    double range = maxVal - minVal;
    if (range == 0) return;

    vector<vector<double>> buckets(n);
    for (int i = 0; i < n; i++) {
        double val = A[i];
        int idx = (int)((val - minVal) / range * (n - 1));
        buckets[idx].push_back(val); sign++;
    }

    int index = 0;
    for (auto &bucket : buckets) {
        InsertionSortForBucket(bucket, comp, sign);
        for (double val : bucket) { A[index++] = (int)val; sign++; }
    }
}

// --- MAIN  ---
void copyArray(int src[], int dest[], int n) {
    for (int i = 0; i < n; i++) dest[i] = src[i];
}

int main() {
    srand(time(0));
    vector<int> n_values = {10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000 ,90000 , 100000};
    vector<string> data_types = {"Losowe", "Posortowane", "Odwrotnie"};
    int *A;

    ofstream plik("dane_do_excela.csv");
    plik << "Algorytm,N,TypDanych,Czas_ms,Porownania,Przypisania" << endl;

    auto runTest = [&](auto sortFunc, int arr[], int n, string name, string type) {
        int *B = new int[n];
        copyArray(arr, B, n);
        long long comp = 0, sign = 0;
        clock_t start = clock();
        sortFunc(B, n, comp, sign);
        clock_t end = clock();
        double time_ms = 1000.0 * (end - start) / CLOCKS_PER_SEC;
        plik << name << "," << n << "," << type << "," << time_ms << "," << comp << "," << sign << endl;
        delete[] B;
    };

    for (int n : n_values) {
        A = new int[n];
        for (const string& type : data_types) {
            if (type == "Losowe") for (int i = 0; i < n; i++) A[i] = rand() % 100000;
            else if (type == "Posortowane") for (int i = 0; i < n; i++) A[i] = i;
            else if (type == "Odwrotnie") for (int i = 0; i < n; i++) A[i] = n - i;

            // QUICK SORT
            if (type == "Losowe" || n <= 5000) {
                 runTest([](int B[], int n, long long &c, long long &s){ QuickSort(B, 0, n - 1, c, s); }, A, n, "QuickSort", type);
                 runTest([](int B[], int n, long long &c, long long &s){ DualPivotQuickSort(B, 0, n - 1, c, s); }, A, n, "DualPivotQuickSort", type);
            }

            // BUCKET SORT
            runTest([](int B[], int n, long long &c, long long &s){ BucketSort(B, n, c, s); }, A, n, "BucketSort", type);
            runTest([](int B[], int n, long long &c, long long &s){ ModBucketSort(B, n, c, s); }, A, n, "ModBucketSort", type);

            // RADIX SORT
            vector<int> bases = {2, 10, 256};
            for(int d : bases) {
                runTest([d](int B[], int n, long long &c, long long &s){ RadixSort(B, n, c, s, d); }, A, n, "RadixSort_d" + to_string(d), type);
                runTest([d](int B[], int n, long long &c, long long &s){ ModRadixSort(B, n, c, s, d); }, A, n, "ModRadixSort_d" + to_string(d), type);
            }

            // LIST INSERTION SORT
            if (n <= 10000 && type == "Losowe") {
                 long long c = 0, s = 0;
                 ListNode* head = arrayToList(A, n);
                 clock_t start = clock();
                 InsertionSortList(&head, c, s);
                 clock_t end = clock();
                 double time_ms = 1000.0 * (end - start) / CLOCKS_PER_SEC;
                 plik << "InsertionSortList" << "," << n << "," << type << "," << time_ms << "," << c << "," << s << endl;
                 deleteList(head);
            }
        }
        delete[] A;
    }

    plik.close();
    cout << "Zakonczono. Wyniki w pliku dane_do_excela.csv" << endl;
    return 0;
}