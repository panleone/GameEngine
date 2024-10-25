CXX=g++
CXXFLAGS=-std=c++20 -I./include
LIBS = -lglfw -lglm

OBJ = main.o  WindowManager.o \

HEADERS =  WindowManager.h
SRC = WindowManager.cpp main.cpp


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
