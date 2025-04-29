#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include "boinc_api.h"
#include "LatinSquare.h"

int main(int argc, char** argv) {
    // Инициализация BOINC (при автономном запуске boinc_init вернет ошибку, что обрабатывается ниже)
    int boinc_status = boinc_init();
    bool boinc_attached = (boinc_status == 0);

    // Открываем входной файл (логическое имя "in")
    char input_path[512], output_path[512];
    int retval = boinc_resolve_filename("in", input_path, sizeof(input_path));
    std::ifstream fin;
    if (retval == 0) {
        fin.open(input_path);    // BOINC-режим: открываем по физическому пути
    } else {
        fin.open("in");      // Автономный режим: пытаемся открыть "in" из текущей директории
    }
    if (!fin) {
        fprintf(stderr, "Error: cannot open input file in\n");
        if (boinc_attached) boinc_finish(1);
        return 1;
    }
    // Читаем параметры задачи из входного файла
    int n = 0;
    unsigned int seed = 0;
    std::string token;
    while (fin >> token) {
        if (token == "n") {
            fin >> n;
        } else if (token == "seed") {
            fin >> seed;
        }
        // при необходимости можно добавить разбор других параметров
    }
    fin.close();

    // Валидация входных параметров
    if (n < 6 || n > 20) {
        fprintf(stderr, "Error: order n must be between 6 and 20\n");
        if (boinc_attached) boinc_finish(1);
        return 1;
    }

    // Генерируем латинский квадрат порядка n (заполняем глобальную матрицу S)
    generateLatinSquare(n, seed);

    // Вычисляем интересующие величины
    int numIntercalates = countIntercalates(n);
    int numDiagTrans = countDiagonalTransversals(n);
    bool hasOrthogonal = findOrthogonalMate(n);
    int numOrthogonal = hasOrthogonal ? 1 : 0;

    // Открываем выходной файл (логическое имя "out") и записываем результаты
    retval = boinc_resolve_filename("out", output_path, sizeof(output_path));
    FILE* fout;
    if (retval == 0) {
        fout = boinc_fopen(output_path, "w");
    } else {
        fout = fopen("out", "w");
    }
    if (!fout) {
        fprintf(stderr, "Error: cannot open output file out for writing\n");
        if (boinc_attached) boinc_finish(1);
        return 1;
    }

    // Выводим подробный отчет: сам квадрат и вычисленные показатели
    fprintf(fout, "Латинский квадрат порядка %d (seed = %u):\n", n, seed);
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            fprintf(fout, "%2d ", S[r][c]);
        }
        fprintf(fout, "\n");
    }
    fprintf(fout, "\n");
    fprintf(fout, "Число интерколятов: %d\n", numIntercalates);
    fprintf(fout, "Число диагональных трансверсалей: %d\n", numDiagTrans);
    fprintf(fout, "Число ортогональных латинских квадратов: %d\n", numOrthogonal);
    fclose(fout);

    // Удаляем файл чекпоинта при успешном завершении задания (чтобы не влиял на будущие задания)
    char checkpoint_path[512];
    if (!boinc_resolve_filename("checkpoint_state.txt", checkpoint_path, sizeof(checkpoint_path))) {
        remove(checkpoint_path);
    }

    // Сообщаем BOINC о успешном завершении (или просто завершаем программу в автономном режиме)
    if (boinc_attached) {
        boinc_fraction_done(1.0);
        boinc_finish(0);
    }
    return 0;
}
