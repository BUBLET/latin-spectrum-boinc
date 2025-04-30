// main.cpp
#include <iostream>
#include <fstream>
#include "LatinSquare.h"
#include "boinc_api.h"

int main(int argc, char** argv) {
    // 1) Инициализация BOINC
    if (boinc_init() != 0) {
        std::cerr << "BOINC init failed\n";
        return 1;
    }

    // 2) Чтение параметров из файла "in"
    std::ifstream fin("in");
    if (!fin) {
        std::cerr << "Cannot open input file 'in'\n";
        boinc_finish(1);
    }
    int n = 5;
    int tries = 10000;
    std::string key;
    long value;
    while (fin >> key >> value) {
        if (key == "n")          n = value;
        else if (key == "tries") tries = value;
    }
    fin.close();

    // 3) Генерация и подсчёты
    LatinSquare ls(n);
    if (!ls.generate()) {
        std::cerr << "Failed to generate Latin Square\n";
        boinc_finish(1);
    }
    int ic = ls.countIntercalations();
    double estT = ls.approximateTransversals(tries);

    // 4) Запись результата в файл "out"
    std::ofstream fout("out");
    fout << "n " << n << "\n"
         << "intercalations " << ic << "\n"
         << "estimated_transversals " << estT << "\n";
    fout.close();

    // 5) Завершение BOINC
    boinc_finish(0);
    return 0;
}
