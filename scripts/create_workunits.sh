#!/bin/bash
# Скрипт генерации и отправки workunits для проекта Latin Spectrum

PROJECT_DIR=~/projects/latin_spectrum
cd "$PROJECT_DIR" || exit 1

# Цикл по порядкам латинских квадратов от 6 до 20
for n in $(seq 6 20); do
  for i in $(seq 1 10); do
      SEED=$RANDOM$RANDOM
      TS=$(date +%s%N)              # наносекундный штамп
      INPUT_FILE="input_${n}_${i}_${TS}.txt"
      WU_NAME="LS_${n}_${i}_${TS}"
      echo "n $n"   > "$INPUT_FILE"
      echo "seed $SEED" >> "$INPUT_FILE"

      ./bin/stage_file "$INPUT_FILE"
      ./bin/create_work --appname LatinSpectrum --wu_name "$WU_NAME" "$INPUT_FILE"
  done
done

