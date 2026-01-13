#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <climits>
#include <chrono>
#include <fstream>
#include <random>
#include <queue>
#include <map>

using namespace std;
using namespace std::chrono;

// --- UTILS ---

class RandomGenerator {
    mt19937 gen;
public:
    RandomGenerator() : gen(random_device{}()) {}
    int get_int(int min, int max) {
        uniform_int_distribution<> distrib(min, max);
        return distrib(gen);
    }
    string get_string(size_t length) {
        const string charset = "ACGT";
        uniform_int_distribution<> distrib(0, charset.size() - 1);
        string str(length, 0);
        generate_n(str.begin(), length, [&]() { return charset[distrib(gen)]; });
        return str;
    }
} rng;

template<typename Func>
double measure_ms(Func func) {
    auto start = high_resolution_clock::now();
    func();
    auto end = high_resolution_clock::now();
    return duration<double, milli>(end - start).count();
}

// =========================================================
// 1. CUT ROD
// =========================================================

int cut_rod_naive(const vector<int>& p, int n) {
    if (n == 0) return 0;
    int q = INT_MIN;
    for (int i = 1; i <= n; i++) {
        q = max(q, p[i-1] + cut_rod_naive(p, n - i));
    }
    return q;
}

int memoized_aux(const vector<int>& p, int n, vector<int>& r, vector<int>& s) {
    if (r[n] >= 0) return r[n];
    int q;
    if (n == 0) q = 0;
    else {
        q = INT_MIN;
        for (int i = 1; i <= n; i++) {
            int val = p[i-1] + memoized_aux(p, n - i, r, s);
            if (val > q) {
                q = val;
                s[n] = i;
            }
        }
    }
    r[n] = q;
    return q;
}

pair<int, vector<int>> cut_rod_memoized(const vector<int>& p, int n) {
    vector<int> r(n + 1, INT_MIN);
    vector<int> s(n + 1, 0);
    int max_val = memoized_aux(p, n, r, s);

    vector<int> cuts;
    int temp = n;
    while (temp > 0) {
        cuts.push_back(s[temp]);
        temp -= s[temp];
    }
    return {max_val, cuts};
}

pair<int, vector<int>> cut_rod_iterative(const vector<int>& p, int n) {
    vector<int> r(n + 1, 0);
    vector<int> s(n + 1, 0);

    for (int j = 1; j <= n; j++) {
        int q = INT_MIN;
        for (int i = 1; i <= j; i++) {
            if (q < p[i-1] + r[j - i]) {
                q = p[i-1] + r[j - i];
                s[j] = i;
            }
        }
        r[j] = q;
    }

    vector<int> cuts;
    int temp = n;
    while (temp > 0) {
        cuts.push_back(s[temp]);
        temp -= s[temp];
    }
    return {r[n], cuts};
}

// =========================================================
// 2. LCS
// =========================================================

string lcs_reconstruct(const string& X, const string& Y, const vector<vector<int>>& table) {
    string res = "";
    int i = X.length(), j = Y.length();
    while (i > 0 && j > 0) {
        if (X[i-1] == Y[j-1]) {
            res += X[i-1];
            i--; j--;
        } else if (table[i-1][j] >= table[i][j-1]) {
            i--;
        } else {
            j--;
        }
    }
    reverse(res.begin(), res.end());
    return res;
}

int lcs_rec_aux(const string& X, const string& Y, int i, int j, vector<vector<int>>& memo) {
    if (i == 0 || j == 0) return 0;
    if (memo[i][j] != -1) return memo[i][j];

    if (X[i-1] == Y[j-1])
        memo[i][j] = 1 + lcs_rec_aux(X, Y, i-1, j-1, memo);
    else
        memo[i][j] = max(lcs_rec_aux(X, Y, i-1, j, memo), lcs_rec_aux(X, Y, i, j-1, memo));

    return memo[i][j];
}

