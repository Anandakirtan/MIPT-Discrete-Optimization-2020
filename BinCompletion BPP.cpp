#include <stdio.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <iostream>
#pragma warning(disable:4996)
using namespace std;
const int inf = 1e9;

int capacity, n, lower_bound_, upper_bound_, volume_sum;
vector<int> v, sorted_index_to_original_index, cur_solution, upper_solution;

void read_data() {
    scanf("%d%d", &n, &capacity);
    v.resize(n);
    cur_solution.resize(n);
    sorted_index_to_original_index.resize(n);
    vector<pair<int, int>> indexed_v(n);
    int volume;
    for (auto i = 0; i < n; ++i) {
        scanf("%d", &volume);
        indexed_v[i] = make_pair(volume, i);
    }
    sort(indexed_v.begin(), indexed_v.end(), greater <pair<int, int>>());
    for (auto i = 0; i < n; ++i) {
        v[i] = indexed_v[i].first;
        sorted_index_to_original_index[i] = indexed_v[i].second;
    }
    accumulate(v.begin(), v.end(), volume_sum = 0);
}

void print_upper_solution() {
    vector<int> answer(n);
    for (int i = 0; i < n; ++i) {
        answer[sorted_index_to_original_index[i]] = upper_solution[i];
    }
    for (int i = 0; i < n; ++i) {
        printf("%d ", answer[i]);
    }
    printf("\n");
}

int trivial_lower_bound(double volume) {
    return ceil(volume / capacity);
}

/* always better than the first,
computes minimal wasted space*/
int waste_lower_bound() {
    vector<int> suffix_sums(n);
    partial_sum(v.rbegin(), v.rend(), suffix_sums.rbegin());
    suffix_sums.push_back(0); // for convenience 
    int cur_element_index = 0, waste = 0, carry_over = 0, last_element_index = n;
    // technically last_element_index is just past last element

    while (cur_element_index < last_element_index) {
        int remaining_capacity = capacity - v[cur_element_index];

        // find all element with value <= remaining capacity
        int l = cur_element_index, r = last_element_index;
        while (r - l > 1) {
            int m = (l + r) / 2;
            if (v[m] <= remaining_capacity) {
                r = m;
            }
            else {
                l = m;
            }
        }

        // if there are such elements, take their sum
        int sum_of_lesser = carry_over;
        carry_over = 0;
        sum_of_lesser += suffix_sums[r] - suffix_sums[last_element_index];
        last_element_index = r; // delete these values

        if (sum_of_lesser < remaining_capacity) {
            waste += remaining_capacity - sum_of_lesser;
        }

        if (remaining_capacity < sum_of_lesser) {
            carry_over += sum_of_lesser - remaining_capacity;
        }
        // r == s -- no waste, no carrying over

        ++cur_element_index;
    }
    return ceil((waste + (double)accumulate(v.begin(), v.end(), 0)) / capacity);
}

int ffd_simple() { // first fit decreasing
// returns number of bins, solution is saved to cur_solution
    vector<int> remaining_bin_capacity;
    bool is_allocated;
    for (auto i = 0; i < n; ++i) {
        is_allocated = false;
        for (auto j = 0; j < remaining_bin_capacity.size(); ++j) {
            if (remaining_bin_capacity[j] >= v[i]) {
                remaining_bin_capacity[j] -= v[i];
                cur_solution[i] = j + 1;
                is_allocated = true;
                break;
            }
        }
        if (!is_allocated) {
            remaining_bin_capacity.push_back(capacity - v[i]);
        }
    }
    return remaining_bin_capacity.size();
}

int bfd_simple() {
    vector<int> remaining_bin_capacity;
    for (auto i = 0; i < n; ++i) {
        int best_fit_index = -1, best_fit_value = capacity;
        for (auto j = 0; j < remaining_bin_capacity.size(); ++j) {
            if (remaining_bin_capacity[j] >= v[i] && remaining_bin_capacity[j] < best_fit_value) {
                best_fit_index = j;
                best_fit_value = remaining_bin_capacity[j];
            }
        }
        if (best_fit_index == -1) {
            cur_solution[i] = remaining_bin_capacity.size() + 1;
            remaining_bin_capacity.push_back(capacity - v[i]);
        }
        else {
            cur_solution[i] = best_fit_index + 1;
            remaining_bin_capacity[best_fit_index] -= v[i];
        }
    }
    return remaining_bin_capacity.size();
}

int find_largest_unpacked_index(vector<int>& packed) {
    for (int i = 0; i < n; ++i) {
        if (packed[i] == -1) {
            return i;
        }
    }
    return -1;
}

int find_largest_unpacked_fitting_index(vector<int>& packed, int volume_limit) {
    for (int i = 0; i < n; ++i) {
        if (packed[i] == -1 && v[i] <= volume_limit) {
            return i;
        }
    }
    return -1;
}

