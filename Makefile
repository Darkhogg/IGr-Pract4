MACHINE= $(shell uname -s)

ifeq ($(MACHINE),Darwin)
	OPENGL_INC= -FOpenGL
	OPENGL_LIB= -framework OpenGL
	SDL_INC= `sdl-config --cflags`
	SDL_LIB= `sdl-config --libs`
else
	OPENGL_INC= -I/usr/X11R6/include
	OPENGL_LIB= -I/usr/lib64 -lGL -lGLU
	SDL_INC= `sdl-config --cflags`
	SDL_LIB= `sdl-config --libs` -lSDL_image
endif

# object files have corresponding source files
OBJS= main.o images.o ImageScene.o Pixbuf.o PtgTree.o PtgSquare.o
CXX=g++
ifeq ($(MODE), prod)
	EXTRA_FLAGS= -O3 -funroll-loops
else
	EXTRA_FLAGS= -g
endif
COMPILER_FLAGS= -std=c++11 $(EXTRA_FLAGS)
INCLUDE= $(SDL_INC) $(OPENGL_INC)
LIBS= $(SDL_LIB) $(OPENGL_LIB)

EXEC= imgt

$(EXEC): _makeskel $(OBJS)
	$(CXX) $(COMPILER_FLAGS) -o $(EXEC) $(OBJS) skel/*.o $(LIBS)

_makeskel:
	$(MAKE) -C skel

%.o:    %.cpp
	$(CXX) -c $(COMPILER_FLAGS) -o $@ $< $(INCLUDE)

clean:
	$(MAKE) -C skel clean
	rm -f $(EXEC) $(OBJS)
