CXX=g++
CXXFLAGS=-std=c++23 -I./libs/include
LIBS = -lglfw -lglm

OBJ = main.o WindowManager.o Camera.o \
		shaders/Shader.o \
 		objects/Model.o \
		objects/cube/Cube.o objects/cubic_light_source/CubicLight.o \
		textures/Texture.o

HEADERS =  WindowManager.h Camera.h \
			math/Matrix.h math/MatrixUtils.h \
 			objects/Entity.h objects/Light.h objects/Model.h \
 			objects/cube/Cube.h objects/cubic_light_source/CubicLight.h \
 			shaders/Shader.h \
 			textures/Texture.h
SRC = WindowManager.cpp main.cpp Camera.cpp \
		shaders/Shader.cpp \
		objects/Model.cpp \
		objects/cube/Cube.cpp objects/cubic_light_source/CubicLight.cpp \
		textures/Texture.cpp

BIN = GameEngine

$(BIN) : $(OBJ) glad.o
	$(CXX) $(CXXFLAGS) $(LIBS) $(OBJ) glad.o -o $(BIN)

$(OBJ) : $(HEADERS) Makefile

glad.o:
	$(CXX) $(CXXFLAGS) -c libs/src/glad.c

.PHONY: clean format
clean:
	find . -type f -name '*.o*' -exec rm {} +
	-rm "$(BIN)"

format:
	clang-format -i $(SRC) $(HEADERS)
