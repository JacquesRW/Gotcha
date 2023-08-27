EXE = Gotcha

ifeq ($(OS),Windows_NT)
	NAME := $(EXE).exe
else
	NAME := $(EXE)
endif

rule:
	clang++ src/main.cpp src/io/*.cpp src/mcts/mcts.cpp src/state/*.cpp src/util/*.cpp -o $(NAME) -O3 -DNDEBUG -Wextra