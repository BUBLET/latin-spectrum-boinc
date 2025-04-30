#ifndef LATINSQUARE_H
#define LATINSQUARE_H

#include <vector>

class LatinSquare {
public:
    LatinSquare(int n);

    // Accessors
    int size() const;

    // Generate a Latin Square (random generation)
    bool generate();

    // Count intercalations (2x2 Latin subsquares)
    int countIntercalations() const;

    // Approximate count of transversals using Monte Carlo
    // tries: number of random permutations to sample
    double approximateTransversals(int tries) const;

    // Print the Latin Square to std::ostream
    void print() const;

private:
    int n;
    std::vector<std::vector<int>> data;

    // Helper: factorial of n
    static double factorial(int n);
};

#endif // LATINSQUARE_H
