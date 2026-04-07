CC = gcc
OPENAL_FLAGS = -lopenal -lalut
OPENGL_FLAGS = -lglut -lGLU -lGL -lX11 -lm
CXXFLAGS = -Wall -O2


TARGET_1 = bin/ejercicio_1
TARGET_2 = bin/ejercicio_2
TARGET_3 = bin/ejercicio_5
TARGET_4 = bin/ejercicio_9
TARGET_5 = bin/ejercicio_11
TARGET_7 = bin/ejercicio_15
TARGET_8 = bin/ejercicio_18
TARGET_9 = bin/actividad_1
TARGET_10 = bin/actividad_2


SRC_1 = code/openal_info.c
SRC_2 = code/helloWorld_ALUT.c
SRC_3 = code/openal_senyals.c
SRC_4 = code/openal_fitxer.c
SRC_5 = code/pige.c
SRC_7 = code/al_simple.c
SRC_8 = code/al_hrtf_loopback_wav.c
SRC_9 = code/actividad_1.c
SRC_10 = code/actividad_2/actividad_1_2.c code/actividad_2/actividad_2.c


OBJ_1  = exec/openal_info.o
OBJ_2 = exec/helloWorld_ALUT.o
OBJ_3 = exec/openal_senyals.o
OBJ_4 = exec/openal_fitxer.o
OBJ_5 = exec/pige.o
OBJ_7 = exec/al_simple.o
OBJ_8 = exec/al_hrtf_loopback_wav.o
OBJ_9 = exec/actividad_1.o
OBJ_10 = $(SRC_10:code/actividad_2/%.c=exec/actividad_2/%.o)


# Compilate all
all: ejercicio_1 ejercicio_2 \
    ejercicio_5 ejercicio_9 \
	ejercicio_11  \
	ejercicio_15 ejercicio_18 \
	actividad_1 actividad_2


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


# Ejercicio 11 compilation
ejercicio_11: $(OBJ_5)
	mkdir -p bin
	$(CC) $(OBJ_5) -o $(TARGET_5) $(OPENAL_FLAGS) $(OPENGL_FLAGS)

$(OBJ_5): $(SRC_5)
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC_5) -o $(OBJ_5)


# Ejercicio 15 compilation
ejercicio_15: $(OBJ_7)
	mkdir -p bin
	$(CC) $(OBJ_7) -o $(TARGET_7) $(OPENAL_FLAGS) -lm

$(OBJ_7): $(SRC_7)
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC_7) -o $(OBJ_7)


# Ejercicio 17 compilation
ejercicio_18: $(OBJ_8)
	mkdir -p bin
	mkdir -p bin/E18_audio
	$(CC) $(OBJ_8) -o $(TARGET_8) $(OPENAL_FLAGS) -lm

$(OBJ_8): $(SRC_8)
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC_8) -o $(OBJ_8)


# Actividad 1 compilation
actividad_1: $(OBJ_9)
	mkdir -p bin
	$(CC) $(OBJ_9) -o $(TARGET_9) $(OPENAL_FLAGS) $(OPENGL_FLAGS)

$(OBJ_9): $(SRC_9)
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC_9) -o $(OBJ_9)


# Actividad 2 compilation
actividad_2: $(OBJ_10)
	mkdir -p bin
	mkdir -p bin/A2_audio
	$(CC) $(OBJ_10) -o $(TARGET_10) $(OPENAL_FLAGS) $(OPENGL_FLAGS)

exec/actividad_2/%.o: code/actividad_2/%.c
	mkdir -p exec
	mkdir -p exec/actividad_2
	$(CC) $(CXXFLAGS) -c $< -o $@


# Clean everything	
clean:
	rm -rf bin exec