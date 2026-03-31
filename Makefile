CC = g++
OPENGL_FLAGS = -lglut -lGL -lGLU -lassimp -ldl
CXXFLAGS = -Wall -O2


TARGET = bin/proyecto


SRC = code/main.cpp \
	code/load_obj.cpp \
	code/player.cpp \
	code/load_minimap.cpp


OBJ = $(SRC:code/%.cpp=exec/%.o)


# Compile all
all: test_map


# Test map compilation
test_map: $(OBJ)
	mkdir -p bin
	$(CC) $(OBJ) -o $(TARGET) $(OPENGL_FLAGS)

exec/%.o: code/%.cpp
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c $< -o $@


# Clean everything
clean:
	rm -r bin exec