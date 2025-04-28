#!/bin/bash
# Скрипт генерации и отправки workunits для проекта Latin Spectrum

PROJECT_DIR=~/projects/latin_spectrum
cd "$PROJECT_DIR" || exit 1

# Цикл по порядкам латинских квадратов от 6 до 20
for n in $(seq 6 20); do
    # Для каждого порядка создаем 10 задач с разными случайными seed
    for i in $(seq 1 10); do
        SEED=$RANDOM$RANDOM  # генерируем случайное большое число из двух RANDOM
        INPUT_FILE="input_${n}_${i}.txt"
        echo "n $n" > "$INPUT_FILE"
        echo "seed $SEED" >> "$INPUT_FILE"
        # Размещаем входной файл в директории загрузки BOINC-сервера
        ./bin/stage_file "$INPUT_FILE"
        # Регистрируем новое задание (workunit) с указанным входным файлом
        ./bin/create_work --appname LatinSpectrum --wu_name "LS_${n}_${i}" "$INPUT_FILE"
    done
done
