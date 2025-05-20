#!/usr/bin/env bash
set -euo pipefail

# generate_and_submit_wus.sh — берёт все параметры из конфигурационного файла wus.conf
# Разместите рядом с этим скриптом файл wus.conf со следующими переменными:
#   PROJECT_DIR    — путь к папке проекта, например: ~/projects/latin_spectrum
#   APPNAME        — имя приложения в BOINC, например: LatinSpectrum
#   RESTARTS       — общее число рестартов (seeds), например: 100
#   MOVES_PER      — число локальных поворотов, например: 5
#   MAX_TRANS_OPS  — лимит операций для эвристического подсчёта трансверсалей, например: 200000
#   NUM_TASKS      — на сколько задач разбить рестарты, например: 10
#   N_START        — начальный порядок квадрата, например: 6
#   N_END          — конечный порядок квадрата, например: 20

# Определяем директорию скрипта и конфиг
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CONFIG="$SCRIPT_DIR/wus.conf"

if [ ! -f "$CONFIG" ]; then
  echo "Ошибка: не найден конфиг $CONFIG" >&2
  exit 1
fi
source "$CONFIG"

: "${PROJECT_DIR:?Не задан PROJECT_DIR в $CONFIG}"
: "${APPNAME:?Не задан APPNAME в $CONFIG}"
: "${RESTARTS:?Не задан RESTARTS в $CONFIG}"
: "${MOVES_PER:?Не задан MOVES_PER в $CONFIG}"
: "${MAX_TRANS_OPS:?Не задан MAX_TRANS_OPS в $CONFIG}"
: "${NUM_TASKS:?Не задан NUM_TASKS в $CONFIG}"
: "${N_START:?Не задан N_START в $CONFIG}"
: "${N_END:?Не задан N_END в $CONFIG}"

cd "$PROJECT_DIR" || { echo "Не удалось зайти в $PROJECT_DIR"; exit 1; }

for n in $(seq "$N_START" "$N_END"); do
  for task_id in $(seq 0 $((NUM_TASKS - 1))); do
    TS=$(date +%s%N)
    INPUT_FILE="in_${n}_${task_id}_${TS}.txt"
    WU_NAME="LS_${n}_${task_id}_${TS}"

    # Генерация входного файла
    cat > "$INPUT_FILE" <<EOF
n $n
task_id $task_id
num_tasks $NUM_TASKS
restarts $RESTARTS
moves_per $MOVES_PER
maxTransOps $MAX_TRANS_OPS
EOF
    echo "[+] Создан $INPUT_FILE"
    ./bin/stage_file "$INPUT_FILE"
    ./bin/create_work \
      --appname "$APPNAME" \
      --wu_name "$WU_NAME" \
      "$INPUT_FILE"
    echo "[>] Отправлен workunit $WU_NAME"
  done
done

TOTAL=$(( (N_END - N_START + 1) * NUM_TASKS ))
echo "Генерация и отправка завершены: $TOTAL workunit’ов создано."
