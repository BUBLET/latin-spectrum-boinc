// main.cpp
#include <iostream>
#include <fstream>
#include <map>
#include <random>
#include "LatinSquare.h"
#include "boinc_api.h"

int main(int argc, char** argv) {
    // 1. Инициализация BOINC
    if (boinc_init() != 0) {
        std::cerr << "BOINC init failed" << std::endl;
        return 1;
    }

    // 2. Чтение параметров из файла "in"
    std::ifstream fin("in");
    if (!fin.is_open()) {
        std::cerr << "Error: cannot open input file 'in'" << std::endl;
        boinc_finish(1);
    }
    int n = 6;
    int task_id = 0, num_tasks = 1;
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

    // 3. Статистика спектров
    std::map<int,int> histI, histT;
    long totalOps = 0;

    // 4. Генерация и анализ
    for (int seed = 0; seed < restarts; ++seed) {
        if (seed % num_tasks != task_id) continue;
        std::mt19937 gen(seed);

        LatinSquare L(n);
        L.generateRandom(seed);

        // Точное число интеркаляций
        int ic = L.countIntercalates();
        // Точное или приближённое число трансверсалей
        long ops = 0;
        int tc = (n <= 6)
                 ? L.countTransversalsExact()
                 : L.countTransversalsHeuristic(maxTransOps, ops);
        totalOps += ops;
        histI[ic]++;
        histT[tc]++;

        // Локальные модификации через поворот интеркаляций
        for (int mv = 0; mv < moves_per; ++mv) {
            if (!L.rotateRandomIntercalate(gen)) break;
            ic = L.countIntercalates();
            tc = (n <= 6)
                 ? L.countTransversalsExact()
                 : L.countTransversalsHeuristic(maxTransOps, ops);
            totalOps += ops;
            histI[ic]++;
            histT[tc]++;
        }
    }

    // 5. Запись результатов в файл "out"
    std::ofstream fout("out");
    fout << "n " << n << "\n";

    fout << "intercalates_spectrum"
         << " min="     << histI.begin()->first
         << " max="     << histI.rbegin()->first
         << " distinct="<< histI.size() << "\n";
    for (auto &p : histI) {
        fout << p.first << " " << p.second << "\n";
    }

    fout << "transversals_spectrum"
         << " min="     << histT.begin()->first
         << " max="     << histT.rbegin()->first
         << " distinct="<< histT.size() << "\n";
    for (auto &p : histT) {
        fout << p.first << " " << p.second << "\n";
    }

    fout << "total_ops " << totalOps << "\n";
    fout.close();

    // 6. Завершение BOINC
    boinc_finish(0);
    return 0;
}
