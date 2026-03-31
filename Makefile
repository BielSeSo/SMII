CC = g++
OPENGL_FLAGS = -lglut -lGLU -lGL -lX11 -lm
OPENCV_FLAGS = $(shell pkg-config --cflags --libs opencv4)
OPENAL_FLAGS = -lopenal -lalut
CXXFLAGS = -Wall -O2

TARGET= bin/proyecto

SRC_INTERFACE = code/interface.cpp
SRC_GAME = code/game/load_minimap.cpp code/game/load_obj.cpp code/game/player.cpp
SRC_MAIN = code/main.cpp

OBJ_INTERFACE = exec/interface.o
OBJ_GAME = $(SRC_GAME:code/game/%.cpp = exec/game/%.o)
OBJ_MAIN = exec/main.o


# Compile all
all: $(OBJ_INTERFACE) $(OBJ_GAME)
	mkdir -p bin	
	$(CC) $(OBJ_INTERFACE) $(OBJ_GAME) -o $(OPENCV_FLAGS) $(OPENGL_FLAGS)

# Interface compilation
$(OBJ_INTERFACE): $(SRC_INTERFACE)
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC_INTERFACE) -o $(OBJ_INTERFACE) $(OPENCV_FLAGS)

# Game compilation
exec/game/%.o: code/game/%.c
	mkdir -p exec
	mkdir -p exec/game
	$(CC) $(CXXFLAGS) -c $< -o $@

# Clean everything
clean:
	rm -r bin exec 
