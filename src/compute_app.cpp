// main.cpp
#include <iostream>
#include <fstream>
#include <map>
#include <queue>
#include <unordered_set>
#include <string>
#include <random>
#include "LatinSquare.h"
#include "boinc_api.h"

// Read key-value pairs from file
void readParams(const std::string &fname,
                int &n, int &task_id, int &num_tasks,
                int &maxExpansions, int &maxDepth, int &maxTransOps) {
    std::ifstream fin(fname);
    if (!fin) {
        std::cerr << "Error: cannot open input file 'in'\n";
        boinc_finish(1);
    }
    std::string key;
    int val;
    while (fin >> key >> val) {
        if      (key == "n")            n = val;
        else if (key == "task_id")     task_id = val;
        else if (key == "num_tasks")   num_tasks = val;
        else if (key == "restarts")    maxExpansions = val;
        else if (key == "moves_per")   maxDepth = val;
        else if (key == "maxTransOps") maxTransOps = val;
    }
}

int main(int argc, char** argv) {
    if (boinc_init() != 0) {
        std::cerr << "BOINC init failed\n";
        return 1;
    }
    // Default params
    int n = 6;
    int task_id = 0, num_tasks = 1;
    int maxExpansions = 1000;  // total BFS expansions budget
    int maxDepth = 3;          // max BFS depth
    int maxTransOps = 100000;  // heuristic ops budget
    readParams("in", n, task_id, num_tasks, maxExpansions, maxDepth, maxTransOps);

    // Histograms for spectra
    std::map<int,int> histI, histT;

    // Prepare base squares
    std::vector<LatinSquare> base;
    base.push_back(LatinSquare::generateCyclic(n));
    base.push_back(LatinSquare::generateBackCirculant(n));
    if (n % 2 == 1) {
        base.push_back(LatinSquare::generateBrown(n));
    }
    // Add other special constructors if needed

    // BFS across intercalate rotations
    struct Node { LatinSquare L; int depth; };
    std::queue<Node> Q;
    std::unordered_set<std::string> visited;
    int expansions = 0;

    for (size_t idx = 0; idx < base.size(); ++idx) {
        if ((int)idx % num_tasks != task_id) continue;
        Q.push({base[idx], 0});
        visited.insert(base[idx].toString());
        // Also process base square itself
        {
            int ic = base[idx].countIntercalates();
            int tc = (n <= 6
                      ? base[idx].countTransversalsExact()
                      : base[idx].countTransversalsHeuristic(maxTransOps));
            histI[ic]++;
            histT[tc]++;
            expansions++;
        }
        while (!Q.empty() && expansions < maxExpansions) {
            auto node = Q.front(); Q.pop();
            if (node.depth >= maxDepth) continue;
            auto& L = node.L;
            auto neigh = L.findIntercalatePositions();
            std::mt19937 gen(0);
            for (auto &pos : neigh) {
                LatinSquare L2 = L;
                L2.rotateIntercalate(pos);
                std::string canon = L2.toString();
                if (visited.count(canon)) continue;
                visited.insert(canon);
                int nextDepth = node.depth + 1;
                // Count
                int ic = L2.countIntercalates();
                int tc;
                int opsBudget = maxTransOps / (1 + nextDepth);
                if (n <= 6) tc = L2.countTransversalsExact();
                else         tc = L2.countTransversalsHeuristic(opsBudget);
                histI[ic]++;
                histT[tc]++;
                expansions++;
                // Enqueue if depth allows
                if (nextDepth < maxDepth && expansions < maxExpansions) {
                    Q.push({L2, nextDepth});
                }
                if (expansions >= maxExpansions) break;
            }
        }
    }

    // Write spectra to out
    std::ofstream fout("out");
    fout << "n " << n << "\n";
    fout << "intercalates_spectrum min="  << histI.begin()->first
         << " max="  << histI.rbegin()->first
         << " distinct=" << histI.size() << "\n";
    for (auto &p : histI) fout << p.first << " " << p.second << "\n";
    fout << "transversals_spectrum min="  << histT.begin()->first
         << " max="  << histT.rbegin()->first
         << " distinct=" << histT.size() << "\n";
    for (auto &p : histT) fout << p.first << " " << p.second << "\n";

    boinc_finish(0);
    return 0;
}
