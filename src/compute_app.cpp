#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <random>
#include "LatinSquare.h"
#include "boinc_api.h"

struct StatEntry {
    int n;
    unsigned int seed;
    int intercalates;
    int transversals;
};

int main(int argc, char** argv) {
    if (boinc_init() != 0) {
        std::cerr << "BOINC init failed\n";
        return 1;
    }

    char in_name[512], out_name[512];
    if (boinc_resolve_filename("in", in_name, sizeof(in_name)) != 0) {
        std::cerr << "Cannot resolve input filename\n";
        boinc_finish(1);
    }
    if (boinc_resolve_filename("out", out_name, sizeof(out_name)) != 0) {
        std::cerr << "Cannot resolve output filename\n";
        boinc_finish(1);
    }

    std::ifstream fin(in_name);
    if (!fin.is_open()) {
        std::cerr << "Error: cannot open input file '" << in_name << "'\n";
        boinc_finish(1);
    }
    int n = 6, task_id = 0, num_tasks = 1;
    int restarts = 1, moves_per = 0, maxTransOps = 100000;
    std::string key;
    int value;
    while (fin >> key >> value) {
        if      (key == "n")            n = value;
        else if (key == "task_id")     task_id = value;
        else if (key == "num_tasks")   num_tasks = value;
        else if (key == "restarts")    restarts = value;
        else if (key == "moves_per")   moves_per = value;
        else if (key == "maxTransOps") maxTransOps = value;
    }
    fin.close();

    std::map<int,int> histI, histT;
    long totalOps = 0;
    std::vector<StatEntry> stats;

    for (int restart_idx = 0; restart_idx < restarts; ++restart_idx) {
        if (restart_idx % num_tasks != task_id) continue;
        unsigned int seed = task_id + restart_idx * num_tasks;
        std::mt19937 gen(seed);

        LatinSquare L(n);
        L.generateRandom(seed);

        int ic = L.countIntercalates();
        long ops = 0;
        int tc = (n <= 6)
                 ? L.countTransversalsExact()
                 : L.countTransversalsHeuristic(maxTransOps, ops);
        totalOps += ops;
        histI[ic]++; histT[tc]++;
        stats.push_back({n, seed, ic, tc});

        for (int mv = 0; mv < moves_per; ++mv) {
            if (!L.rotateRandomIntercalate(gen)) break;
            ic = L.countIntercalates();
            tc = (n <= 6)
                 ? L.countTransversalsExact()
                 : L.countTransversalsHeuristic(maxTransOps, ops);
            totalOps += ops;
            histI[ic]++; histT[tc]++;
            // seed+1000000*mv чтобы все строчки уникально различались (или можно просто писать тот же seed и mv)
            stats.push_back({n, seed, ic, tc});
        }
    }

    std::ofstream fout(out_name);
    if (!fout.is_open()) {
        std::cerr << "Error: cannot open output file '" << out_name << "'\n";
        boinc_finish(1);
    }

    // Сначала выводим таблицу по каждому квадрату
    fout << "n seed intercalates transversals\n";
    for (const auto& s : stats)
        fout << s.n << " " << s.seed << " " << s.intercalates << " " << s.transversals << "\n";

    fout << "\n";
    fout << "intercalates_spectrum min="  << histI.begin()->first
         << " max="  << histI.rbegin()->first
         << " distinct=" << histI.size() << "\n";
    for (auto &p : histI) fout << p.first << " " << p.second << "\n";
    fout << "transversals_spectrum min="  << histT.begin()->first
         << " max="  << histT.rbegin()->first
         << " distinct=" << histT.size() << "\n";
    for (auto &p : histT) fout << p.first << " " << p.second << "\n";
    fout << "total_ops " << totalOps << "\n";
    fout.close();

    boinc_finish(0);
    return 0;
}
