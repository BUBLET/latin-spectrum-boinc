#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <map>
#include <boinc_api.h>     // Заголовок BOINC API
#include <util.h>         // (опционально, для boinc_finish)

const int MAXN = 21;      // Максимальный порядок (20) + запас

// Глобальные статические структуры для латинского квадрата
static int n;                                     // порядок квадрата
static int grid[MAXN][MAXN];                      // текущий частично заполняемый квадрат
static bool LatinRowUsed[MAXN][MAXN];             // LatinRowUsed[i][val] = true, если в строке i уже используется символ val
static bool LatinColUsed[MAXN][MAXN];             // LatinColUsed[j][val] = true, если в столбце j используется символ val

// Для подсчёта спектров
static unsigned long long totalSquares = 0ULL;    // число найденных латинских квадратов в рамках задания
static std::map<int, unsigned long long> intercalateDist;    // распределение по числу интеркалятов
static std::map<int, unsigned long long> orthogonalDiagDist; // распределение по числу ортогональных диагональных квадратов
static unsigned long long diagTransDist[3] = {0ULL, 0ULL, 0ULL}; // распределение по числу диагональных трансверсалей (индекс 0,1,2)

// Глобальные структуры для поиска ортогонального диагонального квадрата (M) к данному L
static bool ORowUsed[MAXN][MAXN];   // ORowUsed[i][val] = true, если в строке i квадрата M уже стоит символ val
static bool OColUsed[MAXN][MAXN];   // OColUsed[j][val] = true, если в столбце j квадрата M уже стоит символ val
static bool pairUsed[MAXN][MAXN];   // pairUsed[a][b] = true, если пара (символ L = a, символ M = b) уже использована (для ортогональности)
static bool OMainDiagUsed[MAXN];    // OMainDiagUsed[val] = true, если символ val уже использован на главной диагонали M
static bool OAntiDiagUsed[MAXN];    // OAntiDiagUsed[val] = true, если символ val уже использован на побочной диагонали M
static unsigned long long orthCount; // число найденных ортогональных диагональных квадратов для текущего L

// Рекурсивная функция поиска всех латинских квадратов M, ортогональных данному L и являющихся диагональными
void searchOrthDiag(int i, int j) {
    if (i == n) {
        // все n строк M заполнены – найден ортогональный диагональный квадрат M
        orthCount++;
        return;
    }
    if (j == n) {
        // переходим на следующую строку M
        searchOrthDiag(i + 1, 0);
        return;
    }
    // Подбираем символ для клетки (i,j) квадрата M
    for (int val = 1; val <= n; ++val) {
        if (ORowUsed[i][val] || OColUsed[j][val]) continue;       // символ уже есть в этой строке или столбце M
        int a = grid[i][j];                                       // символ квадрата L в этой позиции
        if (pairUsed[a][val]) continue;                           // пара (a, val) уже встречалась (нарушение ортогональности)
        bool mainDiag = (i == j);
        bool antiDiag = (i + j == n - 1);
        if (mainDiag && OMainDiagUsed[val]) continue;             // для главной диагонали: символ val уже использован на диагонали M
        if (antiDiag && OAntiDiagUsed[val]) continue;             // для побочной диагонали
        // Применяем символ val в M и отмечаем ограничения
        ORowUsed[i][val] = true;
        OColUsed[j][val] = true;
        pairUsed[a][val] = true;
        bool oldMainFlag = false, oldAntiFlag = false;
        if (mainDiag) { oldMainFlag = OMainDiagUsed[val]; OMainDiagUsed[val] = true; }
        if (antiDiag) { oldAntiFlag = OAntiDiagUsed[val]; OAntiDiagUsed[val] = true; }
        // Рекурсия к следующей ячейке
        searchOrthDiag(i, j + 1);
        // Откат (backtrack)
        ORowUsed[i][val] = false;
        OColUsed[j][val] = false;
        pairUsed[a][val] = false;
        if (mainDiag) OMainDiagUsed[val] = oldMainFlag;
        if (antiDiag) OAntiDiagUsed[val] = oldAntiFlag;
    }
}

