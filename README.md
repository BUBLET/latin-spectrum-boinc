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
