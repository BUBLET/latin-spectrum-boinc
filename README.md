# Latin Spectrum BOINC

## Общий обзор

`latin-spectrum-boinc` — BOINC приложения для вычисления спектров латинских квадратов

## Структура

```
latin-spectrum-boinc/
├── include/               # Заголовочные файлы и описания API
│   └── LatinSquare.h      # Представления латинских квадратов и вспомогательные функции
├── src/                   # Основные модули вычислений
│   ├── compute_app.cpp    # Точка входа BOINC-клиентского приложения
│   └── LatinSquare.cpp    # Реализация методов 
├── scripts/               # Вспомогательные скрипты
│   ├── create_workunits.sh   # Генерация входных файлов задач (workunit) для сервера BOINC
│   └── wus.conf   # Конфиг создания workunit
├── app_info.xml
├── Makefile # Скрипт генерации бинарника
└── README.md
```
## Логика клиентского приложения (`compute_app`)


1. **Чтение параметров**  
Приложение получает из входного файла (`in`) набор ключ-значение:
```cpp
   n             — порядок латинского квадрата
   task_id       — номер этой части работы (0…num_tasks–1)
   num_tasks     — всего частей работы
   restarts      — число случайных рестартов
   moves_per     — число случайных локальных преобразований на рестарт
   maxTransOps   — лимит операций при эвристическом подсчёте транзверсалей
```
Эти параметры управляют разбиением пространства поиска и глубиной эвристики.

2. **Подготовка генератора случайных чисел**
Для каждого рестарта формируется своё зерно:

```cpp
unsigned int seed = task_id + restart_index * num_tasks;
std::mt19937 gen(seed);
```
Таким образом разные клиентские задачи и рестарты работают на разных подпространствах.

3. **Генерация начального латинского квадрата**
```cpp
LatinSquare ls(n);
ls.generateRandom(seed);
```
Если seed == 0, квадрат остаётся «базовым» ( (i+j)%n ).
Иначе происходит:
+ перестановка символов по случайному perm
+ случайная перестановка строк
+ случайная перестановка элементов в каждой строке

4. **Эвристические локальные преобразования**
Повторить moves_per раз:
```cpp
ls.rotateRandomIntercalate(gen);
```
+ findIntercalatePositions() ищет все пары строк и столбцов (i,j,c,d), образующие интеркальты.
+ Из них случайно выбирается один, и в нём меняются местами диагональные элементы:
```cpp
swap(grid[i][c], grid[i][d]);
swap(grid[j][c], grid[j][d]);
```
5. **Подсчёт характеристик**
+ Число интеркалятов:
```cpp
int intercalates = ls.countIntercalates();
```
+ Трансверсали — точный или эвристический подсчёт
Точный (полный бэктрекинг):
```cpp
int t_exact = ls.countTransversalsExact();
```
Эвристический (с ограничением maxTransOps):
```cpp
long opsPerformed = 0;
int t_heur = ls.countTransversalsHeuristic(maxTransOps, opsPerformed);
```
Если число рекурсивных вызовов (opsPerformed) превысило maxTransOps, поиск прерывается и возвращается частичный результат.

6. **Агрегация результатов по рестартам**
Внутри одного work-unit результаты каждого рестарта накапливаются:
+ суммарное число найденных трансверсалей
+ суммарное число интеркаляток
+ фактическое число выполненных операций

7. **Запись результата**
По завершении всех рестартов клиент формирует XML:
```
<results>
  <stat
    n="7"
    task_id="42"
    total_restarts="100"
    total_intercalates="123456"
    total_transversals="7890"
    ops_performed="4567890"/>
</results>
```
Затем BOINC автоматически передаёт этот файл на сервер. 

## Установка на локальный сервер BOINC
Соберите BOINC сервер в соответствии с официальной инструкцией
```
https://github.com/BOINC/boinc/wiki/Create-a-BOINC-server-(cookbook)
```
Клонируйте репозиторий:
```
git clone https://github.com/BUBLET/latin-spectrum-boinc
```
Соберите приложение:
```
cd ~/latin-spectrum-boinc
make
```
Перейдите в папку с вашим проектом и скопируйте файлы с описанием входных и выходных файлов приложения в папку templates
```
cd ~/projects/<имя проекта>
cp ~/latin-spectrum-boinc/latin_spectrum_in templates
cp ~/latin-spectrum-boinc/latin_spectrum_out templates
```
Добавьте следующее в ~/projects/<имя проекта>/config.xml, в раздел <daemons>:
```
      <daemon>
          <cmd>sample_trivial_validator --app latin_spectrum</cmd>
      </daemon>
      <daemon>
          <cmd>sample_assimilator --app latin_spectrum</cmd>
      </daemon>
```
В Firefox перейдите на веб-страницу администратора: http://<адрес проекта>/<имя проекта>_ops. Нажмите «Manage applications». Добавьте приложение с именем "Latin Spectrum".     
Создайте следующую структуру каталогов:
```
projects/
    <имя проекта>/
        apps/
            latin_spectrum/
                1.0/
                    x86_64-pc-linux-gnu/
```
Затем переместите собранный исполняемый файл в эту директорию.
```
cd ~/projects<имя проекта>/apps/latin_spectrum/1.0/x86_64-pc-linux-gnu
cp ~/latin-spectrum-boinc/latin_spectrum .
cp ~/boinc/apps/version.xml .
```
Обновите версию приложения
```
cd ~/projects/<имя проекта>
bin/update_versions
```
Перезапустите демоны
```
cd ~/projects/<имя проекта>
bin/stop
bin/start
```
## Генерация и отправка Work-Unit

### Подготовка конфигурации (`wus.conf`)

Настройте .wus.conf со следующим содержимым:

```
# Путь к корню проекта
PROJECT_DIR=~/projects/latin_spectrum 

# Имя BOINC-приложения
APPNAME="LatinSpectrum" # название приложения, как оно зарегистрировано в BOINC.

# Общее число рестартов (seeds)
RESTARTS=100

# Число локальных поворотов на рестарт
MOVES_PER=5

# Лимит операций при проверке транзверсалей
MAX_TRANS_OPS=200000

# На сколько задач разбить рестарты
NUM_TASKS=10

# Диапазон порядков квадратов
N_START=6
N_END=20
```

### Запуск скрипта create_workunits.sh
Перейдите в корень проекта и скопируйте файлы .wus.conf и create_workunits.sh:

```
cd ~/projects/<имя проекта>
cp ~/latin-spectrum-boinc/scripts .
```
Сделайте скрипт исполняемым:
```
chmod +x create_workunits.sh
```
Запустите его:

```
./scripts/create_workunits.sh
```
Скрипт:
+ прочитает переменные из wus.conf 
+ для каждого порядка n и каждого task_id 
+ создаст входной файл in_<n>_<task_id>_<TIMESTAMP>.txt; 
+ автоматически отправит каждый work-unit на сервер BOINC;
+ выведет итоговое число созданных задач.

После успешного запуска в консоли появятся сообщения о создании и отправке каждого work-unit, а в BOINC-сервере вы увидите новые задачи в статусе «in progress».

Перейдите в BOINC Manager. На вкладке Projects выберите ваш проект и нажмите кнопку Update. Перейдите на вкладку Tasks и убедитесь, что задание запущено.
