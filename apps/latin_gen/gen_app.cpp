#include "boinc_api.h"
#include <vector>
#include <random>
#include <cstdio>

// Генерация случайного латинского квадрата порядка N
std::vector<std::vector<int>> randomLatinSquare(int N) {
    std::vector<std::vector<int>> a(N, std::vector<int>(N));
    std::mt19937 rng(boinc_time());
    std::vector<int> perm(N);
    for (int i = 0; i < N; i++) perm[i] = i;
    // начальная конструкция: символ i в позиции (i, *)
    for (int i = 0; i < N; i++) shuffle(perm.begin(), perm.end(), rng), a[i] = perm;
    return a;
}

int main(int argc, char **argv) {
    boinc_init();
    // Получаем N из init data (байт 0): 6 <= N <=20
    int N = 6 + (boinc_get_init_data_len()>0 ? boinc_get_init_data()[0] % 15 : 0);
    auto sq = randomLatinSquare(N);
    FILE *f = fopen("latin_square.txt", "w");
    fprintf(f, "%d\n", N);
    for (auto &row : sq) {
        for (int v : row) fprintf(f, "%d ", v);
        fprintf(f, "\n");
    }
    fclose(f);
    boinc_finish(0);
}