// LatinSquare.cpp
#include "LatinSquare.h"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <functional>

// Constructors
LatinSquare::LatinSquare(int n): n(n), grid(n, std::vector<int>(n, 0)) {}

LatinSquare LatinSquare::generateCyclic(int n) {
    LatinSquare L(n);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            L.grid[i][j] = (i + j) % n;
    return L;
}

LatinSquare LatinSquare::generateBackCirculant(int n) {
    LatinSquare L(n);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            L.grid[i][j] = (i - j + n) % n;
    return L;
}

LatinSquare LatinSquare::generateBrown(int n) {
    // Brown's construction for odd n
    LatinSquare L(n);
    // Initialization as cyclic
    L = generateCyclic(n);
    // One simple Brown-like twist: for odd n, swap middle elements
    int m = n/2;
    for (int j = 0; j < n; ++j) {
        std::swap(L.grid[m][j], L.grid[j][m]);
    }
    return L;
}

bool LatinSquare::generateRandom(unsigned int seed) {
    if (seed == 0) return true;
    std::mt19937 gen(seed);
    std::vector<int> perm(n);
    std::iota(perm.begin(), perm.end(), 0);
    // Permute symbols, rows, cols
    std::shuffle(perm.begin(), perm.end(), gen);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            grid[i][j] = perm[grid[i][j]];
    std::shuffle(grid.begin(), grid.end(), gen);
    for (int i = 0; i < n; ++i)
        std::shuffle(grid[i].begin(), grid[i].end(), gen);
    return true;
}

int LatinSquare::countIntercalates() const {
    int cnt = 0;
    for (int i = 0; i < n; ++i)
    for (int j = i+1; j < n; ++j)
    for (int c = 0; c < n; ++c)
    for (int d = c+1; d < n; ++d) {
        int a = grid[i][c], b = grid[i][d];
        int c2 = grid[j][c], d2 = grid[j][d];
        if (a == d2 && b == c2 && a != b) cnt++;
    }
    return cnt;
}

int LatinSquare::backtrackTransExact(int row,
        std::vector<bool> &usedCols,
        std::vector<bool> &usedSyms) const {
    if (row == n) return 1;
    int total = 0;
    for (int col = 0; col < n; ++col) {
        int sym = grid[row][col];
        if (!usedCols[col] && !usedSyms[sym]) {
            usedCols[col] = usedSyms[sym] = true;
            total += backtrackTransExact(row+1, usedCols, usedSyms);
            usedCols[col] = usedSyms[sym] = false;
        }
    }
    return total;
}

int LatinSquare::countTransversalsExact() const {
    std::vector<bool> usedCols(n), usedSyms(n);
    return backtrackTransExact(0, usedCols, usedSyms);
}

int LatinSquare::countTransversalsHeuristic(int maxOps) const {
    std::vector<bool> usedCols(n), usedSyms(n);
    int count = 0;
    int ops = 0;
    bool aborted = false;
    std::function<void(int)> dfs = [&](int row) {
        if (aborted) return;
        if (row == n) { count++; return; }
        for (int col = 0; col < n && !aborted; ++col) {
            if (++ops > maxOps) { aborted = true; break; }
            int sym = grid[row][col];
            if (!usedCols[col] && !usedSyms[sym]) {
                usedCols[col] = usedSyms[sym] = true;
                dfs(row+1);
                usedCols[col] = usedSyms[sym] = false;
            }
        }
    };
    dfs(0);
    return count;
}

std::vector<LatinSquare::Pos4> LatinSquare::findIntercalatePositions() const {
    std::vector<Pos4> v;
    for (int i = 0; i < n; ++i)
    for (int j = i+1; j < n; ++j)
    for (int c = 0; c < n; ++c)
    for (int d = c+1; d < n; ++d) {
        int a = grid[i][c], b = grid[i][d];
        int c2 = grid[j][c], d2 = grid[j][d];
        if (a == d2 && b == c2 && a != b)
            v.emplace_back(i,j,c,d);
    }
    return v;
}

bool LatinSquare::rotateIntercalate(const Pos4 &pos) {
    int i,j,c,d;
    std::tie(i,j,c,d) = pos;
    std::swap(grid[i][c], grid[i][d]);
    std::swap(grid[j][c], grid[j][d]);
    return true;
}

std::string LatinSquare::toString() const {
    std::ostringstream oss;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            oss << grid[i][j] << ',';
        }
    }
    return oss.str();
}
