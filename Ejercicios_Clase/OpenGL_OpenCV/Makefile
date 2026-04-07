CC = g++
CXXFLAGS = -Wall -O2 -w
OPENGL_FLAGS = -lglut -lGLU -lGL -lX11 -lm
OPENCV_FLAGS = $(shell pkg-config --cflags --libs opencv4)

TARGET = bin/Trabajo_1
SRC = code/Trabajo_1.cpp
OBJ = exec/Trabajo_1.o

all: trabajo_1

trabajo_1: $(OBJ)
	mkdir -p bin
	$(CC) $(OBJ) -o $(TARGET) $(OPENGL_FLAGS) $(OPENCV_FLAGS)

$(OBJ): $(SRC)
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC) -o $(OBJ) $(OPENCV_FLAGS)

clean:
	rm -r bin exec