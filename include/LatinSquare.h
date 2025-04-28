#ifndef LATIN_SQUARE_H
#define LATIN_SQUARE_H

#include <vector>
#include <string>

#define MAXN 20

// Глобальная матрица латинского квадрата S (значения от 1 до n)
extern int S[MAXN][MAXN];

// Генерация латинского квадрата порядка n с использованием заданного начального зерна (seed).
void generateLatinSquare(int n, unsigned int seed);

// Подсчет количества интерколятов (2x2 латинских подквадратов) в квадрате S.
int countIntercalates(int n);

// Подсчет числа диагональных трансверсалей (главная и побочная диагонали).
int countDiagonalTransversals(int n);

// Поиск ортогонального латинского квадрата (T) к заданному S.
// Возвращает true, если найден хотя бы один ортогональный квадрат.
bool findOrthogonalMate(int n);

// Загрузка состояния из файла чекпоинта (если существует). Возвращает индекс, с которого продолжить.
int loadCheckpoint(int n);

// Сохранение текущего состояния поиска ортогонального квадрата в файл чекпоинта.
void saveCheckpoint(int n, int currentIndex);

#endif // LATIN_SQUARE_H
