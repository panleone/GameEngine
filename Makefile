CXX=g++
CXXFLAGS=-std=c++23 -I./libs/include -g -O2
LIBS = -lglfw -lglm -lassimp

OBJ = src/main.o src/WindowManager.o src/Camera.o \
		src/shaders/Shader.o \
		src/shaders/phong_light_model/EntityShader.o \
 		src/objects/Model.o src/objects/EntityManager.o \
		src/textures/Texture.o \
		src/buffer/FrameBuffer.o \
		src/utils/Random.o

HEADERS =  src/WindowManager.h src/Camera.h \
			src/math/Matrix.h src/math/MatrixUtils.h \
 			src/objects/Entity.h src/objects/Light.h src/objects/Model.h src/objects/EntityManager.h \
 			src/shaders/Shader.h src/shaders/Uniform.h \
 			src/shaders/phong_light_model/EntityShader.h \
 			src/shaders/light_source/LightShader.h \
 			src/shaders/post_processing/PostProcessingShader.h \
 			src/textures/Texture.h \
 			src/buffer/Buffer.h src/buffer/FrameBuffer.h \
 			src/utils/Random.h
SRC = src/WindowManager.cpp src/main.cpp src/Camera.cpp \
		src/shaders/Shader.cpp \
		src/shaders/phong_light_model/EntityShader.cpp \
		src/objects/Model.cpp src/objects/EntityManager.cpp \
		src/textures/Texture.cpp \
		src/buffer/FrameBuffer.cpp \
		src/utils/Random.cpp

BIN = GameEngine

$(BIN) : $(OBJ) glad.o
	$(CXX) $(CXXFLAGS) $(LIBS) $(OBJ) glad.o -o $(BIN)

$(OBJ) : $(HEADERS) Makefile

glad.o:
	$(CXX) $(CXXFLAGS) -c libs/src/glad.c

.PHONY: clean format
clean:
	find . -type f -name '*.o' -exec rm {} +
	-rm "$(BIN)"

format:
	clang-format -i $(SRC) $(HEADERS)