string lcs_memoized(const string& X, const string& Y) {
    int m = X.length(), n = Y.length();
    vector<vector<int>> memo(m + 1, vector<int>(n + 1, -1));
    lcs_rec_aux(X, Y, m, n, memo);

    for(int i=0; i<=m; i++) memo[i][0] = 0;
    for(int j=0; j<=n; j++) memo[0][j] = 0;

    return lcs_reconstruct(X, Y, memo);
}

string lcs_iterative(const string& X, const string& Y) {
    int m = X.length(), n = Y.length();
    vector<vector<int>> c(m + 1, vector<int>(n + 1, 0));

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (X[i-1] == Y[j-1])
                c[i][j] = c[i-1][j-1] + 1;
            else
                c[i][j] = max(c[i-1][j], c[i][j-1]);
        }
    }
    return lcs_reconstruct(X, Y, c);
}

// =========================================================
// 3. ACTIVITY SELECTOR
// =========================================================

struct Activity { int id; int s, f; };

void activity_rec_aux(const vector<Activity>& acts, int k, vector<Activity>& res) {
    size_t m = k + 1;
    while (m < acts.size() && acts[m].s < acts[k].f) m++;
    if (m < acts.size()) {
        res.push_back(acts[m]);
        activity_rec_aux(acts, m, res);
    }
}

vector<Activity> activity_recursive(vector<Activity> acts) {
    if (acts.empty()) return {};
    sort(acts.begin(), acts.end(), [](const Activity& a, const Activity& b){ return a.f < b.f; });
    vector<Activity> res;
    res.push_back(acts[0]);
    activity_rec_aux(acts, 0, res);
    return res;
}

vector<Activity> activity_iterative(vector<Activity> acts) {
    if (acts.empty()) return {};
    sort(acts.begin(), acts.end(), [](const Activity& a, const Activity& b){ return a.f < b.f; });
    vector<Activity> res;
    res.push_back(acts[0]);
    int k = 0;
    for (size_t m = 1; m < acts.size(); m++) {
        if (acts[m].s >= acts[k].f) {
            res.push_back(acts[m]);
            k = m;
        }
    }
    return res;
}

vector<Activity> activity_start_sorted(vector<Activity> acts) {
    if (acts.empty()) return {};
    sort(acts.begin(), acts.end(), [](const Activity& a, const Activity& b){ return a.s < b.s; });

    vector<Activity> res;
    int n = acts.size();
    Activity last_selected = acts[n-1];
    res.push_back(last_selected);

    for (int i = n - 2; i >= 0; i--) {
        if (acts[i].f <= last_selected.s) {
            res.push_back(acts[i]);
            last_selected = acts[i];
        }
    }
    reverse(res.begin(), res.end());
    return res;
}

int activity_dp(vector<Activity> acts) {
    if (acts.empty()) return 0;
    sort(acts.begin(), acts.end(), [](const Activity& a, const Activity& b){ return a.f < b.f; });
    int n = acts.size();
    vector<int> dp(n, 1);
    int global_max = 1;

    for (int i = 1; i < n; i++) {
        for (int j = 0; j < i; j++) {
            if (acts[i].s >= acts[j].f && dp[i] < dp[j] + 1) {
                dp[i] = dp[j] + 1;
            }
        }
        global_max = max(global_max, dp[i]);
    }
    return global_max;
}

// =========================================================
// 4. HUFFMAN
// =========================================================

struct HuffNode {
    char ch;
    long long freq;
    HuffNode *left, *mid, *right;
    HuffNode(long long f, char c = '\0') : freq(f), ch(c), left(nullptr), mid(nullptr), right(nullptr) {}
};

struct CompareHuff {
    bool operator()(HuffNode* l, HuffNode* r) {
        return l->freq > r->freq;
    }
};

void deleteTree(HuffNode* node) {
    if (!node) return;
    deleteTree(node->left);
    deleteTree(node->mid);
    deleteTree(node->right);
    delete node;
}

void getCodesBinary(HuffNode* node, string code, map<char, string>& map) {
    if (!node) return;
    if (!node->left && !node->right) {
        map[node->ch] = code;
        return;
    }
    getCodesBinary(node->left, code + "0", map);
    getCodesBinary(node->right, code + "1", map);
}

