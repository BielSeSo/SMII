CC = gcc
OPENAL_FLAGS = -lopenal -lalut
CXXFLAGS = -Wall -O2


TARGET_1 = bin/ejercicio_1
TARGET_2 = bin/ejercicio_2
TARGET_3 = bin/ejercicio_5
TARGET_4 = bin/ejercicio_9


SRC_1 = code/openal_info.c
SRC_2 = code/helloWorld_ALUT.c
SRC_3 = code/openal_senyals.c
SRC_4 = code/openal_fitxer.c


OBJ_1  = exec/openal_info.o
OBJ_2 = exec/helloWorld_ALUT.o
OBJ_3 = exec/openal_senyals.o
OBJ_4 = exec/openal_fitxer.o


# Compilate all
all: ejercicio_1 ejercicio_2 \
    ejercicio_5 ejercicio_9


# Ejercicio 1 compilation
ejercicio_1: $(OBJ_1)
	mkdir -p bin
	$(CC) $(OBJ_1) -o $(TARGET_1) $(OPENAL_FLAGS)

$(OBJ_1): $(SRC_1)
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC_1) -o $(OBJ_1)


# Ejercicio 2 compilation
ejercicio_2: $(OBJ_2)
	mkdir -p bin
	$(CC) $(OBJ_2) -o $(TARGET_2) $(OPENAL_FLAGS)

$(OBJ_2): $(SRC_2)
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC_2) -o $(OBJ_2)


# Ejercicio 2 compilation
ejercicio_5: $(OBJ_3)
	mkdir -p bin
	$(CC) $(OBJ_3) -o $(TARGET_3) $(OPENAL_FLAGS)

$(OBJ_3): $(SRC_3)
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC_3) -o $(OBJ_3)


# Ejercicio 9 compilation
ejercicio_9: $(OBJ_4)
	mkdir -p bin
	$(CC) $(OBJ_4) -o $(TARGET_4) $(OPENAL_FLAGS)

$(OBJ_4): $(SRC_4)
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC_4) -o $(OBJ_4)


# Clean everything	
clean:
	rm -rf bin exec