CXX = g++

CXXFLAGS = -O2 -std=c++17 -I/home/bulat/boinc/api -I/home/bulat/boinc -I/home/bulat/boinc/lib -I./include

LDFLAGS = -L/home/bulat/boinc/lib   # Путь для библиотеки boinc
LIBS = -L/home/bulat/boinc/api -lboinc_api -L/home/bulat/boinc/lib -lboinc -lpthread  # Линковка с библиотеками BOINC и pthread

TARGET = latin_spectrum

OBJS   = src/compute_app.o src/LatinSquare.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS) $(LIBS)

src/%.o: src/%.cpp include/LatinSquare.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
