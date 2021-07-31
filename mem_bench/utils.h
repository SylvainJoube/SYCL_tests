#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#include <chrono>

// SyCL specific includes
#include <CL/sycl.hpp>
#include <array>
#include <sys/time.h>
#include <stdlib.h>
#include "constants.h"

/*
Here are some structs and useful functions that are not meant to change
very often.
*/



int mode_to_int(sycl_mode m) {
    switch (m) {
    case shared_USM : return 0;
    case device_USM : return 1;
    case host_USM : return 2;
    case accessors : return 3;
    }
    return -1;
}

std::string mode_to_string(sycl_mode m) {
    switch (m) {
    case shared_USM : return "shared_USM";
    case device_USM : return "device_USM";
    case host_USM : return "host_USM";
    case accessors : return "accessors";
    }
    return "unknown";
}





// SyCL asynchronous exception handler
// Create an exception handler for asynchronous SYCL exceptions
static auto exception_handler = [](cl::sycl::exception_list e_list) {
  for (std::exception_ptr const &e : e_list) {
    try {
      std::rethrow_exception(e);
    }
    catch (std::exception const &e) {
#if _DEBUG
      std::cout << "Failure" << std::endl;
#endif
      std::terminate();
    }
  }
};

class stime_utils {
private :
    std::chrono::_V2::steady_clock::time_point _start, _stop;

public :
    
    void start() {
        _start = std::chrono::steady_clock::now();
    }

    // Gets the us count since last start or reset.
    uint64_t reset() {
        std::chrono::duration<int64_t, std::nano> dur = std::chrono::steady_clock::now() - _start;
        _start = std::chrono::steady_clock::now();
        int64_t ns = dur.count();
        int64_t us = ns / 1000;
        return us;
    }

};

/*uint64_t get_ms() {
    auto tm = std::chrono::steady_clock::now();
    std::chrono::duration<double> s = tm - tm;


    struct timeval tp;
    gettimeofday(&tp, NULL);
    uint64_t ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    return ms;
}*/

void log(std::string str) {
    std::cout << str << std::endl;
}
void logs(std::string str) {
    std::cout << str << std::flush;
}

// level :
// 0 : important
// 1 : info
// 2 : flood

const int MAX_SHOWN_LEVEL = 1;

void log(std::string str, int level) {
    if (level <= MAX_SHOWN_LEVEL) {
        std::cout << str << std::endl;
    }
}

// Memory intensive operation (read only)
/*int compute_sum(int* array, int size) {
    int sum = 0;
    for (int i = 0; i < size; ++i) {
        sum += array[i];
    }
    return sum;
}*/


//static sycl_mode mode = sycl_mode::device_USM;
// static bool wait_queue = true;

struct host_dataset {
    //bool need_copy; // USM device needs a copy, but not host or shared.
    data_type *data_input;
    data_type *data_output;
    data_type final_result_verif = 0;
    unsigned int seed;
    // Memory allocated on the device
    data_type *device_input = nullptr;
    data_type *device_output = nullptr;
};
unsigned int global_t_data_generation_and_ram_allocation = 0;

struct gpu_timer {
    uint64_t t_data_generation_and_ram_allocation = 0;
    uint64_t t_queue_creation = 0;
    uint64_t t_allocation = 0;
    uint64_t t_copy_to_device = 0;
    uint64_t t_parallel_for = 0;
    uint64_t t_read_from_device = 0;
    uint64_t t_free_gpu = 0;
};

const bool SHOW_TIME_STATS = false;

void print_timer_iter_alloc(gpu_timer& time) {
    if ( ! SHOW_TIME_STATS ) return;
    uint64_t t_gpu;
    t_gpu = time.t_allocation + time.t_copy_to_device + time.t_read_from_device
            + time.t_parallel_for + time.t_free_gpu;
    std::cout 
            << "t_gpu - - - - - - - - - -  = " << t_gpu << std::endl
            //<< "t_queue_creation           = " << t_queue_creation << std::endl
            << "t_allocation - - - - - - - = " << time.t_allocation << std::endl
            << "t_copy_to_device           = " << time.t_copy_to_device << std::endl
            << "t_parallel_for - - - - - - = " << time.t_parallel_for << std::endl
            << "t_read_from_device         = " << time.t_read_from_device << std::endl
            << "t_free_gpu - - - - - - - - = " << time.t_free_gpu << std::endl
            ;

    log("");
}

