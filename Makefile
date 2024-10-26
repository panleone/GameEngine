CXX=g++
CXXFLAGS=-std=c++20 -I./include
LIBS = -lglfw -lglm

OBJ = main.o  WindowManager.o \
 		objects/Shader.o objects/Model.o \
		objects/cube/Cube.o

HEADERS =  WindowManager.h \
			math/Matrix.h math/MatrixUtils.h \
 			objects/Entity.h objects/Shader.h objects/Model.h \
 			objects/cube/Cube.h
SRC = WindowManager.cpp main.cpp \
		objects/Shader.cpp objects/Model.cpp \
		objects/cube/Cube.cpp

BIN = GameEngine

$(BIN) : $(OBJ) glad.o
	$(CXX) $(CXXFLAGS) $(LIBS) $(OBJ) glad.o -o $(BIN)

$(OBJ) : $(HEADERS) Makefile

glad.o:
	$(CXX) $(CXXFLAGS) -c src/glad.c

.PHONY: clean format
clean:
	find . -type f -name '*.o*' -exec rm {} +
	-rm "$(BIN)"

format:
	clang-format -i $(SRC) $(HEADERS)
