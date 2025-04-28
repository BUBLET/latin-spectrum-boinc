# Makefile – сборка приложения Latin Spectrum
CXX = g++
CXXFLAGS = -O2 -std=c++17 -I./include    # Оптимизация, стандарт C++17, путь к заголовкам
LIBS = -lboinc_api -lboinc -lpthread    # Ссылаемся на библиотеки BOINC и pthread

TARGET = latin_spectrum_1.0_x86_64-pc-linux-gnu
OBJS   = src/main.o src/LatinSquare.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LIBS)

src/%.o: src/%.cpp include/LatinSquare.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
