CXX = g++
CXXFLAGS = -O2 -w
OPENCV_FLAGS = $(shell pkg-config --cflags --libs opencv4)

SRC = $(wildcard code/*.cpp)
OBJ = $(patsubst code/%.cpp,exec/%.o,$(SRC))
BIN = $(patsubst code/%.cpp,bin/%,$(SRC))

.PHONY: all clean carpets
.SECONDARY: $(OBJ)

all: $(BIN)

bin/%: exec/%.o
	$(CXX) $(CXXFLAGS) $< -o $@ $(OPENCV_FLAGS)

exec/%.o: code/%.cpp | carpets
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(OPENCV_FLAGS)

carpets:
	mkdir -p bin bin/marcas exec

clean:
	rm -rf bin exec