// Функция подсчёта показателей для полностью заполненного латинского квадрата grid
void computeMetrics() {
    // 1. Число интеркалятов (2x2 латинских подквадратов)
    int interCount = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {        // перебор пар различных строк i, j
            for (int k = 0; k < n; ++k) {
                for (int l = k + 1; l < n; ++l) { // перебор пар различных столбцов k, l
                    int a = grid[i][k];
                    int b = grid[i][l];
                    int c = grid[j][k];
                    int d = grid[j][l];
                    // условие интеркаляции: a == d, b == c и при этом a != b (две пары противоположных углов совпадают, образуя мини-латинский квадрат 2x2)
                    if (a == d && b == c && a != b) {
                        interCount++;
                    }
                }
            }
        }
    }

    // 2. Число диагональных трансверсалей
    bool mainDiagOK = true;
    bool antiDiagOK = true;
    {
        bool seen[MAXN] = {false};
        // главная диагональ (ячейки (0,0), (1,1), ..., (n-1,n-1))
        for (int i = 0; i < n; ++i) {
            int val = grid[i][i];
            if (seen[val]) { mainDiagOK = false; break; }
            seen[val] = true;
        }
    }
    {
        bool seen[MAXN] = {false};
        // побочная диагональ (ячейки (0,n-1), (1,n-2), ..., (n-1,0))
        for (int i = 0; i < n; ++i) {
            int val = grid[i][n - 1 - i];
            if (seen[val]) { antiDiagOK = false; break; }
            seen[val] = true;
        }
    }
    int diagTransCount = (mainDiagOK ? 1 : 0) + (antiDiagOK ? 1 : 0);

    // 3. Число ортогональных диагональных латинских квадратов
    int orthDiagCount = 0;
    if (n == 6) {
        // Известно: для n=6 ортогональных пар латинских квадратов не существует (магический квадрат Эйлера),
        // следовательно для порядка 6 можно сразу поставить 0.
        orthDiagCount = 0;
    } else {
        // Инициализация структур и запуск поиска всех подходящих квадратов M
        memset(ORowUsed, 0, sizeof(ORowUsed));
        memset(OColUsed, 0, sizeof(OColUsed));
        memset(pairUsed, 0, sizeof(pairUsed));
        memset(OMainDiagUsed, 0, sizeof(OMainDiagUsed));
        memset(OAntiDiagUsed, 0, sizeof(OAntiDiagUsed));
        orthCount = 0ULL;
        searchOrthDiag(0, 0);
        orthDiagCount = (int)orthCount;
    }

    // Обновляем глобальные счётчики распределений
    totalSquares++;
    intercalateDist[interCount] += 1ULL;
    diagTransDist[diagTransCount] += 1ULL;
    orthogonalDiagDist[orthDiagCount] += 1ULL;
}

// Рекурсивная функция для перебора латинского квадрата (с учётом фиксированных элементов)
void searchLatin(int i, int j) {
    if (i == n) {
        // латинский квадрат полностью заполнен – вычисляем показатели
        computeMetrics();
        return;
    }
    if (j == n) {
        // достигли конца строки – переходим на следующую строку
        searchLatin(i + 1, 1);  // пропускаем столбец 0, т.к. он фиксирован для i>=1
        return;
    }
    if (i == 0) {
        // первую строку полностью пропускаем (она уже фиксирована)
        searchLatin(1, 1);
        return;
    }
    if (j == 0) {
        // первый столбец пропускаем (он фиксирован для всех i)
        searchLatin(i, 1);
        return;
    }
    if (grid[i][j] != 0) {
        // если ячейка (i,j) заранее заполнена частичной фиксацией – пропускаем её
        searchLatin(i, j + 1);
        return;
    }
    // Перебираем возможные символы 1..n для ячейки (i,j)
    for (int val = 1; val <= n; ++val) {
        if (LatinRowUsed[i][val] || LatinColUsed[j][val]) continue; // уже встречался в этой строке или столбце
        // Присваиваем значение и отмечаем использование
        grid[i][j] = val;
        LatinRowUsed[i][val] = true;
        LatinColUsed[j][val] = true;
        // Рекурсивно заполняем следующую ячейку
        searchLatin(i, j + 1);
        // Откат назначения (backtracking)
        LatinRowUsed[i][val] = false;
        LatinColUsed[j][val] = false;
        grid[i][j] = 0;
    }
}

