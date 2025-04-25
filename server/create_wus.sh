#!/bin/bash
cd ~/projects/latin   # Перейти в каталог проекта BOINC

# Цикл по порядкам латинских квадратов от 6 до 20
for ((n=6; n<=20; ++n)); do
  # Перебор возможного значения a = L(2,2) (ячейка 2-й строки, 2-й столбец) 
  for ((a=1; a<=n; ++a)); do
    # Пропустить a=2, так как в строке 2 уже стоит 2 (первый столбец фиксирован как 2)
    if [[ $a -eq 2 ]]; then continue; fi
    # Перебор возможного значения b = L(3,2) (ячейка 3-й строки, 2-й столбец)
    for ((b=1; b<=n; ++b)); do
      # В 3-й строке первый столбец =3, поэтому пропустить b=3; также пропустить b=2 (уже в этом столбце)
      if [[ $b -eq 2 || $b -eq 3 ]]; then continue; fi
      # Значения a и b в одном столбце 2 не должны совпадать
      if [[ $a -eq $b ]]; then continue; fi

      # Сформировать имя рабочего задания (уникальное)
      wu_name="latin_${n}_${a}_${b}"
      # Создать входной файл с параметрами (порядок n и значения a,b)
      echo -e "${n}\n${a} ${b}" > "input_${n}_${a}_${b}.txt"
      # Зарегистрировать задание в БД и поставить в очередь на рассылку
      ./bin/create_work \
        -appname latin_spectrum \
        -wu_name "$wu_name" \
        -wu_template templates/latin_wu.xml \
        -result_template templates/latin_result.xml \
        "input_${n}_${a}_${b}.txt"
      echo "Created workunit $wu_name (n=$n, a=$a, b=$b)"
    done
  done
done
