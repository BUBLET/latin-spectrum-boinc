#include "LatinSquare.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <random>
#include "boinc_api.h"
#include "filesys.h"

using namespace std;

// Определяем глобальную матрицу S (объявлена в заголовке)
int S[MAXN][MAXN];

// Вспомогательные структуры для поиска ортогонального квадрата T
static int T_square[MAXN][MAXN];             // Матрица для формируемого ортогонального квадрата T
static bool usedRowT[MAXN][MAXN+1];          // usedRowT[r][val] = true, если val уже используется в строке r квадрата T
static bool usedColT[MAXN][MAXN+1];          // usedColT[c][val] = true, если val уже используется в столбце c квадрата T
static bool usedPair[MAXN+1][MAXN+1];        // usedPair[a][b] = true, если пара (значение a в S, значение b в T) уже встречалась

static int maxDepthReached = 0;             // Максимальная достигнутая глубина рекурсии (для отчета прогресса)

// Рекурсивная функция для поиска ортогонального квадрата T начиная с заданной ячейки (по индексу cellIndex).
// Возвращает true, если удается полностью заполнить квадрат T.
static bool solveOrthogonal(int n, int cellIndex) {
    if (cellIndex == n * n) {
        // Базовый случай: все ячейки заполнены – ортогональный квадрат найден
        return true;
    }
    int r = cellIndex / n;
    int c = cellIndex % n;
    if (T_square[r][c] != 0) {
        // Если эта ячейка уже зафиксирована (например, первая строка), переходим к следующей
        return solveOrthogonal(n, cellIndex + 1);
    }
    // Обновление прогресса: фиксируем максимальный достигнутый индекс
    if (cellIndex > maxDepthReached) {
        maxDepthReached = cellIndex;
        boinc_fraction_done((double)cellIndex / (double)(n * n));
    }
    // Перебираем возможные значения для ячейки (r, c) в квадрате T
    for (int val = 1; val <= n; ++val) {
        if (!usedRowT[r][val] && !usedColT[c][val] && !usedPair[ S[r][c] ][ val ]) {
            // Пробуем поставить значение val в T[r][c]
            T_square[r][c] = val;
            usedRowT[r][val] = true;
            usedColT[c][val] = true;
            usedPair[ S[r][c] ][ val ] = true;

            // Чекпоинт: при необходимости сохраняем состояние
            if (boinc_time_to_checkpoint()) {
                saveCheckpoint(n, cellIndex);
                boinc_checkpoint_completed();
            }

            // Рекурсия на следующую ячейку
            if (solveOrthogonal(n, cellIndex + 1)) {
                return true;  // найдено решение, возвращаем успех вверх по рекурсии
            }

            // Откат (backtrack), если дальнейшее заполнение не удалось
            T_square[r][c] = 0;
            usedRowT[r][val] = false;
            usedColT[c][val] = false;
            usedPair[ S[r][c] ][ val ] = false;
        }
    }
    // Если ни одно значение не подошло в эту ячейку, возвращаем неуспех для текущего ветвления
    return false;
}

void generateLatinSquare(int n, unsigned int seed) {
    // Создаем исходный латинский квадрат S: базовая циклическая структура (таблица сложения по модулю n)
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            S[r][c] = ((r + c) % n) + 1;
        }
    }
    if (seed != 0) {
        // Перемешиваем квадрат S случайным образом с использованием заданного seed
        mt19937 gen(seed);
        vector<int> permSym(n+1), permRows(n), permCols(n);
        for (int i = 1; i <= n; ++i) permSym[i] = i;
        for (int i = 0; i < n; ++i) {
            permRows[i] = i;
            permCols[i] = i;
        }
        shuffle(permSym.begin()+1, permSym.begin()+1+n, gen);
        shuffle(permRows.begin(), permRows.end(), gen);
        shuffle(permCols.begin(), permCols.end(), gen);
        // Применяем перестановки к базовому квадрату
        static int temp[MAXN][MAXN];
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < n; ++c) {
                int oldVal = S[ permRows[r] ][ permCols[c] ];
                temp[r][c] = permSym[ oldVal ];
            }
        }
        // Копируем перемешанный квадрат обратно в S
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < n; ++c) {
                S[r][c] = temp[r][c];
            }
        }
    }
}

