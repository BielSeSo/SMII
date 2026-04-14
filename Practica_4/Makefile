CC = g++
OPENCV_FLAGS = $(shell pkg-config --cflags --libs opencv4) -lopencv_calib3d

SRC = $(wildcard code/*.cpp)
BIN = $(patsubst code/%.cpp,bin/%,$(SRC))

all: carpets $(BIN) 

bin/%: code/%.cpp
	$(CC) $< -o $@ $(OPENCV_FLAGS)

carpets:
	mkdir -p bin
	mkdir -p bin/marcas
clean:
	rm -rf bin
