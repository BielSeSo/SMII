CC = g++
CXXFLAGS = -Wall -O2 -w
OPENCV_FLAGS = $(shell pkg-config --cflags --libs opencv4)


TARGET_1 = bin/ejercicio_1
TARGET_2 = bin/ejercicio_2
TARGET_3 = bin/ejercicio_3
TARGET_4_1 = bin/ejemplo_4
TARGET_4_2 = bin/ejercicio_4
TARGET_5 = bin/ejercicio_5
TARGET_6 = bin/ejercicio_6
TARGET_7 = bin/ejercicio_7
TARGET_8 = bin/ejercicio_8
TARGET_9 = bin/ejercicio_9
TARGET_A1 = bin/buscarLaser


SRC_1 = code/ejercicio_1.cpp
SRC_2 = code/ejercicio_2.cpp
SRC_3 = code/ejercicio_3.cpp
SRC_4_1 = code/ejemplo_4.cpp
SRC_4_2 = code/ejercicio_4.cpp
SRC_5 = code/ejercicio_5.cpp
SRC_6 = code/ejercicio_6.cpp
SRC_7 = code/ejercicio_7.cpp
SRC_8 = code/ejercicio_8.cpp
SRC_9 = code/ejercicio_9.cpp
SRC_A1 = code/actividad_1.cpp code/buscarLaser.c


OBJ_1 = exec/ejercicio_1.o
OBJ_2 = exec/ejercicio_2.o
OBJ_3 = exec/ejercicio_3.o
OBJ_4_1 = exec/ejemplo_4.o
OBJ_4_2 = exec/ejercicio_4.o
OBJ_5 = exec/ejercicio_5.o
OBJ_6 = exec/ejercicio_6.o
OBJ_7 = exec/ejercicio_7.o
OBJ_8 = exec/ejercicio_8.o
OBJ_9 = exec/ejercicio_9.o
OBJ_A1 = $(SRC_A1:code/%.cpp=exec/%.o)


# Compilate all
all: ejercicio_1 ejercicio_2 \
	ejercicio_3 ejemplo_4 \
	ejercicio_4 ejercicio_5 \
	ejercicio_6 ejercicio_7 \
	ejercicio_8 ejercicio_9 \
	actividad_1


# Ejercicio 1 compilation
ejercicio_1: $(OBJ_1)
	mkdir -p bin
	$(CC) $(OBJ_1) -o $(TARGET_1) $(OPENCV_FLAGS)

$(OBJ_1): $(SRC_1)
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC_1) -o $(OBJ_1) $(OPENCV_FLAGS)


# Ejercicio 2 compilation
ejercicio_2: $(OBJ_2)
	mkdir -p bin
	$(CC) $(OBJ_2) -o $(TARGET_2) $(OPENCV_FLAGS)

$(OBJ_2): $(SRC_2)
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC_2) -o $(OBJ_2) $(OPENCV_FLAGS)


# Ejercicio 3 compilation
ejercicio_3: $(OBJ_3)
	mkdir -p bin
	$(CC) $(OBJ_3) -o $(TARGET_3) $(OPENCV_FLAGS)

$(OBJ_3): $(SRC_2)
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC_3) -o $(OBJ_3) $(OPENCV_FLAGS)


# Ejemplo 4 compilation
ejemplo_4: $(OBJ_4_1)
	mkdir -p bin
	mkdir -p bin/ejemplo_4_img
	$(CC) $(OBJ_4_1) -o $(TARGET_4_1) $(OPENCV_FLAGS) 

$(OBJ_4_1): $(SRC_4_1)
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC_4_1) -o $(OBJ_4_1) $(OPENCV_FLAGS) 
# Ejecucion: ./bin/ejemplo4 code/images/LinuxLogo.jpg code/images/WindowsLogo.jpg


# Ejercicio 4 compilation
ejercicio_4: $(OBJ_4_2)
	mkdir -p bin
	mkdir -p bin/ejercicio_4_img
	$(CC) $(OBJ_4_2) -o $(TARGET_4_2) $(OPENCV_FLAGS)

$(OBJ_4_2): $(SRC_4_2)
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC_4_2) -o $(OBJ_4_2) $(OPENCV_FLAGS) 


# Ejercicio 5 compilation
ejercicio_5: $(OBJ_5)
	mkdir -p bin
	mkdir -p bin/ejercicio_5_img
	$(CC) $(OBJ_5) -o $(TARGET_5) $(OPENCV_FLAGS)

$(OBJ_5):$(SRC_5) 
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC_5) -o $(OBJ_5) $(OPENCV_FLAGS)


# Ejercicio 6 compilation
ejercicio_6: $(OBJ_6)
	mkdir -p bin
	$(CC) $(OBJ_6) -o $(TARGET_6) $(OPENCV_FLAGS)

$(OBJ_6):$(SRC_6) 
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC_6) -o $(OBJ_6) $(OPENCV_FLAGS)


# Ejercicio 7 compilation
ejercicio_7: $(OBJ_7)
	mkdir -p bin
	$(CC) $(OBJ_7) -o $(TARGET_7) $(OPENCV_FLAGS)

$(OBJ_7):$(SRC_7) 
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC_7) -o $(OBJ_7) $(OPENCV_FLAGS)


# Ejercicio 8 compilation
ejercicio_8: $(OBJ_8)
	mkdir -p bin
	$(CC) $(OBJ_8) -o $(TARGET_8) $(OPENCV_FLAGS)

$(OBJ_8):$(SRC_8) 
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC_8) -o $(OBJ_8) $(OPENCV_FLAGS)


# Ejercicio 9 compilation
ejercicio_9: $(OBJ_9)
	mkdir -p bin
	$(CC) $(OBJ_9) -o $(TARGET_9) $(OPENCV_FLAGS)

$(OBJ_9):$(SRC_9) 
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC_9) -o $(OBJ_9) $(OPENCV_FLAGS)


# Actividad 1 compilation
actividad_1: $(OBJ_A1)
	mkdir -p bin
	$(CC) $(OBJ_A1) -o $(TARGET_A1) $(OPENCV_FLAGS)

exec/%.o: code/%.cpp
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $< -o $@ $(OPENCV_FLAGS)


# Clean everything	
clean:
	rm -rf bin exec