int main(int argc, char** argv) {
    BOINC_OPTIONS boinc_opts;
    boinc_opts.normal_thread_priority = true;
    boinc_opts.multi_thread = false;
    boinc_opts.check_heartbeat = true;
    boinc_opts.handle_trickle_up = true;
    boinc_opts.handle_trickle_down = true;
    boinc_opts.handle_process_control = true;
    boinc_opts.send_status_msgs = true;
    boinc_init_options(&boinc_opts);  // Инициализация BOINC

    // Открываем входной файл (имя задано в шаблоне как "in.txt")
    FILE* fin = boinc_fopen("in.txt", "r");
    if (!fin) {
        fprintf(stderr, "ERROR: input file not found\n");
        boinc_finish(-1);
        return 0;
    }
    // Читаем порядок n и (при наличии) заданные фиксированные значения
    std::vector<int> partialVals;
    if (fscanf(fin, "%d", &n) != 1) {
        fprintf(stderr, "ERROR: failed to read n\n");
        fclose(fin);
        boinc_finish(-1);
        return 0;
    }
    // Считываем оставшиеся числа (если есть)
    int pv;
    while (fscanf(fin, "%d", &pv) == 1) {
        partialVals.push_back(pv);
    }
    fclose(fin);

    // Инициализируем структуры для построения квадрата
    memset(grid, 0, sizeof(grid));
    memset(LatinRowUsed, 0, sizeof(LatinRowUsed));
    memset(LatinColUsed, 0, sizeof(LatinColUsed));
    totalSquares = 0ULL;
    intercalateDist.clear();
    orthogonalDiagDist.clear();
    diagTransDist[0] = diagTransDist[1] = diagTransDist[2] = 0ULL;

    // Фиксируем первую строку (1,2,...,n)
    for (int j = 0; j < n; ++j) {
        int val = j + 1;
        grid[0][j] = val;
        LatinRowUsed[0][val] = true;
        LatinColUsed[j][val] = true;
    }
    // Фиксируем первый столбец (1,2,...,n) для строк 1..n-1 (индексы 1..n-1)
    for (int i = 1; i < n; ++i) {
        int val = i + 1;
        grid[i][0] = val;
        LatinRowUsed[i][val] = true;
        LatinColUsed[0][val] = true;
    }
    // Применяем частичную фиксацию (разделение задачи):
    if (!partialVals.empty()) {
        if (partialVals.size() >= 1) {
            // фиксируем значение в клетке (2,2) – индекс [1][1] (если передано хотя бы одно значение)
            int a = partialVals[0];
            grid[1][1] = a;
            LatinRowUsed[1][a] = true;
            LatinColUsed[1][a] = true;
        }
        if (partialVals.size() >= 2) {
            // фиксируем значение в клетке (3,2) – индекс [2][1] (если передано второе значение)
            int b = partialVals[1];
            grid[2][1] = b;
            LatinRowUsed[2][b] = true;
            LatinColUsed[1][b] = true;
        }
        // (при необходимости можно расширить для большего числа фиксированных позиций)
    }

    // Запуск перебора латинского квадрата
    searchLatin(1, 1);  // начинаем со строки 2 (индекс 1), столбца 2 (индекс 1), т.к. первая строка и первый столбец зафиксированы

    // Запись результатов в выходной файл
    FILE* fout = fopen("output.txt", "w");
    if (!fout) {
        fprintf(stderr, "ERROR: cannot open output file\n");
        boinc_finish(-1);
        return 0;
    }
    fprintf(fout, "Total Latin squares: %llu\n", totalSquares);
    // Распределение интеркалятов
    fprintf(fout, "Intercalates distribution:\n");
    for (auto it = intercalateDist.begin(); it != intercalateDist.end(); ++it) {
        fprintf(fout, "  %d: %llu\n", it->first, it->second);
    }
    // Распределение диагональных трансверсалей
    fprintf(fout, "Diagonal transversals distribution:\n");
    for (int t = 0; t <= 2; ++t) {
        if (diagTransDist[t] > 0ULL) {
            fprintf(fout, "  %d: %llu\n", t, diagTransDist[t]);
        }
    }
    // Распределение ортогональных диагональных латинских квадратов
    fprintf(fout, "Orthogonal diagonal Latin squares distribution:\n");
    for (auto it = orthogonalDiagDist.begin(); it != orthogonalDiagDist.end(); ++it) {
        fprintf(fout, "  %d: %llu\n", it->first, it->second);
    }
    fclose(fout);

    // Завершение приложения
    boinc_finish(0);
    return 0;
}
