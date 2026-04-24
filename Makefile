CC = g++
OPENGL_FLAGS = -lglut -lGLU -lGL -lX11
OPENCV_FLAGS = $(shell pkg-config --cflags --libs opencv4)
OPENAL_FLAGS = -lopenal -lalut
FREE_FLAGS = -lm -lassimp
CXXFLAGS = -O2 -w

TARGET= bin/proyecto

SRC_INTERFACE = code/interface.cpp
SRC_SOUND = code/sound_maker.cpp
SRC_MAIN_GAME = code/game_render.cpp
SRC_GAME = code/game/load_map.cpp code/game/load_obj.cpp code/game/player.cpp
SRC_MAIN = code/main.cpp
SRC_CONTROL = code/control_mando.cpp

OBJ_INTERFACE = exec/interface.o
OBJ_SOUND = exec/sound_maker.o
OBJ_MAIN_GAME = exec/game_render.o
OBJ_GAME = $(SRC_GAME:code/game/%.cpp=exec/game/%.o)
OBJ_CONTROL = exec/control_mando.o
OBJ_MAIN = exec/main.o


all: bin

# Binary compilation for Ubuntu
bin: $(OBJ_INTERFACE) $(OBJ_SOUND) $(OBJ_GAME) $(OBJ_MAIN_GAME) $(OBJ_CONTROL) $(OBJ_MAIN)
	mkdir -p bin	
	$(CC) $(OBJ_INTERFACE) $(OBJ_SOUND) $(OBJ_GAME) $(OBJ_MAIN_GAME) $(OBJ_CONTROL) $(OBJ_MAIN) -o $(TARGET) \
	$(OPENCV_FLAGS) $(OPENGL_FLAGS) $(OPENAL_FLAGS) $(FREE_FLAGS)

# Interface compilation
$(OBJ_INTERFACE): $(SRC_INTERFACE)
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $(SRC_INTERFACE) -o $(OBJ_INTERFACE) $(OPENCV_FLAGS) $(OPENGL_FLAGS)

# Sound maker compilation
$(OBJ_SOUND): $(SRC_SOUND)
	$(CC) $(CXXFLAGS) -c $(SRC_SOUND) -o $(OBJ_SOUND)

# Game render compilation
$(OBJ_MAIN_GAME): $(SRC_MAIN_GAME)
	$(CC) $(CXXFLAGS) -c $(SRC_MAIN_GAME) -o $(OBJ_MAIN_GAME) $(OPENCV_FLAGS)

# Game compilation
exec/game/%.o: code/game/%.cpp
	mkdir -p exec/game
	$(CC) $(CXXFLAGS) -c $< -o $@ $(OPENCV_FLAGS)

$(OBJ_MAIN): $(SRC_MAIN)
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c code/main.cpp -o exec/main.o $(OPENCV_FLAGS)
	
# Clean everything
clean:
	rm -r bin exec 