void pack_set(vector<int>& packed, vector<int>& set, int bin_number) {
    for (int i = 0; i < set.size(); ++i) {
        packed[set[i]] = bin_number;
    }
}

void unpack_set(vector<int>& packed, vector<int>& set) {
    for (int i = 0; i < set.size(); ++i) {
        packed[set[i]] = -1;
    }
}

// 
vector<int> unpacked_fitting_indexes(vector<int>& packed, int volume_limit) {
    vector<int> ans;
    // it's better to reverse sorting for tree searching for sets
    for (int i = n - 1; i >= 0; --i) {
        if (packed[i] == -1 && v[i] <= volume_limit) {
            ans.push_back(i);
        }
    }
    return ans;
}

int domination_resedial_capacity = -1;

void test_domination(vector<int> included, vector<int> excluded) {
    int s = 0;
    for (int i = 0; i < included.size(); ++i) {
        s += v[i];
    }

}

vector<vector<int>> sets;

void find_feasible_sets(vector<int> included, vector<int> excluded, vector<int> remaining, int lower_b, int upper_b) {
    if (remaining.size() == 0 || upper_b == 0) {
        if (lower_b < 0) {
            // test_domination();
            sets.push_back(included);
        }
    }
    else {
        int k = remaining.back();
        int x = v[k];
        remaining.pop_back();
        if (x > upper_b) {
            // just deleting x from remaining
            find_feasible_sets(included, excluded, remaining, lower_b, upper_b);
        }
        else {
            if (x == upper_b) {
                included.push_back(k);
                find_feasible_sets(included, excluded, remaining, lower_b - x, upper_b - x);
            }
            else {
                // include
                included.push_back(k);
                find_feasible_sets(included, excluded, remaining, lower_b - x, upper_b - x);
                included.pop_back();
                //exclude
                excluded.push_back(k);
                find_feasible_sets(included, excluded, remaining, max(lower_b, x), upper_b);
                // if we exclude smth, we will have to put items with more volume in order
                // to be not dominated, hence the lower_b update
            }
        }
    }
}

int set_volume(vector<int> set) {
    int sum = 0;
    for (int i = 0; i < set.size(); ++i) {
        sum += v[set[i]];
    }
    return sum;
}

// many articles with original alg by Korf, 2002
void BinCompeltion(vector<int> packed, int depth = 1, int wasted_space = 0) {
    // find next-to-be-packed
    int core_element_index = find_largest_unpacked_index(packed);
    // if everything's packed, finish branch, rewritre upper_bound if necessary
    if (core_element_index == -1) {
        if (depth - 1 < upper_bound_) { // we haven't yet added a bin, so -1
            upper_bound_ = depth - 1;
            upper_solution = packed;
        }
        return;
    }

    // if current_solution is not better than an already existing one, prune
    if (depth >= upper_bound_) {
        // can't add a bin
        return;
    }

    // lower_bound pruning
    if (trivial_lower_bound(wasted_space + volume_sum) >= upper_bound_) {
        return;
    }

    // find undominated feasible sets
    int core_volume = v[core_element_index];
    // we are looking for a solution strictly better than upper
    int max_allowed_waste = (upper_bound_ - 1) * capacity - volume_sum - wasted_space;
    vector<int> current_set;
    packed[core_element_index] = depth;

    // find next largest
    int next_largest_index = find_largest_unpacked_fitting_index(packed, capacity - core_volume);
    if (next_largest_index == -1) {
        // nothing fits anymore
        BinCompeltion(packed, depth + 1, wasted_space + capacity - core_volume);
        return;
    }
    int next_largest_volume = v[next_largest_index];
    if (capacity - core_volume - next_largest_volume <= max_allowed_waste) {
        current_set.push_back(next_largest_index);

        // branch by it right away, because it's dfs anyway
        pack_set(packed, current_set, depth);
        BinCompeltion(packed, depth + 1, wasted_space + capacity - core_volume - next_largest_volume);
        unpack_set(packed, current_set);
    }
    else {
        // wasting too much
    }

    // search for two elements, which may dominate

    // search for any sets LOL
    sets.clear();
    vector<int> included, excluded, remaining = unpacked_fitting_indexes(packed, capacity - core_volume);
    int lower_b = next_largest_volume, upper_b = capacity - core_volume;
    find_feasible_sets(included, excluded, remaining, lower_b, upper_b);
    for (int i = 0; i < sets.size(); ++i) {
        current_set = sets[i];
        int volume = set_volume(current_set) + core_volume;
        pack_set(packed, current_set, depth);
        BinCompeltion(packed, depth + 1, wasted_space + capacity - volume);
        unpack_set(packed, current_set);
    }
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
    
    lower_bound_ = waste_lower_bound();
    upper_bound_ = bfd_simple();

    upper_solution = cur_solution;
    if (upper_bound_ == lower_bound_) {
        printf("EASY ");
        print_upper_solution();
        return 0;
    }

    vector<int> packed(n, -1);
    BinCompeltion(packed);

    print_upper_solution();

    return 0;
}