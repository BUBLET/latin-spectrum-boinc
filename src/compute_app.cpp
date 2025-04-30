// compute_app.cpp
#include <fstream>
#include <map>
#include <random>
#include "boinc_api.h"
#include "LatinSquare.h"

int main() {
    boinc_init();
    int n = 0, task_id = 0, num_tasks = 1, restarts = 0, moves_per = 0, maxTransOps = 0;
    std::ifstream fin("in");
    if (!fin.is_open()) {
        std::cerr << "Error: cannot open input file 'in'\n";
        boinc_finish(1);
        return 1;
    }
    
    std::string key;
    int value;
    while (fin >> key >> value) {
        if (key == "n")           n = value;
        else if (key == "task_id")   task_id = value;
        else if (key == "num_tasks") num_tasks = value;
        else if (key == "restarts")  restarts = value;
        else if (key == "moves_per") moves_per = value;
        else if (key == "maxTransOps") maxTransOps = value;
    }
    fin.close();
    
    // Проверка валидности
    if (n < 6 || n > 20 ||
        num_tasks < 1 ||
        restarts <= 0 ||
        moves_per < 0 ||
        maxTransOps <= 0) {
        std::cerr << "Bad parameters in 'in' file\n";
        boinc_finish(1);
        return 1;
    }
    

    std::map<int,int> histI, histT;
    long totalOps=0;
    for (int seed=0; seed<restarts; seed++) {
        if (seed%num_tasks!=task_id) continue;
        std::mt19937 gen(seed);
        LatinSquare L(n);
        L.generateRandom(seed);
        int ic=L.countIntercalates();
        long ops; int tc=L.countTransversalsHeuristic(maxTransOps,ops);
        totalOps+=ops;
        histI[ic]++; histT[tc]++;
        for (int mv=0; mv<moves_per; mv++) {
            if (!L.rotateRandomIntercalate(gen)) break;
            ic=L.countIntercalates();
            tc=L.countTransversalsHeuristic(maxTransOps/10,ops);
            totalOps+=ops;
            histI[ic]++; histT[tc]++;
        }
    }

    std::ofstream fout("out");
    fout<<"n "<<n<<"\n"<<"task_id "<<task_id<<"\n";
    fout<<"intercalates_spectrum min="<<histI.begin()->first
        <<" max="<<histI.rbegin()->first
        <<" distinct="<<histI.size()<<"\n";
    for (auto &p:histI) fout<<p.first<<" "<<p.second<<"\n";
    fout<<"transversals_spectrum min="<<histT.begin()->first
        <<" max="<<histT.rbegin()->first
        <<" distinct="<<histT.size()<<"\n";
    for (auto &p:histT) fout<<p.first<<" "<<p.second<<"\n";
    fout<<"total_ops "<<totalOps<<"\n";
    fout.close();

    boinc_finish(0);
    return 0;
}
