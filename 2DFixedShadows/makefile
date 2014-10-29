COMPILER=g++
FLAGS=-std=c++11 -Wall

SOURCES=Main.cpp RecoilRLImage.cpp
VPATH=source
OBJECTS=$(SOURCES:.cpp=.o)
OUTPUT=Shadows2D
LINKER=`pkg-config --static --libs glfw3` -lGLU -lSOIL -lGLEW
all: $(OUTPUT)

$(OUTPUT):$(OBJECTS)
	$(COMPILER) $(OBJECTS) $(LINKER) -o $(OUTPUT)

$%.o:%.cpp
	$(COMPILER) $(FLAGS) -c $< -o $@

clean:
	rm -r *.o $(OUTPUT)
