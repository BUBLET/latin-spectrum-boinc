#include <iostream>
#include "LatinSquare.h"
#include "boinc_api.h" // BOINC API header

int main(int argc, char** argv) {
    // Initialize BOINC
    if (boinc_init() != 0) {
        std::cerr << "BOINC initialization failed." << std::endl;
        return 1;
    }

    // Determine Latin square size from arguments or use default
    int n = 5;
    if (argc > 1) {
        try {
            n = std::stoi(argv[1]);
        } catch (...) {
            std::cerr << "Invalid size argument." << std::endl;
        }
    }

    // Create and generate Latin square
    LatinSquare ls(n);
    if (!ls.generate()) {
        std::cerr << "Failed to generate Latin Square." << std::endl;
        boinc_finish(1);
    }

    // Count intercalations
    int intercalations = ls.countIntercalations();

    // Estimate number of transversals using Monte Carlo (e.g., 10000 trials)
    int trials = 10000;
    double transversals_est = ls.approximateTransversals(trials);

    // Output results
    std::cout << "Latin Square (" << n << "x" << n << "):" << std::endl;
    ls.print();
    std::cout << "Intercalations: " << intercalations << std::endl;
    std::cout << "Estimated number of transversals (with " << trials << " trials): " << transversals_est << std::endl;

    // Finalize BOINC
    boinc_finish(0);
}
