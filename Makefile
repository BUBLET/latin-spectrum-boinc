# Указание компилятора
CXX = g++

# Флаги компиляции
CXXFLAGS = -O2 -std=c++17 -I/home/bulat/boinc/api -I/home/bulat/boinc -I/home/bulat/boinc/lib -I./include

# Флаги для линковки
LDFLAGS = -L/home/bulat/boinc/lib   # Путь для библиотеки boinc
LIBS = -L/home/bulat/boinc/api -lboinc_api -L/home/bulat/boinc/lib -lboinc -lpthread  # Линковка с библиотеками BOINC и pthread

# Целевой файл
TARGET = latin_calc

# Объектные файлы
OBJS   = src/compute_app.o src/LatinSquare.o

# Правила сборки
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS) $(LIBS)

src/%.o: src/%.cpp include/LatinSquare.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Очистка
clean:
	rm -f $(OBJS) $(TARGET)
