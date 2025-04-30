// LatinSquare.cpp
#include "LatinSquare.h"
#include <algorithm>
#include <numeric>
#include <random>

LatinSquare::LatinSquare(int n): n(n), grid(n, std::vector<int>(n)) {}

void LatinSquare::generateRandom(unsigned int seed) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            grid[i][j] = (i + j) % n + 1;
    if (!seed) return;
    std::mt19937 gen(seed);
    std::vector<int> permSym(n), permRow(n), permCol(n);
    std::iota(permSym.begin(), permSym.end(), 1);
    std::iota(permRow.begin(), permRow.end(), 0);
    std::iota(permCol.begin(), permCol.end(), 0);
    std::shuffle(permSym.begin(), permSym.end(), gen);
    std::shuffle(permRow.begin(), permRow.end(), gen);
    std::shuffle(permCol.begin(), permCol.end(), gen);
    std::vector<std::vector<int>> tmp(n, std::vector<int>(n));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            tmp[i][j] = permSym[grid[permRow[i]][permCol[j]] - 1];
    grid.swap(tmp);
}

int LatinSquare::countIntercalates() const {
    int cnt = 0;
    for (int i = 0; i < n; i++) for (int j = i+1; j < n; j++)
    for (int c = 0; c < n; c++) for (int d = c+1; d < n; d++) {
        auto a = grid[i][c], b = grid[i][d];
        auto c2 = grid[j][c], d2 = grid[j][d];
        if (a==d2 && b==c2 && a!=b) cnt++;
    }
    return cnt;
}

std::vector<std::tuple<int,int,int,int>> LatinSquare::findIntercalatePositions() const {
    std::vector<std::tuple<int,int,int,int>> v;
    for (int i = 0; i < n; i++) for (int j = i+1; j < n; j++)
    for (int c = 0; c < n; c++) for (int d = c+1; d < n; d++) {
        auto a = grid[i][c], b = grid[i][d];
        auto c2 = grid[j][c], d2 = grid[j][d];
        if (a==d2 && b==c2 && a!=b)
            v.emplace_back(i,j,c,d);
    }
    return v;
}

bool LatinSquare::rotateRandomIntercalate(std::mt19937 &gen) {
    auto v = findIntercalatePositions();
    if (v.empty()) return false;
    std::uniform_int_distribution<size_t> dist(0, v.size()-1);
    auto [i,j,c,d] = v[dist(gen)];
    std::swap(grid[i][c], grid[i][d]);
    std::swap(grid[j][c], grid[j][d]);
    return true;
}

namespace {
    bool backtrackTrans(const std::vector<std::vector<int>> &G, int n,
                        int row, std::vector<bool> &usedCols,
                        std::vector<bool> &usedSyms, int &count,
                        int maxOps, long &opsPerformed, bool &aborted) {
        if (aborted) return false;
        if (row==n) { count++; return false; }
        for (int col=0; col<n && !aborted; col++) {
            if (++opsPerformed > maxOps) { aborted=true; break; }
            int sym = G[row][col];
            if (!usedCols[col] && !usedSyms[sym]) {
                usedCols[col]=usedSyms[sym]=true;
                backtrackTrans(G,n,row+1,usedCols,usedSyms,count,maxOps,opsPerformed,aborted);
                usedCols[col]=usedSyms[sym]=false;
            }
        }
        return !aborted;
    }
}

int LatinSquare::countTransversalsHeuristic(int maxOps, long &opsPerformed) const {
    std::vector<bool> usedCols(n), usedSyms(n+1);
    int count=0; bool aborted=false;
    backtrackTrans(grid,n,0,usedCols,usedSyms,count,maxOps,opsPerformed,aborted);
    return count;
}
