// main.cpp
#include <iostream>
#include <fstream>
#include <map>
#include <random>
#include "LatinSquare.h"
#include "boinc_api.h"

int main(int argc, char** argv) {
    // 1) Инициализация BOINC
    if (boinc_init() != 0) {
        std::cerr << "BOINC init failed\n";
        return 1;
    }

    // 2) Определяем корректные пути к in/out
    char in_name[512], out_name[512];
    if (boinc_resolve_filename("in", in_name, sizeof(in_name)) != 0) {
        std::cerr << "Cannot resolve input filename\n";
        boinc_finish(1);
    }
    if (boinc_resolve_filename("out", out_name, sizeof(out_name)) != 0) {
        std::cerr << "Cannot resolve output filename\n";
        boinc_finish(1);
    }

    // 3) Читаем параметры из in
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

    // 4) Готовим гистограммы
    std::map<int,int> histI, histT;
    long totalOps = 0;

    // 5) Основной цикл по restarts
    for (int seed = 0; seed < restarts; ++seed) {
        if (seed % num_tasks != task_id) continue;
        std::mt19937 gen(seed);

        LatinSquare L(n);
        L.generateRandom(seed);

        // подсчёт интеркаляций
        int ic = L.countIntercalates();
        // подсчёт трансверсалей
        long ops = 0;
        int tc = (n <= 6)
                 ? L.countTransversalsExact()
                 : L.countTransversalsHeuristic(maxTransOps, ops);
        totalOps += ops;
        histI[ic]++; histT[tc]++;

        // локальные повороты
        for (int mv = 0; mv < moves_per; ++mv) {
            if (!L.rotateRandomIntercalate(gen)) break;
            ic = L.countIntercalates();
            tc = (n <= 6)
                 ? L.countTransversalsExact()
                 : L.countTransversalsHeuristic(maxTransOps, ops);
            totalOps += ops;
            histI[ic]++; histT[tc]++;
        }
    }

    // 6) Пишем в out
    std::ofstream fout(out_name);
    if (!fout.is_open()) {
        std::cerr << "Error: cannot open output file '" << out_name << "'\n";
        boinc_finish(1);
    }
    fout << "n " << n << "\n"
         << "intercalates_spectrum min="  << histI.begin()->first
         << " max="  << histI.rbegin()->first
         << " distinct=" << histI.size() << "\n";
    for (auto &p : histI) fout << p.first << " " << p.second << "\n";
    fout << "transversals_spectrum min="  << histT.begin()->first
         << " max="  << histT.rbegin()->first
         << " distinct=" << histT.size() << "\n";
    for (auto &p : histT) fout << p.first << " " << p.second << "\n";
    fout << "total_ops " << totalOps << "\n";
    fout.close();

    // 7) Завершаем BOINC
    boinc_finish(0);
    return 0;
}
