#include <stdio.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <numeric>
#pragma warning(disable:4996)
using namespace std;
const int inf = 1e9;

int max_weight, n;
vector<int> w, v, sums;
using namespace std;

void read_data() {
    scanf("%d%d", &max_weight, &n);
    vector<int> w0(n), v0(n);
    vector<pair<double, size_t>> eff(n);
    for (size_t i = 0; i < n; ++i) {
        scanf("%d%d", &w0[i], &v0[i]);
        eff[i] = make_pair(((double)v0[i]) / w0[i], i);
    }
    w.resize(n), v.resize(n), sums.resize(n);
    sort(eff.begin(), eff.end());
    for (size_t i = 0; i < n; ++i) {
        w[i] = w0[eff[n - i - 1].second];
        v[i] = v0[eff[n - i - 1].second];
    }
}

int greedy_knapsack() {
    int cur_weight = 0, cur_value = 0, first_not_taken = -1;
    for (size_t i = 0; i < n; ++i) {
        if (cur_weight + w[i] <= max_weight) {
            cur_weight += w[i];
            cur_value += v[i];
        }
        else {
            if (first_not_taken == -1) {
                first_not_taken = i;
            }
        }
    }
    int second_value = 0;
    if (first_not_taken != -1) {
        int second_weight = 0;
        if (w[first_not_taken] <= max_weight) {
            second_weight = w[first_not_taken];
            second_value = v[first_not_taken];
        }
        for (size_t i = 0; i < n; ++i) {
            if (i != first_not_taken && second_weight + w[i] <= max_weight) {
                second_weight += w[i];
                second_value += v[i];
            }
        }
    }
    return max(cur_value, second_value);
}

double upper_knapsack(size_t start, int cur_max_weight) {
    partial_sum(w.begin() + start, w.end(), sums.begin() + start);
    int l = start, r = n, m = 0;
    while (r - l > 1) {
        m = (int)((l + r) / 2);
        if (sums[m] > cur_max_weight) {
            r = m;
        }
        else {
            l = m;
        }
    }
    int value = 0;
    for (size_t i = start; i < r; ++i) {
        value += v[i];
    }
    if (r < n) {
        value += (cur_max_weight - (double)sums[l]) / w[r] * v[r];
    }
    return value;
}

int search_answer(size_t start, int cur_max_weight, int lower_bound) {
    if (start >= n || cur_max_weight <= 0) {
        return 0;
    }
    if ((int)upper_knapsack(start, cur_max_weight) <= lower_bound) {
        return lower_bound;
    }
    for (; start < n; ++start) {
        if (w[start] <= cur_max_weight) {
            break;
        }
    }
    if (start >= n) {
        return 0;
    }
    int ans = max(v[start] + search_answer(start + 1, cur_max_weight - w[start], lower_bound - v[start]),
        search_answer(start + 1, cur_max_weight, lower_bound));

    if (ans > lower_bound) {
        lower_bound = ans;
    }
    return ans;
}

int main() {
#pragma comment(linker,"/STACK:268435456")
#ifdef _DEBUG
    freopen("input.txt", "rt", stdin);
    freopen("output.txt", "wt", stdout);
#else
    //freopen("input.txt", "rt", stdin);
    //freopen("output.txt", "wt", stdout);
#endif
    read_data();
    int lower_bound = greedy_knapsack();
    printf("%d", search_answer(0, max_weight, lower_bound));

    return 0;
}