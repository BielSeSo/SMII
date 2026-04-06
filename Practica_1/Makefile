CC = gcc
OPENGL_FLAGS = -lglut -lGLU -lGL -lX11 -lm
CXXFLAGS = -Wall -O2


TARGET_1 = bin/atlantis
TARGET_2 = bin/geoface


SRC_1 = code/atlantis/atlantis.c \
       code/atlantis/whale.c \
       code/atlantis/dolphin.c \
       code/atlantis/shark.c \
       code/atlantis/swim.c
SRC_2 = code/geoface/display.c \
       code/geoface/fileio.c \
       code/geoface/main.c \
       code/geoface/make_face.c \
       code/geoface/muscle.c


OBJ_1 = $(SRC_1:code/atlantis/%.c=exec/atlantis/%.o)
OBJ_2 = $(SRC_2:code/geoface/%.c=exec/geoface/%.o)
       

# Compilate all
all: glutplane \
	mjkwarp \
	triselect \
	atlantis \
	geoface \
	bouncing_ball \
	game_1 \
	game_2


# Glutplane compilation
glutplane: exec/glutplane.o
	mkdir -p bin
	$(CC) exec/glutplane.o -o bin/glutplane $(OPENGL_FLAGS)

exec/glutplane.o: code/glutplane.c
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c code/glutplane.c -o exec/glutplane.o


# Mjwarp compilation
mjkwarp: exec/mjkwarp.o exec/mjkimage.o
	mkdir -p bin
	$(CC) exec/mjkwarp.o exec/mjkimage.o -o bin/mjkwarp $(OPENGL_FLAGS)

exec/mjkwarp.o: code/mjkwarp.c 
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c code/mjkwarp.c -o exec/mjkwarp.o

exec/mjkimage.o: code/mjkimage.c
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c code/mjkimage.c -o exec/mjkimage.o


# Triselect compilation
triselect: exec/triselect.o
	mkdir -p bin
	$(CC) exec/triselect.o -o bin/triselect $(OPENGL_FLAGS)

exec/triselect.o: code/triselect.c
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c code/triselect.c -o exec/triselect.o


# Atlantis compilation
atlantis: $(OBJ_1)
	mkdir -p bin
	$(CC) $(OBJ_1) -o $(TARGET_1) $(OPENGL_FLAGS)

exec/atlantis/%.o: code/atlantis/%.c
	mkdir -p exec
	mkdir -p exec/atlantis
	$(CC) $(CXXFLAGS) -c $< -o $@


# Geoface compilation
geoface: $(OBJ_2)
	mkdir -p bin
	$(CC) $(OBJ_2) -o $(TARGET_2) $(OPENGL_FLAGS)

exec/geoface/%.o: code/geoface/%.c
	mkdir -p exec
	mkdir -p exec/geoface
	$(CC) $(CXXFLAGS) -c $< -o $@


# Bouncing ball compilation
bouncing_ball: exec/bouncing_ball.o
	mkdir -p bin
	$(CC) exec/bouncing_ball.o -o bin/bouncing_ball $(OPENGL_FLAGS)

exec/bouncing_ball.o: code/bouncing_ball.c
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c code/bouncing_ball.c -o exec/bouncing_ball.o


# Game 1 compilation
game_1: exec/game_1.o
	mkdir -p bin
	$(CC) exec/game_1.o -o bin/game_1 $(OPENGL_FLAGS)

exec/game_1.o: code/game_1.c
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c code/game_1.c -o exec/game_1.o


# Game 2 compilation
game_2: exec/game_2.o
	mkdir -p bin
	$(CC) exec/game_2.o -o bin/game_2 $(OPENGL_FLAGS)

exec/game_2.o: code/game_2.c
	mkdir -p exec
	$(CC) $(CXXFLAGS) -c code/game_2.c -o exec/game_2.o 


# Clean everything	
clean:
	rm -r bin exec
