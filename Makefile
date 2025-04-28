# Указание компилятора
CXX = g++

# Флаги компиляции
CXXFLAGS = -O2 -std=c++17 \
           -I/home/bulat/boinc/api \
           -I/home/bulat/boinc \
           -I/home/bulat/boinc/lib

# Флаги для линковки
LDFLAGS = -L/home/bulat/boinc/lib
LIBS = -lboinc_api -lboinc -lpthread

# Целевой файл
TARGET = latin_spectrum_1.0_x86_64-pc-linux-gnu

# Объектные файлы
OBJS   = src/main.o src/LatinSquare.o

# Правила сборки
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LIBS)

src/%.o: src/%.cpp include/LatinSquare.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Очистка
clean:
	rm -f $(OBJS) $(TARGET)
