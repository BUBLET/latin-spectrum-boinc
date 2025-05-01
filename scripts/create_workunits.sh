#!/usr/bin/env bash
set -euo pipefail

PROJECT_DIR=~/projects/latin_spectrum
cd "$PROJECT_DIR" || { echo "Не удалось зайти в $PROJECT_DIR"; exit 1; }

restarts=100        # общее число рестартов
moves_per=5         # число локальных поворотов
maxTransOps=200000  # лимит операций эвристического счёта трансверсалей
num_tasks=10        # на сколько задач разбить рестарты

for n in $(seq 6 20); do
  for task_id in $(seq 0 $((num_tasks - 1))); do
    TS=$(date +%s%N)
    INPUT_FILE="in_${n}_${task_id}_${TS}.txt"
    WU_NAME="LS_${n}_${task_id}_${TS}"

    cat > "$INPUT_FILE" <<EOF
n $n
task_id $task_id
num_tasks $num_tasks
restarts $restarts
moves_per $moves_per
maxTransOps $maxTransOps
EOF
    echo "[+] Создан $INPUT_FILE"

    ./bin/stage_file "$INPUT_FILE"
    ./bin/create_work \
      --appname LatinSpectrum \
      --wu_name "$WU_NAME" \
      "$INPUT_FILE"
    echo "[>] Отправлен workunit $WU_NAME"
  done
done

echo "Генерация и отправка завершены: $(seq 6 20 | wc -l)×$num_tasks workunits создано."
