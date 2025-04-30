// LatinSquare.cpp
#include "LatinSquare.h"
#include <numeric>
#include <algorithm>
#include <functional>
#include <random>

LatinSquare::LatinSquare(int n)
    : n(n), grid(n, std::vector<int>(n)) {}

bool LatinSquare::generateRandom(unsigned int seed) {
    // Базовый латинский квадрат (i+j)%n
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            grid[i][j] = (i + j) % n;
    if (seed == 0) return true;

    std::mt19937 gen(seed);
    std::vector<int> perm(n);
    std::iota(perm.begin(), perm.end(), 0);

    // Перестановка символов
    std::shuffle(perm.begin(), perm.end(), gen);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            grid[i][j] = perm[grid[i][j]];

    // Перестановка строк и столбцов
    std::shuffle(grid.begin(), grid.end(), gen);
    for (auto &row : grid)
        std::shuffle(row.begin(), row.end(), gen);

    return true;
}

int LatinSquare::countIntercalates() const {
    int cnt = 0;
    for (int i = 0; i < n; ++i)
        for (int j = i + 1; j < n; ++j)
            for (int c = 0; c < n; ++c)
                for (int d = c + 1; d < n; ++d) {
                    int a = grid[i][c], b = grid[i][d];
                    int c2 = grid[j][c], d2 = grid[j][d];
                    if (a == d2 && b == c2 && a != b)
                        ++cnt;
                }
    return cnt;
}

int LatinSquare::backtrackTrans(int row,
                                std::vector<bool> &usedCols,
                                std::vector<bool> &usedSyms) const {
    if (row == n) return 1;
    int total = 0;
    for (int col = 0; col < n; ++col) {
        int sym = grid[row][col];
        if (!usedCols[col] && !usedSyms[sym]) {
            usedCols[col] = usedSyms[sym] = true;
            total += backtrackTrans(row + 1, usedCols, usedSyms);
            usedCols[col] = usedSyms[sym] = false;
        }
    }
    return total;
}

int LatinSquare::countTransversalsExact() const {
    std::vector<bool> usedCols(n), usedSyms(n);
    return backtrackTrans(0, usedCols, usedSyms);
}

int LatinSquare::countTransversalsHeuristic(int maxOps,
                                            long &opsPerformed) const {
    std::vector<bool> usedCols(n), usedSyms(n);
    int count = 0, ops = 0;
    bool aborted = false;

    std::function<void(int)> dfs = [&](int row) {
        if (aborted) return;
        if (row == n) { ++count; return; }
        for (int col = 0; col < n && !aborted; ++col) {
            if (++ops > maxOps) { aborted = true; break; }
            int sym = grid[row][col];
            if (!usedCols[col] && !usedSyms[sym]) {
                usedCols[col] = usedSyms[sym] = true;
                dfs(row + 1);
                usedCols[col] = usedSyms[sym] = false;
            }
        }
    };

    dfs(0);
    opsPerformed = ops;
    return count;
}

bool LatinSquare::rotateRandomIntercalate(std::mt19937 &gen) {
    auto v = findIntercalatePositions();
    if (v.empty()) return false;
    std::uniform_int_distribution<size_t> dist(0, v.size() - 1);
    auto [i, j, c, d] = v[dist(gen)];
    std::swap(grid[i][c], grid[i][d]);
    std::swap(grid[j][c], grid[j][d]);
    return true;
}

std::vector<std::tuple<int,int,int,int>> LatinSquare::findIntercalatePositions() const {
    std::vector<std::tuple<int,int,int,int>> v;
    for (int i = 0; i < n; ++i)
        for (int j = i + 1; j < n; ++j)
            for (int c = 0; c < n; ++c)
                for (int d = c + 1; d < n; ++d) {
                    int a = grid[i][c], b = grid[i][d];
                    int c2 = grid[j][c], d2 = grid[j][d];
                    if (a == d2 && b == c2 && a != b)
                        v.emplace_back(i, j, c, d);
                }
    return v;
}
