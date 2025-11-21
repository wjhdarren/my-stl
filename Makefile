CPPFLAGS = -std=c++23 -Wall -Wextra -g -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/opt/llvm/lib/c++ -L/opt/homebrew/opt/llvm/lib/unwind -L/opt/homebrew/opt/boost/lib -L/opt/homebrew/lib -L$(HOME)/.local/lib -lunwind

all: main
main: main.cpp
	clang++ $(CPPFLAGS) -o main main.cpp $(LDFLAGS)
