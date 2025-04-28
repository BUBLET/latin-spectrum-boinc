# Указание компилятора
CXX = g++

# Флаги компиляции
CXXFLAGS = -O2 -std=c++17 \
           -I/home/bulat/boinc/api \  # Путь для boinc_api.h
           -I/home/bulat/boinc       # Путь для config.h
           -I/home/bulat/boinc/lib   # Путь для filesys.h и mfile.h

# Флаги для линковки
LDFLAGS = -L/home/bulat/boinc/lib   # Путь для библиотек
LIBS = -lboinc_api -lboinc -lpthread  # Линковка с библиотеками BOINC и pthread

# Целевой файл
TARGET = latin_spectrum_1.0_x86_64-pc-linux-gnu

# Объектные файлы
OBJS   = src/main.o src/LatinSquare.o

# Правила сборки
all: $(TARGET)

$(TARGET): $(OBJS)
<TAB>$(CXX) $(OBJS) -o $@ $(LIBS)

src/%.o: src/%.cpp include/LatinSquare.h
<TAB>$(CXX) $(CXXFLAGS) -c $< -o $@

# Очистка
clean:
<TAB>rm -f $(OBJS) $(TARGET)
