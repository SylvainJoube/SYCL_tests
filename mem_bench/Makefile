CXX = dpcpp
CXXFLAGS = -Og -g -std=c++17

DIRS = bin
IMPLICIT_EXE_NAME = bin/memory_benchmark
IMPLICIT_SOURCES = memory_benchmark.cpp

all: build_implicit

build_implicit:
	$(CXX) $(CXXFLAGS) -o $(IMPLICIT_EXE_NAME) $(IMPLICIT_SOURCES)

run: 
	./$(IMPLICIT_EXE_NAME)

clean: 
	rm -f $(IMPLICIT_EXE_NAME)


# Creates needed directories
$(shell mkdir -p $(DIRS))