// LatinSquare.h
#ifndef LATINSQUARE_H
#define LATINSQUARE_H

#include <vector>
#include <tuple>
#include <random>

class LatinSquare {
public:
    explicit LatinSquare(int n);

    // Генерация: инициализация + рандомизация символов, строк и столбцов
    bool generateRandom(unsigned int seed);

    // Подсчёт интеркаляций (2×2 подсубквадраты)
    int countIntercalates() const;

    // Точный бэктрекинг подсчёта трансверсалей (все перестановки столбцов)
    int countTransversalsExact() const;

    // Эвристический (с ограничением операций) подсчёт трансверсалей
    // opsPerformed возвращает реальное число проверенных размещений
    int countTransversalsHeuristic(int maxOps, long &opsPerformed) const;

    // Поворот случайной интеркаляции
    bool rotateRandomIntercalate(std::mt19937 &gen);

private:
    int n;
    std::vector<std::vector<int>> grid;

    // Поиск всех позиций интеркаляций
    std::vector<std::tuple<int,int,int,int>> findIntercalatePositions() const;

    // Вспомогательный бэктрекинг для точного подсчёта трансверсалей
    int backtrackTrans(int row,
                       std::vector<bool> &usedCols,
                       std::vector<bool> &usedSyms) const;
};

#endif // LATINSQUARE_H
