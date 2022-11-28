CXX = /home/${USER}/sycl_workspace/llvm/build/bin/clang++ -fsycl -fsycl-targets=nvptx64-nvidia-cuda --cuda-path=/usr/local/cuda
#dpcpp
CXXFLAGS = -O2 -std=c++17 -Wall # -O0 -g     -O2 -std=c++17

FILE_NAME = bench
#memory_benchmark_file_output_cmp_all
#chrono_test
#memory_benchmark_file_output

DIRS = bin
RUN_EXE_NAME = bin/$(FILE_NAME)
#IMPLICIT_SOURCES = memory_benchmark.cpp

# Affiche uniquement la liste des devices dispo
all: build run

everything : build run_all_tests run_traccc

# Build and run all tests in a single program
all_tests: build run_all_tests

# Builds and runs each test separately
each_test: build run_each_test

profile_alloc: build run_profile_alloc

traccc_sparse: build run_traccc_sparse

#terminalout_build:
#	$(CXX) $(CXXFLAGS) -o bin/memory_benchmark memory_benchmark.cpp

mem: build run_mem
mem2: build run_mem2

traccc : build run_traccc

traccc_acat : build run_traccc_acat

# ACAT nouvelle version : make build && \
# ./bin/bench traccc_acat start_test_index stop_test_index run_count ld_repeat
# Example : ./bin/bench traccc_acat 2 2 1 1000

helloworld: build run_helloworld

run_mem:
	./$(RUN_EXE_NAME) mem
run_mem2:
	./$(RUN_EXE_NAME) mem2

run_helloworld:
	./$(RUN_EXE_NAME) helloworld

run_traccc:
	./$(RUN_EXE_NAME) traccc

run_traccc_acat:
	./$(RUN_EXE_NAME) traccc_acat

run_traccc_acat_flat_ld100:
	./$(RUN_EXE_NAME) traccc_acat_flat_ld100

run_traccc_sparse:
	./$(RUN_EXE_NAME) traccc sparse

run_profile_alloc:
	./$(RUN_EXE_NAME) profile_alloc

build:
	$(CXX) $(CXXFLAGS) -o bin/$(FILE_NAME) $(FILE_NAME).cpp

syclcc:
	syclcc $(CXXFLAGS) -o bin/$(FILE_NAME) $(FILE_NAME).cpp

dpcpp:
	dpcpp $(CXXFLAGS) -o bin/$(FILE_NAME) $(FILE_NAME).cpp

run: 
	./$(RUN_EXE_NAME)

run_all_tests:
	./$(RUN_EXE_NAME) 1
# 4 runs for every tests

run_test_LM_optim :
	for runCount in 1 2 ; do \
		for testID in 1 ; do \
        	echo run $$runCount testid $$testID ; \
			./$(RUN_EXE_NAME) $$testID $$runCount ; \
    	done \
    done

run_test_DMA :
	for runCount in 1 2 ; do \
		for testID in 2 ; do \
        	echo run $$runCount testid $$testID ; \
			./$(RUN_EXE_NAME) $$testID $$runCount ; \
    	done \
    done

run_test_alloc :
	for runCount in 1 2 ; do \
		for testID in 4 ; do \
        	echo run $$runCount testid $$testID ; \
			./$(RUN_EXE_NAME) $$testID $$runCount ; \
    	done \
    done

#./$(RUN_EXE_NAME) 1 1

# 
run_each_test:
	for runCount in 1 2 3 4 ; do \
		for testID in 1 2 3 4 5 6 7 ; do \
        	echo run $$runCount testid $$testID ; \
			./$(RUN_EXE_NAME) $$testID $$runCount ; \
    	done \
    done

run_sum_count_access_time:
	./$(RUN_EXE_NAME) 6 1
# test number 6, run 1

run_cache_size:
	./$(RUN_EXE_NAME) 7 1

clean: 
	rm -f $(RUN_EXE_NAME)





hello: build_hello run_hello

hello_named: build_hello_named run_hello_named

mbf: build_mbf run_mbf

build_hello:
	$(CXX) $(CXXFLAGS) -o bin/hello_world hello_world.cpp

run_hello: 
	./bin/hello_world


build_hello_named:
	$(CXX) $(CXXFLAGS) -o bin/hello_world_named hello_world_named.cpp


run_hello_named: 
	./bin/hello_world_named

# Testing launch wait and order
hello_sleep:
	$(CXX) $(CXXFLAGS) -o bin/hello_world_named_sleep hello_world_named_sleep.cpp
	./bin/hello_world_named_sleep 1
	./bin/hello_world_named_sleep 2
	./bin/hello_world_named_sleep 3
	./bin/hello_world_named_sleep 4

build_mbf:
	$(CXX) $(CXXFLAGS) -o bin/mbfoca_named mbfoca_named.cpp

run_mbf: 
	./bin/mbfoca_named

#	$(CXX) $(CXXFLAGS) -o $(IMPLICIT_EXE_NAME) $(IMPLICIT_SOURCES)



gcc_build :
	g++ -O0 -g -std=c++17 -o bin/simple_stupid_gcc simple_stupid_gcc.cpp




# Creates needed directories
$(shell mkdir -p $(DIRS))
