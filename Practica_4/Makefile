CXX = g++
CXXFLAGS = -Wall -O2 -w
OPENCV_FLAGS = $(shell pkg-config --cflags --libs opencv4)

SRC = $(wildcard code/*.cpp)
BIN = $(patsubst code/%.cpp,bin/%,$(SRC))

.PHONY: all clean carpets

all: $(BIN)

# Garantiza que los directorios existen antes de compilar
bin/%: code/%.cpp | carpets
	$(CXX) $(CXXFLAGS) $^ -o $@ $(OPENCV_FLAGS)

carpets:
	mkdir -p bin bin/marcas

clean:
	rm -rf bin