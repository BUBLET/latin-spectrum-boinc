#!/bin/bash
PROJ=~/projects/latin_spectrum/project
cd "$PROJ"
# для каждого результата latin_gen создаём задачу для latin_compute
for d in results/latin_gen/*/; do
    if [[ -f "$d/latin_square.txt" ]]; then
        ../tools/create_work --app=latin_compute --input-file latin_square.txt:"$d/latin_square.txt"
    fi
done