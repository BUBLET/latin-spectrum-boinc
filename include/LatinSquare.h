#ifndef LATINSQUARE_H
#define LATINSQUARE_H

// LatinSquare.h
#pragma once
#include <vector>
#include <tuple>
#include <random>
#include <string>

class LatinSquare {
public:
    LatinSquare(int n);
    // Special constructors
    static LatinSquare generateCyclic(int n);
    static LatinSquare generateBackCirculant(int n);
    static LatinSquare generateBrown(int n); // only odd n

    // Randomization (seeded)
    bool generateRandom(unsigned int seed);

    // Counts
    int countIntercalates() const;
    int countTransversalsExact() const;
    int countTransversalsHeuristic(int maxOps) const;

    // Neighborhood
    using Pos4 = std::tuple<int,int,int,int>;
    std::vector<Pos4> findIntercalatePositions() const;
    bool rotateIntercalate(const Pos4 &pos);

    // Canonical string for hashing
    std::string toString() const;

private:
    int n;
    std::vector<std::vector<int>> grid;
    int backtrackTransExact(int row,
            std::vector<bool> &usedCols,
            std::vector<bool> &usedSyms) const;
};


#endif // LATINSQUARE_H
