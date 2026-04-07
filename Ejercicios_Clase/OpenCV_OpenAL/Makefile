CC = g++
CXXFLAGS = -Wall -O2 -w
OPENCV_FLAGS = $(shell pkg-config --cflags --libs opencv4)
OPENAL_FLAGS = -lopenal -lalut


TARGET = bin/discoCam
SRC = discoCam.cpp
OBJ = exec/discoCam.o

all: discoCam

discoCam: $(OBJ)
	mkdir -p bin
	$(CC) $(OBJ) -o $(TARGET) $(OPENCV_FLAGS) $(OPENAL_FLAGS) -lm

$(OBJ): $(SRC)
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC) -o $(OBJ) $(OPENCV_FLAGS)

clean:
	rm -r bin exec
zip:
	zip -r instrumentVirtual.zip Makefile discoCam.cpp 