map<char, string> huffman_binary(const vector<pair<char, long long>>& freqMap) {
    priority_queue<HuffNode*, vector<HuffNode*>, CompareHuff> Q;
    for (auto p : freqMap) Q.push(new HuffNode(p.second, p.first));

    while (Q.size() > 1) {
        HuffNode *left = Q.top(); Q.pop();
        HuffNode *right = Q.top(); Q.pop();
        HuffNode *parent = new HuffNode(left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        Q.push(parent);
    }

    map<char, string> codes;
    if (!Q.empty()) {
        HuffNode* root = Q.top();
        if (!root->left && !root->right) codes[root->ch] = "0";
        else getCodesBinary(root, "", codes);
        deleteTree(root);
    }
    return codes;
}

void getCodesTernary(HuffNode* node, string code, map<char, string>& map) {
    if (!node) return;
    if (!node->left && !node->mid && !node->right) {
        if (node->ch != '\0') map[node->ch] = code;
        return;
    }
    getCodesTernary(node->left, code + "0", map);
    if (node->mid) getCodesTernary(node->mid, code + "1", map);
    string rightCode = (node->mid) ? "2" : "1";
    getCodesTernary(node->right, code + rightCode, map);
}

map<char, string> huffman_ternary(const vector<pair<char, long long>>& freqMap) {
    priority_queue<HuffNode*, vector<HuffNode*>, CompareHuff> Q;
    for (auto p : freqMap) Q.push(new HuffNode(p.second, p.first));

    if (Q.size() % 2 == 0) Q.push(new HuffNode(0, '\0'));

    while (Q.size() > 1) {
        HuffNode *x = Q.top(); Q.pop();
        HuffNode *y = Q.top(); Q.pop();
        HuffNode *z = Q.top(); Q.pop();

        HuffNode *parent = new HuffNode(x->freq + y->freq + z->freq);
        parent->left = x; parent->mid = y; parent->right = z;
        Q.push(parent);
    }

    map<char, string> codes;
    if (!Q.empty()) {
        getCodesTernary(Q.top(), "", codes);
        deleteTree(Q.top());
    }
    return codes;
}

// =========================================================
// MAIN
// =========================================================

int main() {
    cout << "=== ROZPOCZYNAM BENCHMARKI ===" << endl;

    ofstream f("benchmark_results.csv");
    f << "Algorithm,Size_N,Time_ms\n";

    // CUT ROD
    for (int n = 1000; n <= 25000; n += 1000) {
        vector<int> prices(n);
        for(int i=0; i<n; i++) prices[i] = rng.get_int(1, 50);
        f << "CutRod_Iter," << n << "," << measure_ms([&](){ cut_rod_iterative(prices, n); }) << endl;
        f << "CutRod_Memo," << n << "," << measure_ms([&](){ cut_rod_memoized(prices, n); }) << endl;
    }

    // LCS
    for (int n = 1000; n <= 12000; n += 1000) {
        string s1 = rng.get_string(n);
        string s2 = rng.get_string(n);
        f << "LCS_Iter," << n << "," << measure_ms([&](){ lcs_iterative(s1, s2); }) << endl;
        f << "LCS_Memo," << n << "," << measure_ms([&](){ lcs_memoized(s1, s2); }) << endl;
    }

    // ACTIVITY SELECTION
    for (int n = 1000; n <= 30000; n += 1000) {
        vector<Activity> data;
        for(int i=0; i<n; i++) {
            int s = rng.get_int(0, 100000);
            data.push_back({i, s, s + rng.get_int(1, 1000)});
        }
        f << "Activity_DP," << n << "," << measure_ms([&](){ activity_dp(data); }) << endl;
        f << "Activity_GreedyStart," << n << "," << measure_ms([&](){ activity_start_sorted(data); }) << endl;
        f << "Activity_GreedyRec," << n << "," << measure_ms([&](){ activity_recursive(data); }) << endl;
        f << "Activity_GreedyIter," << n << "," << measure_ms([&](){ activity_iterative(data); }) << endl;
    }

    f.close();
    cout << "Zakonczono." << endl;

    return 0;
}