int countIntercalates(int n) {
    int count = 0;
    // Перебираем все пары строк и столбцов для поиска 2x2 подквадратов
    for (int r1 = 0; r1 < n; ++r1) {
        for (int r2 = r1 + 1; r2 < n; ++r2) {
            for (int c1 = 0; c1 < n; ++c1) {
                for (int c2 = c1 + 1; c2 < n; ++c2) {
                    int a = S[r1][c1];
                    int b = S[r1][c2];
                    int c = S[r2][c1];
                    int d = S[r2][c2];
                    // Интерколят: a == d, b == c, при этом a != b
                    if (a == d && b == c && a != b) {
                        count++;
                    }
                }
            }
        }
    }
    return count;
}

int countDiagonalTransversals(int n) {
    int count = 0;
    bool mainDiag = true;
    bool antiDiag = true;
    vector<bool> seen(n+1);
    // Проверяем главную диагональ
    fill(seen.begin(), seen.end(), false);
    for (int i = 0; i < n; ++i) {
        if (seen[ S[i][i] ]) {
            mainDiag = false;
            break;
        }
        seen[ S[i][i] ] = true;
    }
    // Проверяем побочную диагональ
    fill(seen.begin(), seen.end(), false);
    for (int i = 0; i < n; ++i) {
        int j = n - 1 - i;
        if (seen[ S[i][j] ]) {
            antiDiag = false;
            break;
        }
        seen[ S[i][j] ] = true;
    }
    if (mainDiag) count++;
    if (antiDiag) count++;
    return count;
}

int loadCheckpoint(int n) {
    // Загружаем сохраненное состояние из checkpoint_state.txt, если файл существует
    string checkpointFile;
    if (boinc_resolve_filename_s("checkpoint_state.txt", checkpointFile)) {
        return 0;  // не удалось определить имя файла (вероятно, BOINC не инициализирован)
    }
    ifstream fin(checkpointFile);
    if (!fin) {
        return 0;  // файл чекпоинта отсутствует
    }
    int resumeIndex = 0;
    fin >> resumeIndex;
    // Считываем частично заполненный квадрат T
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            fin >> T_square[r][c];
        }
    }
    fin.close();
    // Восстанавливаем структуру использованных элементов
    memset(usedRowT, 0, sizeof(usedRowT));
    memset(usedColT, 0, sizeof(usedColT));
    memset(usedPair, 0, sizeof(usedPair));
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            int val = T_square[r][c];
            if (val != 0) {
                usedRowT[r][val] = true;
                usedColT[c][val] = true;
                usedPair[ S[r][c] ][ val ] = true;
            }
        }
    }
    // Восстанавливаем прогресс
    maxDepthReached = resumeIndex;
    boinc_fraction_done((double)resumeIndex / (double)(n * n));
    return resumeIndex;
}

void saveCheckpoint(int n, int currentIndex) {
    // Сохраняем текущее состояние поиска в файл checkpoint_state.txt
    string checkpointFile;
    if (boinc_resolve_filename_s("checkpoint_state.txt", checkpointFile)) {
        return;
    }
    FILE* f = boinc_fopen(checkpointFile.c_str(), "w");
    if (!f) return;
    // Записываем индекс следующей ячейки для продолжения
    fprintf(f, "%d\n", currentIndex);
    // Записываем текущее состояние квадрата T (0 обозначает незаполненную ячейку)
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            fprintf(f, "%d ", T_square[r][c]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

bool findOrthogonalMate(int n) {
    // Инициализируем структуры перед поиском
    memset(T_square, 0, sizeof(T_square));
    memset(usedRowT, 0, sizeof(usedRowT));
    memset(usedColT, 0, sizeof(usedColT));
    memset(usedPair, 0, sizeof(usedPair));
    maxDepthReached = 0;
    // Проверяем, не было ли ранее прерванного расчета
    int startIndex = loadCheckpoint(n);
    if (startIndex == 0) {
        // Начинаем новый поиск: фиксируем первую строку T как (1,2,...,n) для устранения тривиальных симметрий
        for (int c = 0; c < n; ++c) {
            int val = c + 1;
            T_square[0][c] = val;
            usedRowT[0][val] = true;
            usedColT[c][val] = true;
            usedPair[ S[0][c] ][ val ] = true;
        }
        startIndex = n;  // пропускаем первую строку
        maxDepthReached = startIndex;
        boinc_fraction_done((double)startIndex / (double)(n * n));
    } else {
        fprintf(stderr, "Resuming orthogonal search from index %d...\n", startIndex);
    }
    // Запускаем рекурсивный поиск с найденной или начальной точки
    bool found = solveOrthogonal(n, startIndex);
    return found;
}
