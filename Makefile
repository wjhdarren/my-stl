all: main
main: main.cpp my/unordered_map.h
	clang++ --std=c++23 -o main main.cpp
