#include <iostream>
#include <utility>
#include <algorithm>
#include "graph.h"

using namespace std;

typedef vector< vector<int> > mat;
const int inf = 2147483647;

mat identity(int n)
{
    mat ans(n, vector<int>(n, inf));
    for (int i = 0; i < n; ++i)
        ans[i][i] = 0;
    return ans;
}

mat multiply(mat a, mat b)
{
    int n = a.size();
    mat ans(n, vector<int>(n, inf));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            for (int k = 0; k < n; ++k)
                if (a[i][k] != inf && b[k][j] != inf)
                    ans[i][j] = min(ans[i][j], a[i][k]+b[k][j]);
    return ans;
}

mat graph_to_mat(Graph g)
{
    int n = g.count_vertices();
    mat v = identity(n);
    for (auto it = g.edges_begin(); it != g.edges_end(); ++it)
        v[it->vertex1][it->vertex2] = it->cost;
    return v;
}

mat fastexpo(const mat& v)
{
    int n = v.size(), b = n-1;
    mat now = v, p = identity(n);
    while (b)
        if (b%2 == 0){
            now = multiply(now, now);
            b /= 2;
        }else{
            p = multiply(p, now);
            --b;
        }
    return p;
}

vector<int> getwalk(Graph g, int a, int b)
{
    int n = g.count_vertices();
    mat v = graph_to_mat(g);
    mat final = fastexpo(v);
    if (final[a][b] == inf)
        return vector<int>();
    vector<int> sol;
    sol.push_back(a);
    while (a != b){
        if (v[a][b] == final[a][b]){
            sol.push_back(b);
            break;
        }
        for (int k = 0; k < n; ++k)
            if (k != a && v[a][k]+final[k][b] == final[a][b]){
                sol.push_back(k);
                a = k;
                break;
            }
    }
    return sol;
}

bool has_neg_cycles(Graph g)
{
    mat v = graph_to_mat(std::move(g));
    mat now = fastexpo(v);
    return multiply(now, v) != now;
}

void read_shortest_path()
{
    ifstream fin ("data.in");
    int n, m;
    fin >> n >> m;
    Graph g(n);
    for (int i = 0; i < m; ++i){
        int x, y, z;
        fin >> x >> y >> z;
        g.add_edge(x, y, z);
    }
    if (has_neg_cycles(g))
        cout << "Negative cycles!\n\n";
    int a, b;
    fin >> a >> b;
    cout << "Path is ";
    if (getwalk(g, a, b).empty())
        cout << "non-existent\n";
    else {
        for (auto x : getwalk(g, a, b))
            cout << x << " ";
        cout << "\n";
    }
    fin.close();
}

void dfs(Graph &g, int nod, unordered_set<int> &met, vector<int> &sol)
{
    met.insert(nod);
    for (auto child = g.neighbours_begin(nod); child != g.neighbours_end(nod); ++child)
        if (met.find(*child) == met.end())
            dfs(g, *child, met, sol);
    sol.push_back(nod);
}

int distinct_walks(Graph g, int a, int b)
{
    vector<int> toposort;
    unordered_set<int> met;
    dfs(g, a, met, toposort);
    reverse(toposort.begin(), toposort.end());
    int where1 = 0, where2 = -1;
    for (unsigned i = 0; i < toposort.size(); ++i)
        if (toposort[i] == b)
            where2 = i;
    if (where2 == -1)
        return 0;
    unordered_map<int, int> dp;
    dp[b] = 1;
    for (int i = where2-1; i >= where1; --i){
        dp[toposort[i]] = 0;
        for (auto child = g.neighbours_begin(toposort[i]); child != g.neighbours_end(toposort[i]); ++child)
            dp[toposort[i]] += dp[*child];
    }
    return dp[a];
}

void read_count_paths()
{
    ifstream fin ("data2.in");
    int n, m;
    fin >> n >> m;
    Graph g(n);
    for (int i = 0; i < m; ++i){
        int x, y;
        fin >> x >> y;
        g.add_edge(x, y);
    }
    int a, b;
    fin >> a >> b;
    cout << "There are " << distinct_walks(g, a, b) << " distinct walks\n";
    fin.close();
}

int main() {
    read_shortest_path();
    read_count_paths();
    return 0;
}