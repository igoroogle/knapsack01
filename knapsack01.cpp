#include <algorithm>
#include <cassert>
#include <iostream>
#include <set>
#include <utility>
#include <vector>

void initialize(std::vector<int>& v, std::vector<std::vector<int>>& a, std::vector<int>& b) {
    int n, m;
    std::cin >> n >> m;
    v.resize(n);
    a.assign(m, std::vector<int>(n));
    b.resize(m);

    for (size_t i = 0; i < n; ++i) {
        std::cin >> v[i];
    }

    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            std::cin >> a[i][j];
        }
        std::cin >> b[i];
    }
}

bool is_correct(std::set<size_t>& x, const std::vector<std::vector<int>>& a, const std::vector<int>& b) {
    int m = a.size();
    int n = a[0].size();
    for (size_t i = 0; i < m; ++i) {
        int sum = 0;
        for (size_t j = 0; j < n; ++j) {
            if (x.find(j) != x.end()) {
                sum += a[i][j];
                if (sum > b[i]) {
                    return false;
                }
            }
        }
    }

    return true;
}

void upd_lower_bound(const std::vector<int>& v, const std::vector<std::vector<int>>& a, const std::vector<int>& b,
                     std::set<size_t> includes, std::set<size_t>& best_ans, double& lower_bound) {
    if (!is_correct(includes, a, b)) {
        return;
    }

    double sum = 0;
    for (auto cur: includes) {
        sum += v[cur];
    }

    if (sum > lower_bound) {
        lower_bound = sum;
        best_ans = includes;
    }
}

double get_upper_bound(const std::vector<int>& v, const std::vector<int>& a, int b,
        std::set<size_t>& includes, std::set<size_t>&others) {

    double ans = 0;
    double sum = 0;
    for (auto& cur : includes) {
        sum += a[cur];
        ans += v[cur];
    }

    std::vector<std::pair<double, size_t>> values;
    values.reserve(others.size());

    for (auto& cur : others) {
        values.emplace_back(static_cast<double>(v[cur]) / static_cast<double>(a[cur]), cur);
    }

    std::sort(values.begin(), values.end(), std::greater<std::pair<double, size_t>>());
    for (auto cur : values) {
        if (sum + a[cur.second] >= b) {
            ans += (static_cast<double>(b) - sum) / static_cast<double>(a[cur.second])
                    * static_cast<double>(v[cur.second]);
            break;
        }

        sum += a[cur.second];
        ans += v[cur.second];
    }

    return ans;
}

double get_min_upper_bound(const std::vector<int>& v, const std::vector<std::vector<int>>& a, const std::vector<int>& b,
                           std::set<size_t> includes, std::set<size_t> others) {
    double ans = get_upper_bound(v, a[0], b[0], includes, others);

    for (size_t i = 1; i < a.size(); ++i) {
        ans = std::min(ans, get_upper_bound(v, a[i], b[i], includes, others));
    }

    return ans;
}

//branch and bound
void solve(const std::vector<int>& v, const std::vector<std::vector<int>>& a, const std::vector<int>& b,
           std::set<size_t> includes, std::set<size_t> excludes, std::set<size_t> others,
           std::set<size_t>& best_ans, double& lower_bound) {

    upd_lower_bound(v, a, b, includes, best_ans, lower_bound);

    if (others.empty()) {
        return;
    }

    std::vector<std::pair<double, size_t>> values;
    std::set<size_t> old_others = others;
    for (auto cur : old_others) {
        others.erase(cur);
        includes.insert(cur);
        values.emplace_back(get_min_upper_bound(v, a, b, includes, others), cur);
        others.insert(cur);
        includes.erase(cur);
    }
    std::sort(values.begin(), values.end(), std::greater<std::pair<double, size_t>>());


    for (auto cur : values) {
        if (cur.first <= lower_bound) {
            excludes.insert(cur.second);
            others.erase(cur.second);
        }
    }

    for (auto cur : values) {
        if (cur.first > lower_bound) {
            includes.insert(cur.second);
            others.erase(cur.second);
            solve(v, a, b, includes, excludes, others, best_ans, lower_bound);
            includes.erase(cur.second);
            others.insert(cur.second);
        }
    }
}



void print(const std::set<size_t>& x, const std::vector<int>& v) {
    int n = v.size();
    int sum = 0;
    std::vector<int> ans;
    for (size_t i = 0; i < n; ++i) {
        if (x.find(i) != x.end()) {
            ans.push_back(i + 1);
            sum += v[i];
        }
    }

    std::cout << "Total: " << sum << std::endl;
    std::cout << "Use: " << ' ';
    for (auto cur : ans) {
        std::cout << cur << ' ';
    }

    std::cout << std::endl;
}


int main() {
    std::vector<int> v;
    std::vector<std::vector<int>> a;
    std::vector<int> b;
    initialize(v, a, b);

    //preparing
    std::set<size_t> includes, excludes, others, best_ans;
    double lower_bound;
    for (size_t i = 0; i < v.size(); ++i) {
        others.insert(i);
    }

    solve(v, a, b, includes, excludes, others, best_ans, lower_bound);
    assert(is_correct(best_ans, a, b));
    print(best_ans, v);
}

/*
10 2
84 34 31 14 67 65 86 98 50 7
20 12 7 75 93 21 75 67 34 28 190
41 51 24 40 84 70 34 41 49 27 250
 */
