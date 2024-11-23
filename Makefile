CXX=g++
CXXFLAGS=-std=c++23 -I./libs/include -g
LIBS = -lglfw -lglm -lassimp

OBJ = main.o WindowManager.o Camera.o \
		shaders/Shader.o \
 		objects/Model.o objects/EntityManager.o \
		textures/Texture.o \
		buffer/Buffer.o buffer/FrameBuffer.o

HEADERS =  WindowManager.h Camera.h \
			math/Matrix.h math/MatrixUtils.h \
 			objects/Entity.h objects/Light.h objects/Model.h objects/EntityManager.h \
 			shaders/Shader.h \
 			textures/Texture.h \
 			buffer/Buffer.h buffer/FrameBuffer.h
SRC = WindowManager.cpp main.cpp Camera.cpp \
		shaders/Shader.cpp \
		objects/Model.cpp objects/EntityManager.cpp \
		textures/Texture.cpp \
		buffer/Buffer.cpp buffer/FrameBuffer.cpp

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
