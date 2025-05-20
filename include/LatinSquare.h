#ifndef LATINSQUARE_H
#define LATINSQUARE_H

#include <vector>
#include <tuple>
#include <random>

class LatinSquare {
public:
    explicit LatinSquare(int n);

    bool generateRandom(unsigned int seed);
    int countIntercalates() const;
    int countTransversalsExact() const;
    int countTransversalsHeuristic(int maxOps, long &opsPerformed) const;
    bool rotateRandomIntercalate(std::mt19937 &gen);

private:
    int n;
    std::vector<std::vector<int>> grid;

    std::vector<std::tuple<int,int,int,int>> findIntercalatePositions() const;
    int backtrackTrans(int row,
                       std::vector<bool> &usedCols,
                       std::vector<bool> &usedSyms) const;
};

#endif // LATINSQUARE_H
