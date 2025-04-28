#include "boinc_api.h"
#include <vector>
#include <fstream>
#include <cstdio>
#include <cstdint>

// Подсчёт интерколятов
uint64_t countIntercalates(const std::vector<std::vector<int>>& a) {
    int N = a.size(); uint64_t cnt = 0;
    for (int i = 0; i < N; i++) for (int j = 0; j < N; j++)
    for (int k = i+1; k < N; k++) for (int l = j+1; l < N; l++)
        if (a[i][j]==a[i][l] && a[i][j]==a[k][j] && a[i][j]==a[k][l]) cnt++;
    return cnt;
}

// Подсчёт всех трансверсалей
uint64_t countTransversals(const std::vector<std::vector<int>>& a) {
    int N = a.size(); uint64_t cnt=0;
    std::vector<bool> usedCol(N), usedSym(N);
    std::function<void(int)> dfs = [&](int i) {
        if (i==N) { cnt++; return; }
        for (int j = 0; j < N; j++) {
            int v = a[i][j];
            if (!usedCol[j] && !usedSym[v]) {
                usedCol[j]=usedSym[v]=true;
                dfs(i+1);
                usedCol[j]=usedSym[v]=false;
            }
        }
    };
    dfs(0);
    return cnt;
}

// Подсчёт ортогональных квадратов-двойников
uint64_t countOrthogonal(const std::vector<std::vector<int>>& a) {
    int N = a.size(); uint64_t cnt=0;
    std::vector<std::vector<bool>> rowUsed(N, std::vector<bool>(N)),
                              colUsed(N, std::vector<bool>(N)),
                              pairUsed(N, std::vector<bool>(N*N));
    std::vector<int> B(N*N);
    std::function<void(int)> dfs = [&](int idx) {
        if (idx == N*N) { cnt++; return; }
        int i = idx / N, j = idx % N;
        for (int v = 0; v < N; v++) {
            int p = a[i][j]*N + v;
            if (!rowUsed[i][v] && !colUsed[j][v] && !pairUsed[a[i][j]][v]) {
                rowUsed[i][v]=colUsed[j][v]=pairUsed[a[i][j]][v]=true;
                B[idx]=v; dfs(idx+1);
                rowUsed[i][v]=colUsed[j][v]=pairUsed[a[i][j]][v]=false;
            }
        }
    };
    dfs(0);
    return cnt;
}

int main(int argc, char **argv) {
    boinc_init();
    std::ifstream in("latin_square.txt");
    int N; in >> N;
    std::vector<std::vector<int>> a(N, std::vector<int>(N));
    for (int i=0;i<N;i++) for (int j=0;j<N;j++) in>>a[i][j];

    boinc_fraction_done(0.0);
    uint64_t ic = countIntercalates(a);
    boinc_fraction_done(0.3);
    boinc_checkpoint();

    uint64_t tr = countTransversals(a);
    boinc_fraction_done(0.6);
    boinc_checkpoint();

    uint64_t ort = countOrthogonal(a);
    boinc_fraction_done(1.0);

    std::ofstream out("spectrum.txt");
    out << ic << " " << tr << " " << ort << "\n";
    out.close();
    boinc_finish(0);
}