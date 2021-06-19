CXX = dpcpp
CXXFLAGS = -O0 -g -std=c++17 # -O2 -std=c++17

DIRS = bin
IMPLICIT_EXE_NAME = bin/memory_benchmark
IMPLICIT_SOURCES = memory_benchmark.cpp

all: implicit_build

implicit_build:
	$(CXX) $(CXXFLAGS) -o $(IMPLICIT_EXE_NAME) $(IMPLICIT_SOURCES)

gcc_build :
	g++ -O0 -g -std=c++17 -o bin/simple_stupid_gcc simple_stupid_gcc.cpp

run: 
	./$(IMPLICIT_EXE_NAME)

clean: 
	rm -f $(IMPLICIT_EXE_NAME)


# Creates needed directories
$(shell mkdir -p $(DIRS))