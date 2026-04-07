CC = g++
OPENGL_FLAGS = -lglut -lGLU -lGL -lX11 -lm
OPENCV_FLAGS = $(shell pkg-config --cflags --libs opencv4)
OPENAL_FLAGS = -lopenal -lalut
CXXFLAGS = -Wall -O2 -w


TARGET_1 = bin/pruebaGL
TARGET_2 = bin/puebaCV
TARGET_3 = bin/pruebaAL


SRC_1 = code/pruebaGL.cpp
SRC_2 = code/pruebaCV.cpp
SRC_3 = code/pruebaAL.cpp


OBJ_1 = exec/pruebaGL.o
OBJ_2 = exec/pruebaCV.o
OBJ_3 = exec/pruebaAL.o


# Compile all
all: pruebaGL

# Prueba GL compilation
pruebaGL: $(OBJ_1)
	mkdir -p bin
	$(CC) $(OBJ_1) -o $(TARGET_1) $(OPENGL_FLAGS)

$(OBJ_1): $(SRC_1)
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC_1) -o $(OBJ_1)


# Prueba GL compilation
pruebaCV: $(OBJ_2)
	mkdir -p bin
	$(CC) $(OBJ_2) -o $(TARGET_2) $(OPENCV_FLAGS)

$(OBJ_2): $(SRC_2)
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC_2) -o $(OBJ_2) $(OPENCV_FLAGS)


# Prueba GL compilation
pruebaAL: $(OBJ_3)
	mkdir -p bin
	$(CC) $(OBJ_3) -o $(TARGET_3) $(OPENAL_FLAGS)

$(OBJ_3): $(SRC_3)
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC_3) -o $(OBJ_3)


# Clean everything
clean:
	rm -r bin exec