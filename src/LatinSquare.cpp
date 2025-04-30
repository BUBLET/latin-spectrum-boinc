#include "LatinSquare.h"
#include <algorithm>
#include <random>
#include <iostream>
#include <functional>

// Constructor
LatinSquare::LatinSquare(int n): n(n), data(n, std::vector<int>(n, -1)) {
}

// Return size
int LatinSquare::size() const {
    return n;
}

// Print function
void LatinSquare::print() const {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            std::cout << data[i][j] << " ";
        }
        std::cout << "\n";
    }
}

// Generate a random Latin square of order n
bool LatinSquare::generate() {
    if (n <= 0) return false;
    // Initialize first row to 0..n-1
    for (int j = 0; j < n; ++j) {
        data[0][j] = j;
    }
    std::random_device rd;
    std::mt19937 gen(rd());

    // Fill each subsequent row with a random permutation satisfying Latin conditions
    for (int i = 1; i < n; ++i) {
        // Available numbers for this row
        std::vector<int> available(n);
        for (int j = 0; j < n; ++j) available[j] = j;

        // Shuffle available to randomize the order
        std::shuffle(available.begin(), available.end(), gen);

        // Place values in this row by backtracking to satisfy no column conflict
        std::vector<int> row(n, -1);
        std::function<bool(int)> place = [&](int col) -> bool {
            if (col == n) return true;
            for (int k = 0; k < (int)available.size(); ++k) {
                int val = available[k];
                bool used = false;
                // Check column conflict
                for (int r = 0; r < i; ++r) {
                    if (data[r][col] == val) {
                        used = true;
                        break;
                    }
                }
                if (used) continue;
                // Place this value temporarily
                row[col] = val;
                int saved = available[k];
                available.erase(available.begin() + k);

                if (place(col + 1)) {
                    return true;
                }

                // Backtrack
                available.insert(available.begin() + k, saved);
                row[col] = -1;
            }
            return false;
        };

        if (!place(0)) {
            // If backtracking failed, restart generation
            i = 0;
            std::shuffle(available.begin(), available.end(), gen);
            // Reset first row to random as well to vary
            std::vector<int> firstRow(n);
            for (int j = 0; j < n; ++j) firstRow[j] = j;
            std::shuffle(firstRow.begin(), firstRow.end(), gen);
            for (int j = 0; j < n; ++j) {
                data[0][j] = firstRow[j];
            }
            // Clear all other rows
            for (int r = 1; r < n; ++r) {
                for (int c = 0; c < n; ++c) data[r][c] = -1;
            }
            i = 0;
            continue;
        }
        // place succeeded, fill data for row i
        for (int j = 0; j < n; ++j) {
            data[i][j] = row[j];
        }
    }
    return true;
}

// Count intercalations (2x2 Latin subsquares)
int LatinSquare::countIntercalations() const {
    int count = 0;
    // iterate over pairs of rows and columns
    for (int i = 0; i < n; ++i) {
        for (int j = i+1; j < n; ++j) {
            for (int k = 0; k < n; ++k) {
                for (int l = k+1; l < n; ++l) {
                    // Check if entries form an intercalate
                    if (data[i][k] == data[j][l] && data[i][l] == data[j][k]) {
                        if (data[i][k] != data[i][l]) {
                            count++;
                        }
                    }
                }
            }
        }
    }
    return count;
}

// Approximate count of transversals
double LatinSquare::approximateTransversals(int tries) const {
    if (tries <= 0 || n <= 0) return 0.0;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<int> perm(n);
    for (int i = 0; i < n; ++i) perm[i] = i;
    int success = 0;
    for (int t = 0; t < tries; ++t) {
        std::shuffle(perm.begin(), perm.end(), gen);
        // Check if this permutation yields a transversal
        std::vector<bool> usedValue(n, false);
        bool ok = true;
        for (int i = 0; i < n; ++i) {
            int val = data[i][perm[i]];
            if (usedValue[val]) {
                ok = false;
                break;
            }
            usedValue[val] = true;
        }
        if (ok) success++;
    }
    double factorial_n = factorial(n);
    return (double)success / tries * factorial_n;
}

// factorial helper
double LatinSquare::factorial(int x) {
    static std::vector<double> fact = {1.0};
    if ((int)fact.size() > x) {
        return fact[x];
    }
    for (int i = fact.size(); i <= x; ++i) {
        fact.push_back(fact.back() * i);
    }
    return fact[x];
}
