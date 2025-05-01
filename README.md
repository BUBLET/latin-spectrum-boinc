# Latin Spectrum BOINC

## Общий обзор

`latin-spectrum-boinc` — BOINC приложения для вычисления спектров латинских квадратов

## Структура

```plaintext
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
    
