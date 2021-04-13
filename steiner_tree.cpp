#include <stdio.h>
#include <math.h>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <functional>
#include <iostream>
#pragma warning(disable:4996)
using namespace std;
const int inf = 1e9;

int n, m, t;
// well...
vector<vector<int>> edges, metric_edges, insertions;
vector<bool> terminal;

void read_data() {
    string useless_stuff;
    for (int i = 0; i < 9; ++i) {
        getline(std::cin, useless_stuff);
    }
    std::cin >> useless_stuff;
    scanf("%d", &n);
    std::cin >> useless_stuff;
    scanf("%d", &m);
    vector<int> edges0(n, inf);
    edges.resize(n, edges0);
    insertions.resize(n, edges0);
    int u, v, c;
    for (int i = 0; i < m; ++i) {
        std::cin >> useless_stuff;
        scanf("%d%d%d", &u, &v, &c);
        edges[u - 1][v - 1] = c;
        edges[v - 1][u - 1] = c;
    }
    for (int i = 0; i < 4; ++i) {
        getline(std::cin, useless_stuff);
    }
    std::cin >> useless_stuff;
    scanf("%d", &t);
    terminal.resize(n, false);
    for (int i = 0; i < t; ++i) {
        std::cin >> useless_stuff;
        scanf("%d", &u);
        terminal[u - 1] = true;
    }
}

void metric_closing() {
    metric_edges = edges;
    vector<int> edge(2, -1);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (edges[i][j] < inf) {
                edge[0] = i;
                edge[1] = j;
            }
        }
    }

    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (i != j && metric_edges[i][k] < inf && metric_edges[k][j] < inf) {
                    if (metric_edges[i][j] > metric_edges[i][k] + metric_edges[k][j]) {
                        metric_edges[i][j] = metric_edges[i][k] + metric_edges[k][j];
                        insertions[i][j] = k;
                    }
                }
            }
        }
    }
}

vector<vector<int>> Prim_metric_MST() {
    vector<bool> in_the_tree(n, false);
    vector<vector<int>> st_edges;
    vector<int> edge(2, -1);
    for (int i = 0; i < n; ++i) {
        if (terminal[i]) {
            in_the_tree[i] = true;
            break;
        }
    }
    for (int k = 0; k < t - 1; ++k) {
        int min_cost = inf, min_u = -1, min_v = -1;
        for (int u = 0; u < n; ++u) {
            if (terminal[u]) {
                for (int v = 0; v < n; ++v) {
                    if (terminal[v]) {
                        // one's already been added, the other has not
                        if ((in_the_tree[u] && !in_the_tree[v] || in_the_tree[v] && !in_the_tree[u])) {
                            if (metric_edges[u][v] < min_cost) {
                                min_cost = metric_edges[u][v];
                                min_u = u;
                                min_v = v;
                            }
                        }
                    }
                }
            }
        }
        edge[0] = min_u;
        edge[1] = min_v;
        st_edges.push_back(edge);
        in_the_tree[min_u] = true;
        in_the_tree[min_v] = true;
    }
    return st_edges;
}

vector<vector<int>> get_path(int u, int v) {
    vector<int> edge(2, -1);
    vector<vector<int>> edge_set;
    if (insertions[u][v] < inf) {
        vector<vector<int>> path1 = get_path(u, insertions[u][v]),
            path2 = get_path(insertions[u][v], v);
        for (int i = 0; i < path2.size(); ++i) {
            path1.push_back(path2[i]);
        }
        return path1;
    }
    else {
        edge[0] = u;
        edge[1] = v;
        edge_set.push_back(edge);
        return edge_set;
    }
}

vector<vector<int>> metric_st_edges_to_normal_edges(vector<vector<int>> st_edges) {
    vector<vector<int>> normal_edges;
    for (int i = 0; i < st_edges.size(); ++i) {
        int u = st_edges[i][0], v = st_edges[i][1];
        vector<vector<int>> path = get_path(u, v);
        for (int k = 0; k < path.size(); ++k) {
            vector<int> cur_edge = path[k];
            if (std::find(normal_edges.begin(), normal_edges.end(), cur_edge) == normal_edges.end()) {
                // cur_edge is not yet in the normal_edges
                normal_edges.push_back(cur_edge);
            }
        }
    }
    return normal_edges;
}

vector<vector<int>> make_tree_Kruskal(vector<vector<int>> edge_set) {
    vector<bool> in_the_tree(n, false);
    int vertices_number = 0, first_vertix = -1;
    for (int i = 0; i < edge_set.size(); ++i) {
        int u = edge_set[i][0], v = edge_set[i][1];
        if (!in_the_tree[u]) {
            ++vertices_number;
        }
        if (!in_the_tree[v]) {
            ++vertices_number;
        }
        in_the_tree[u] = true;
        in_the_tree[v] = true;
        first_vertix = v;
    }
    in_the_tree.clear();
    in_the_tree.resize(n, false);
    in_the_tree[first_vertix] = true;
    vector<vector<int>> tree_edges_set;
    vector<int> edge(2, -1);
    for (int j = 0; j < vertices_number - 1; ++j) {
        int min_cost = inf, min_u = -1, min_v = -1;
        for (int i = 0; i < edge_set.size(); ++i) {
            int u = edge_set[i][0], v = edge_set[i][1];
            if ((in_the_tree[u] && !in_the_tree[v] || in_the_tree[v] && !in_the_tree[u])) {
                if (edges[u][v] < min_cost) {
                    min_cost = edges[u][v];
                    min_u = u;
                    min_v = v;
                }
            }
        }
        edge[0] = min_u;
        edge[1] = min_v;
        tree_edges_set.push_back(edge);
        in_the_tree[min_u] = true;
        in_the_tree[min_v] = true;
    }
    return tree_edges_set;
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
    metric_closing();
    vector<vector<int>> st;
    st = make_tree_Kruskal(metric_st_edges_to_normal_edges(Prim_metric_MST()));
    for (int i = 0; i < st.size(); ++i) {
        printf("%d %d\n", st[i][0] + 1, st[i][1] + 1);
    }

    return 0;
}