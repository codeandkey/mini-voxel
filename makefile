COMPILER = g++
FLAGS = -std=c++11 -Wall
LDFLAGS = `pkg-config --static --libs glfw3` -lGLU -lGLEW -lSOIL

SOURCES = Implementation.cpp Voxel.cpp VoxelGrid.cpp
OUTPUT = EnvOutput

OBJECTS = $(SOURCES:.cpp=.o)
VPATH = source

all: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	$(COMPILER) $(OBJECTS) $(LDFLAGS) -o $(OUTPUT)

%.o: %.cpp
	$(COMPILER) $(FLAGS) -c $< -o $@

clean:
	rm -R *.o $(OUTPUT)