void print_timer_iter(gpu_timer& time) {
    if ( ! SHOW_TIME_STATS ) return;
    //uint64_t t_gpu;
    //t_gpu = time.t_read_from_device + time.t_parallel_for;
    std::cout 
            << "t_parallel_for - - - - - - = " << time.t_parallel_for << std::endl
            << "t_read_from_device         = " << time.t_read_from_device << std::endl
            ;

    log("");
}

void print_timer_alloc(gpu_timer& time) {
    if ( ! SHOW_TIME_STATS ) return;
    uint64_t t_alloc_and_free;
    t_alloc_and_free = time.t_allocation + time.t_copy_to_device + time.t_free_gpu;
    std::cout 
            << "t_alloc_and_free - - - - - = " << t_alloc_and_free << std::endl
            << "t_allocation - - - - - - - = " << time.t_allocation << std::endl
            << "t_copy_to_device           = " << time.t_copy_to_device << std::endl
            << "t_free_gpu - - - - - - - - = " << time.t_free_gpu << std::endl
            ;

    log("");
}

host_dataset* generate_datasets(uint a_DATASET_NUMBER, uint a_INPUT_DATA_LENGTH,
                                uint a_OUTPUT_DATA_LENGTH, bool a_CHECK_SIMD_CPU,
                                uint a_PARALLEL_FOR_SIZE, uint a_VECTOR_SIZE_PER_ITERATION) {

    log("Generating data...", 1);
    stime_utils chrono;
    chrono.start();

    host_dataset *hdata = new host_dataset[a_DATASET_NUMBER];

    for (int i = 0; i < a_DATASET_NUMBER; ++i) {
        host_dataset *hd = &hdata[i];

        hd->data_input = new data_type[a_INPUT_DATA_LENGTH];
        hd->data_output = new data_type[a_OUTPUT_DATA_LENGTH];
        hd->seed = 452 + i * 68742;

        srand(hd->seed);

        // Fills the array with random data
        for (int i = 0; i < a_INPUT_DATA_LENGTH; ++i) {
            data_type v = rand();
            hd->data_input[i] = v;
            hd->final_result_verif += v;
        }

        // Perform SMID-like operations to verify the sum algorithm
        if (a_CHECK_SIMD_CPU) {
            data_type sum_simd_check_cpu = 0;
            // SIMD-like check
            for (int ip = 0; ip < a_PARALLEL_FOR_SIZE; ++ip) {
                for (int it = 0; it < a_VECTOR_SIZE_PER_ITERATION; ++it) {
                    int iindex = ip + it * a_PARALLEL_FOR_SIZE;
                    sum_simd_check_cpu += hd->data_input[iindex];
                }
            }

            // SMID-like OKAY VALLID - total sum = -1553315753
            if (sum_simd_check_cpu == hd->final_result_verif) {
                std::cout << "SMID-like OKAY VALLID - total sum = " << sum_simd_check_cpu << std::endl;
                std::cout << "SMID-like OKAY VALLID - total sum = " << sum_simd_check_cpu << std::endl;
                std::cout << "SMID-like OKAY VALLID - total sum = " << sum_simd_check_cpu << std::endl;
            } else {
                std::cout << "SMID-like ERROR should be " << hd->final_result_verif << " but is " << sum_simd_check_cpu << " - ERROR ERROR" << std::endl;
                std::cout << "SMID-like ERROR should be " << hd->final_result_verif << " but is " << sum_simd_check_cpu << " - ERROR ERROR" << std::endl;
                std::cout << "SMID-like ERROR should be " << hd->final_result_verif << " but is " << sum_simd_check_cpu << " - ERROR ERROR" << std::endl;
            }
        }
    }

    global_t_data_generation_and_ram_allocation = chrono.reset(); //get_ms() - t_start;

    return hdata;
}


void delete_datasets(host_dataset* hdata, uint a_DATASET_NUMBER) {
    if (hdata == nullptr) return;

    for (int i = 0; i < a_DATASET_NUMBER; ++i) {
        host_dataset *hd = &hdata[i];
        delete[] hd->data_input;
        delete[] hd->data_output;
    }
    delete[] hdata;
}

int total_main_seq_runs = 1;
//int current_main_seq_runs = 0;
int current_iteration_count = 0;

void print_total_progress() {
    const int total_iteration_count_per_seq = DATASET_NUMBER * (REPEAT_COUNT_REALLOC + REPEAT_COUNT_ONLY_PARALLEL);
    int total_iteration_count = total_iteration_count_per_seq * total_main_seq_runs;
    int progress = 100 * double(current_iteration_count) / double(total_iteration_count);
    logs( std::to_string(progress) + "% ");
}
