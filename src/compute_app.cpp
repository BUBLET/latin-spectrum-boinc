// compute_app.cpp
#include <fstream>
#include <map>
#include <random>
#include "boinc_api.h"
#include "LatinSquare.h"

int main() {
    boinc_init();
    int n, task_id, num_tasks, restarts, moves_per, maxTransOps;
    std::ifstream fin("in");
    while (fin >> std::ws) {
        std::string key; fin >> key;
        if (key=="n") fin>>n;
        else if (key=="task_id") fin>>task_id;
        else if (key=="num_tasks") fin>>num_tasks;
        else if (key=="restarts") fin>>restarts;
        else if (key=="moves_per") fin>>moves_per;
        else if (key=="maxTransOps") fin>>maxTransOps;
    }
    fin.close();
    if (n<6||n>20) boinc_finish(1);

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
