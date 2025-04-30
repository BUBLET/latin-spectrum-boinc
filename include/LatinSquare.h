// LatinSquare.h
#pragma once
#include <vector>
#include <tuple>
#include <random>

class LatinSquare {
public:
    LatinSquare(int n);
    void generateRandom(unsigned int seed);
    int countIntercalates() const;
    int countTransversalsHeuristic(int maxOps, long &opsPerformed) const;
    bool rotateRandomIntercalate(std::mt19937 &gen);
private:
    int n;
    std::vector<std::vector<int>> grid;
    std::vector<std::tuple<int,int,int,int>> findIntercalatePositions() const;
};
