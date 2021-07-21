#include <iostream>
#include <fstream>
#include <chrono>

// SyCL specific includes
#include <CL/sycl.hpp>
#include <array>
#include <sys/time.h>
#include <stdlib.h>


enum sycl_mode {shared_USM, device_USM, host_USM, accessors};
//enum dataset_type {implicit_USM, device_USM, host_USM, accessors};

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

// Comparison of various USM flavours

/*#define PARALLEL_FOR_SIZE 1024
#define VECTOR_SIZE_PER_ITERATION 200 * 1024*/

unsigned int PARALLEL_FOR_SIZE;// = 1024 * 32 * 8;// = M ; work items number
unsigned int VECTOR_SIZE_PER_ITERATION;// = 1; // = L ; vector size per workitem (i.e. parallel_for task) = nb itérations internes par work item

sycl_mode CURRENT_MODE = sycl_mode::device_USM;

constexpr int MEMCOPY_IS_SYCL = 1;
int SIMD_FOR_LOOP = 0;
constexpr int USE_NAMED_KERNEL = 1; // Sandor does not support anonymous kernels.
constexpr bool KEEP_SAME_DATASETS = true; 

// faire un repeat sur les mêmes données pour essayer d'utiliser le cache
// hypothèse : les données sont évincées du cache avant de pouvoir y avoir accès
// observation : j'ai l'impression d'être un peu en train de me perdre dans les explorations,
// avoir une liste pour prioriser ce que je dois faire et 

#define DATA_TYPE float

// number of iterations - no realloc to make it go faster
#define REPEAT_COUNT_REALLOC 12
#define REPEAT_COUNT_ONLY_PARALLEL 0

//#define OUTPUT_FILE_NAME "sh_output_bench_h53.shared_txt"
#define OUTPUT_FILE_NAME "sandor_h54_L_M_1G.txt"


static std::string ver_prefix = "X38";

#define DATA_VERSION 5

// number of diffrent datasets
#define DATASET_NUMBER 1

#define CHECK_SIMD_CPU false


#define INPUT_DATA_LENGTH PARALLEL_FOR_SIZE * VECTOR_SIZE_PER_ITERATION
#define OUTPUT_DATA_LENGTH PARALLEL_FOR_SIZE


#define INPUT_DATA_SIZE INPUT_DATA_LENGTH * sizeof(DATA_TYPE)
#define OUTPUT_DATA_SIZE OUTPUT_DATA_LENGTH * sizeof(DATA_TYPE)



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

uint64_t get_ms() {
    auto tm = std::chrono::steady_clock::now();
    std::chrono::duration<double> s = tm - tm;


    struct timeval tp;
    gettimeofday(&tp, NULL);
    uint64_t ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    return ms;
}

void log(std::string str) {
    std::cout << str << std::endl;
}

// Memory intensive operation (read only)
int compute_sum(int* array, int size) {
    int sum = 0;
    for (int i = 0; i < size; ++i) {
        sum += array[i];
    }
    return sum;
}


using data_type = int;
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


void generic_USM_compute(cl::sycl::queue &sycl_q, host_dataset* dataset,
                          gpu_timer& timer, sycl_mode mode) {

    stime_utils chrono;
    chrono.start();

    //  data_type* ddata_input, data_type* ddata_output,
    data_type* ddata_input = dataset->device_input;
    data_type* ddata_output = dataset->device_output;

    //data_type* ddata_output_verif = static_cast<data_type *> (cl::sycl::malloc_device(OUTPUT_DATA_SIZE, sycl_q));

    unsigned int local_VECTOR_SIZE_PER_ITERATION = VECTOR_SIZE_PER_ITERATION;

    /*if (USE_NAMED_KERNEL == 0) {

        if (SIMD_FOR_LOOP == 0) {
            // Starts a kernel - traditional for loop
            auto e = sycl_q.parallel_for(cl::sycl::range<1>(PARALLEL_FOR_SIZE), [=](cl::sycl::id<1> chunk_index) {
                int cindex = chunk_index[0];
                int start_index = cindex * local_VECTOR_SIZE_PER_ITERATION;
                int stop_index = start_index + local_VECTOR_SIZE_PER_ITERATION;
                data_type sum = 0;

                for (int i = start_index; i < stop_index; ++i) {
                    sum += ddata_input[i];
                }

                ddata_output[cindex] = sum;
                //ddata_output_verif[cindex] = sum;
            });
            e.wait();
        } else {
            // Starts a kernel - SIMD optimized for loop
            auto e = sycl_q.parallel_for(cl::sycl::range<1>(PARALLEL_FOR_SIZE), [=](cl::sycl::id<1> chunk_index) {
                int cindex = chunk_index[0];
                //int start_index = cindex * local_VECTOR_SIZE_PER_ITERATION;
                //int stop_index = start_index + local_VECTOR_SIZE_PER_ITERATION;
                data_type sum = 0;

                for (int it = 0; it < local_VECTOR_SIZE_PER_ITERATION; ++it) {
                    int iindex = cindex + it * PARALLEL_FOR_SIZE;
                    sum += ddata_input[iindex];
                }

                

                ddata_output[cindex] = sum;
                //ddata_output_verif[cindex] = sum;
            });
            e.wait();
        }
    } else {*/

    // Sandor does not support anonymous kernels.

    class MyKernel_a;
    class MyKernel_b;

    if (SIMD_FOR_LOOP == 0) {
        // Starts a kernel - traditional for loop
        auto e = sycl_q.parallel_for<MyKernel_a>(cl::sycl::range<1>(PARALLEL_FOR_SIZE), [=](cl::sycl::id<1> chunk_index) {
            int cindex = chunk_index[0];
            int start_index = cindex * local_VECTOR_SIZE_PER_ITERATION;
            int stop_index = start_index + local_VECTOR_SIZE_PER_ITERATION;
            data_type sum = 0;

            for (int i = start_index; i < stop_index; ++i) {
                sum += ddata_input[i];
            }

            ddata_output[cindex] = sum;
            //ddata_output_verif[cindex] = sum;
        });
        e.wait();
    } else {
        // Starts a kernel - SIMD optimized for loop
        auto e = sycl_q.parallel_for<MyKernel_b>(cl::sycl::range<1>(PARALLEL_FOR_SIZE), [=](cl::sycl::id<1> chunk_index) {
            int cindex = chunk_index[0];
            //int start_index = cindex * local_VECTOR_SIZE_PER_ITERATION;
            //int stop_index = start_index + local_VECTOR_SIZE_PER_ITERATION;
            data_type sum = 0;

            for (int it = 0; it < local_VECTOR_SIZE_PER_ITERATION; ++it) {
                int iindex = cindex + it * PARALLEL_FOR_SIZE;
                sum += ddata_input[iindex];
            }

            ddata_output[cindex] = sum;
            //ddata_output_verif[cindex] = sum;
        });
        e.wait();
    }

    //}

    sycl_q.wait_and_throw();
    timer.t_parallel_for = chrono.reset();

    std::cout << ver_prefix + " - COMPUTE MODE = " + mode_to_string(mode) << std::endl;
    if ( mode == sycl_mode::device_USM ) {
        //std::cout << "MODE = DEVICE USM OK" << std::endl;
        //if (MEMCOPY_IS_SYCL) 
        sycl_q.memcpy(dataset->data_output, ddata_output, OUTPUT_DATA_SIZE);
        sycl_q.wait_and_throw();
    }

    //sycl_q.memcpy(dataset->data_output, ddata_output_verif, OUTPUT_DATA_SIZE);
    //sycl_q.wait_and_throw();

    data_type sum_simd_check_cpu = 0;



    // Value verification
    data_type total_sum = 0;
    if ( mode == sycl_mode::device_USM ) {
        // device USM : memory has been copied to dataset->data_output
        // because data in dataset->device_output is only accessible by the device
        for (int i = 0; i < OUTPUT_DATA_LENGTH; ++i) {
            total_sum += dataset->data_output[i]; //ddata_output_verif[i];//dataset->data_output[i];
        }
    }

    if ( (mode == sycl_mode::host_USM)
    ||   (mode == sycl_mode::shared_USM) ) {
        // memory accessible by the host : data already in dataset->device_output
        // and since data is shared, it can be accessed directly
        for (int i = 0; i < OUTPUT_DATA_LENGTH; ++i) {
            total_sum += dataset->device_output[i]; //ddata_output_verif[i];//dataset->data_output[i];
        }
    }

    //cl::sycl::free(ddata_output_verif, sycl_q);
    timer.t_read_from_device = chrono.reset();

    if (total_sum == dataset->final_result_verif) {
        log("VALID - Right data size ! (" + std::to_string(total_sum) + ")");
    } else {
        log("ERROR - expected size " + std::to_string(dataset->final_result_verif) + " but found " + std::to_string(total_sum) + ".");
    }
}


void generic_USM_allocation(cl::sycl::queue &sycl_q, host_dataset *dataset, gpu_timer& timer, sycl_mode mode) {
    stime_utils chrono;
    chrono.start();
    
    std::cout << ver_prefix + " - ALLOC MODE = " + mode_to_string(mode) << std::endl;
    switch (mode) {
    case sycl_mode::device_USM :
        dataset->device_input = static_cast<data_type *> (cl::sycl::malloc_device(INPUT_DATA_SIZE, sycl_q));
        dataset->device_output = static_cast<data_type *> (cl::sycl::malloc_device(OUTPUT_DATA_SIZE, sycl_q));
        break;
    case sycl_mode::host_USM :
        dataset->device_input = static_cast<data_type *> (cl::sycl::malloc_host(INPUT_DATA_SIZE, sycl_q));
        dataset->device_output = static_cast<data_type *> (cl::sycl::malloc_host(OUTPUT_DATA_SIZE, sycl_q));
        break;
    case sycl_mode::shared_USM :
        dataset->device_input = static_cast<data_type *> (cl::sycl::malloc_shared(INPUT_DATA_SIZE, sycl_q));
        dataset->device_output = static_cast<data_type *> (cl::sycl::malloc_shared(OUTPUT_DATA_SIZE, sycl_q));
        break;
    }
    
    sycl_q.wait_and_throw();
    timer.t_allocation = chrono.reset();

    // Explicit copy only if malloc_device
    if ( (mode == sycl_mode::device_USM)
    ||   (mode == sycl_mode::host_USM)
    ||   (mode == sycl_mode::shared_USM) ) {

        // The only way to copy data to the device is to use sycl_q.memcpy
        // For shared memory (host and shared), glibc memcpy can be used.
        if ( (MEMCOPY_IS_SYCL == 1) || (mode == sycl_mode::device_USM) ) {
            std::cout << "MEM - SYCL MEMCOPY ----\n";
            sycl_q.memcpy(dataset->device_input, dataset->data_input, INPUT_DATA_SIZE).wait();
            sycl_q.wait_and_throw();
        } else {
            std::cout << "MEM - GLIBC MEMCOPY ----\n";
            // Probably works with host and shared, most likely does not work with device
            memcpy(dataset->device_input, dataset->data_input, INPUT_DATA_SIZE);
        }
        timer.t_copy_to_device = chrono.reset();
    }

    // TODO : also copy data from dataset to shared memory.



}

void generic_USM_free(cl::sycl::queue &sycl_q, host_dataset* dataset, gpu_timer& timer, sycl_mode mode) {
    stime_utils chrono;
    chrono.start();

    std::cout << ver_prefix + " - FREE MODE = " + mode_to_string(mode) << std::endl;

    cl::sycl::free(dataset->device_input, sycl_q);
    cl::sycl::free(dataset->device_output, sycl_q);
    sycl_q.wait_and_throw();
    timer.t_free_gpu = chrono.reset();
    dataset->device_input = nullptr;
    dataset->device_output = nullptr;
}


/*void host_USM_allocation(cl::sycl::queue &sycl_q, host_dataset *dataset, gpu_timer& timer, sycl_mode mode) {
    stime_utils chrono;
    chrono.start();
    
    dataset->device_input = static_cast<data_type *> (cl::sycl::malloc_host(INPUT_DATA_SIZE, sycl_q));
    dataset->device_output = static_cast<data_type *> (cl::sycl::malloc_host(OUTPUT_DATA_SIZE, sycl_q));

    sycl_q.wait_and_throw();
    timer.t_allocation = chrono.reset();

    //sycl_q.memcpy(dataset->device_input, dataset->data_input, INPUT_DATA_SIZE);
    
    //sycl_q.wait_and_throw();
    //timer.t_copy_to_device = chrono.reset();
}


void host_USM_free(cl::sycl::queue &sycl_q, host_dataset* dataset, gpu_timer& timer, sycl_mode mode) {
    stime_utils chrono;
    chrono.start();

    cl::sycl::free(dataset->device_input, sycl_q);
    cl::sycl::free(dataset->device_output, sycl_q);
    sycl_q.wait_and_throw();
    timer.t_free_gpu = chrono.reset();
    dataset->device_input = nullptr;
    dataset->device_output = nullptr;
}*/




void print_timer_iter_alloc(gpu_timer& time) {
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
    //uint64_t t_gpu;
    //t_gpu = time.t_read_from_device + time.t_parallel_for;
    std::cout 
            << "t_parallel_for - - - - - - = " << time.t_parallel_for << std::endl
            << "t_read_from_device         = " << time.t_read_from_device << std::endl
            ;

    log("");
}

void print_timer_alloc(gpu_timer& time) {
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


host_dataset* generate_datasets() {

    log("Generating data...");
    stime_utils chrono;
    chrono.start();

    host_dataset *hdata = new host_dataset[DATASET_NUMBER];

    for (int i = 0; i < DATASET_NUMBER; ++i) {
        host_dataset *hd = &hdata[i];

        hd->data_input = new data_type[INPUT_DATA_LENGTH];
        hd->data_output = new data_type[OUTPUT_DATA_LENGTH];
        hd->seed = 452 + i * 68742;

        srand(hd->seed);

        // Fills the array with random data
        for (int i = 0; i < INPUT_DATA_LENGTH; ++i) {
            data_type v = rand();
            hd->data_input[i] = v;
            hd->final_result_verif += v;
        }

        // Perform SMID-like operations to verify the sum algorithm
        if (CHECK_SIMD_CPU) {
            data_type sum_simd_check_cpu = 0;
            // SIMD-like check
            for (int ip = 0; ip < PARALLEL_FOR_SIZE; ++ip) {
                for (int it = 0; it < VECTOR_SIZE_PER_ITERATION; ++it) {
                    int iindex = ip + it * PARALLEL_FOR_SIZE;
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

void delete_datasets(host_dataset* hdata) {
    if (hdata == nullptr) return;

    for (int i = 0; i < DATASET_NUMBER; ++i) {
        host_dataset *hd = &hdata[i];
        delete[] hd->data_input;
        delete[] hd->data_output;
    }
    delete[] hdata;
}

host_dataset *global_persistent_datasets = nullptr;

void main_sequence(std::ofstream& write_file, sycl_mode mode) {

    // Pointers to allocation, compute and free SYCL functions.
    void (*sycl_allocation)(cl::sycl::queue &, host_dataset *, gpu_timer &, sycl_mode mode);
    void (*sycl_compute)(cl::sycl::queue &, host_dataset *, gpu_timer &, sycl_mode mode);
    void (*sycl_free)(cl::sycl::queue &, host_dataset *, gpu_timer &, sycl_mode mode);

    switch (mode) {
    case sycl_mode::device_USM :
        sycl_allocation = generic_USM_allocation;
        sycl_compute = generic_USM_compute;
        sycl_free = generic_USM_free;
        break;

    case sycl_mode::host_USM :
        sycl_allocation = generic_USM_allocation;
        sycl_compute = generic_USM_compute;
        sycl_free = generic_USM_free;
        break;

    case sycl_mode::shared_USM :
        sycl_allocation = generic_USM_allocation;
        sycl_compute = generic_USM_compute;
        sycl_free = generic_USM_free;
        break;

    }

    uint64_t t_start, t_start2;
    gpu_timer gtimer;

    stime_utils chrono;

    //log("Generating data...");

    chrono.start();
    //t_start = get_ms();
    host_dataset *hdata;

    if (KEEP_SAME_DATASETS) {
        if (global_persistent_datasets == nullptr) {
            // First generation, if no dataset already present
            global_persistent_datasets = generate_datasets();
        }
        hdata = global_persistent_datasets;
    } else {
        // New dataset for each run of main_sequence
        hdata = generate_datasets();
    }

    /*host_dataset *hdata = new host_dataset[DATASET_NUMBER];

    for (int i = 0; i < DATASET_NUMBER; ++i) {
        host_dataset *hd = &hdata[i];

        hd->data_input = new data_type[INPUT_DATA_LENGTH];
        hd->data_output = new data_type[OUTPUT_DATA_LENGTH];
        hd->seed = 452 + i * 68742;

        srand(hd->seed);

        // Fills the array with random data
        for (int i = 0; i < INPUT_DATA_LENGTH; ++i) {
            data_type v = rand();
            hd->data_input[i] = v;
            hd->final_result_verif += v;
        }

        // Perform SMID-like operations to verify the sum algorithm
        if (CHECK_SIMD_CPU) {
            data_type sum_simd_check_cpu = 0;
            // SIMD-like check
            for (int ip = 0; ip < PARALLEL_FOR_SIZE; ++ip) {
                for (int it = 0; it < VECTOR_SIZE_PER_ITERATION; ++it) {
                    int iindex = ip + it * PARALLEL_FOR_SIZE;
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
    }*/

    gtimer.t_data_generation_and_ram_allocation = global_t_data_generation_and_ram_allocation; //chrono.reset(); //get_ms() - t_start;

    log("\n");
    log("Input data size  : " + std::to_string(INPUT_DATA_SIZE)
        + " (" + std::to_string(INPUT_DATA_SIZE / (1024*1024)) + " MiB)");
    log("Output data size : " + std::to_string(OUTPUT_DATA_SIZE)
        + " (" + std::to_string(OUTPUT_DATA_SIZE / (1024*1024)) + " MiB)");
    log("\n");

    // The default device selector will select the most performant device.
    cl::sycl::default_selector d_selector;

    try {
        chrono.reset(); //t_start = get_ms();
        cl::sycl::queue sycl_q(d_selector, exception_handler);
        sycl_q.wait_and_throw();
        gtimer.t_queue_creation = chrono.reset();//get_ms() - t_start;

        // Print out the device information used for the kernel code.
        std::cout << "Running on device: "
                << sycl_q.get_device().get_info<cl::sycl::info::device::name>() << "\n";

        // ========== RIP IMPLICIT USM RIP ==========
        


        // ========== EXPLICIT USM ==========
        //if (mode == sycl_mode::device_USM) {
        //int change_data_round = REPEAT_COUNT / 2;

        //log("Mode : EXPLICIT USM");
        
        log("============== alloc / free each time ==============");

        // INPUT_DATA_SIZE OUTPUT_DATA_SIZE PARALLEL_FOR_SIZE VECTOR_SIZE_PER_ITERATION REPEAT_COUNT
        // t_data_generation_and_ram_allocation t_queue_creation
        write_file << DATASET_NUMBER << " "
                    << INPUT_DATA_SIZE << " "
                    << OUTPUT_DATA_SIZE << " "
                    << PARALLEL_FOR_SIZE << " "
                    << VECTOR_SIZE_PER_ITERATION << " "
                    << REPEAT_COUNT_REALLOC << " "
                    << REPEAT_COUNT_ONLY_PARALLEL << " "
                    << gtimer.t_data_generation_and_ram_allocation << " "
                    << gtimer.t_queue_creation << " "
                    << mode_to_int(mode) << " "
                    << MEMCOPY_IS_SYCL << " " // flag to indicate if sycl mem copy or glibc mem copy
                    << SIMD_FOR_LOOP << " " // flag to indicate wether a traditional for loop was used, or a SIMD GPU-specific loop
                    << USE_NAMED_KERNEL // flag to indicate if the named kernel was used or traditional lambda kernel
                    << "\n";

        log("\n######## ALLOCATION, COPY AND FREE FOR EACH ITERATION ########");

        // Allocation, copy and free each time
        for (int ids = 0; ids < DATASET_NUMBER; ++ids) {
            
            host_dataset* dataset = &hdata[ids];
            write_file << dataset->seed << "\n";

            log("------- DATASET SEED " + std::to_string(dataset->seed) + " -------\n");

            // Allocation and free on device, for each iteration
            for (int rpt = 0; rpt < REPEAT_COUNT_REALLOC; ++rpt) {
                log("Iteration " + std::to_string(rpt+1) + " on " + std::to_string(REPEAT_COUNT_REALLOC));

                sycl_allocation(sycl_q, dataset, gtimer, mode);
                sycl_compute(sycl_q, dataset, gtimer, mode);
                sycl_free(sycl_q, dataset, gtimer, mode);
                

                write_file << gtimer.t_allocation << " "
                            << gtimer.t_copy_to_device << " "
                            << gtimer.t_parallel_for << " " 
                            << gtimer.t_read_from_device << " "
                            << gtimer.t_free_gpu
                            << "\n";

                // New line in file, output format :
                // ... see above ...
                // A new line for each repeat count :
                // t_allocation t_copy_to_device t_parallel_for t_read_from_device t_free_gpu
                print_timer_iter_alloc(gtimer);
            }
        }

        log("\n######## ALLOCATION, COPY AND FREE ONLY ONCE ########");

        // Allocation, copy and free once
        for (int ids = 0; ids < DATASET_NUMBER; ++ids) {
            
            host_dataset* dataset = &hdata[ids];
            write_file << dataset->seed << "\n";

            log("------- DATASET SEED " + std::to_string(dataset->seed) + " -------\n");
            sycl_allocation(sycl_q, dataset, gtimer, mode);

            // Allocation and free on device, for each iteration
            for (int rpt = 0; rpt < REPEAT_COUNT_ONLY_PARALLEL; ++rpt) {
                log("Iteration " + std::to_string(rpt+1) + " on " + std::to_string(REPEAT_COUNT_ONLY_PARALLEL));
                
                sycl_compute(sycl_q, dataset, gtimer, mode);
                
                write_file << gtimer.t_parallel_for << " " 
                            << gtimer.t_read_from_device << " "
                            << "\n";

                // New line in file, output format :
                // ... see above ...
                // A new line for each repeat count :
                // t_allocation t_copy_to_device t_parallel_for t_read_from_device t_free_gpu
                print_timer_iter(gtimer);
            }
            sycl_free(sycl_q, dataset, gtimer, mode);

            write_file << gtimer.t_allocation << " "
                        << gtimer.t_copy_to_device << " "
                        << gtimer.t_free_gpu
                        << "\n";

            print_timer_alloc(gtimer);
        }

        //}

        

        std::cout 
            << "t_data_ram_init            = " << gtimer.t_data_generation_and_ram_allocation << std::endl
            << "t_queue_creation           = " << gtimer.t_queue_creation << std::endl
            ;

        log("");



    } catch (cl::sycl::exception const &e) {
        std::cout << "An exception has been caught while processing SyCL code.\n";
        std::terminate();
    }

    if ( ! KEEP_SAME_DATASETS ) {
        // Delete local datasets
        delete_datasets(hdata);
    }

    log("Bye.");
}


void bench_smid_modes(std::ofstream& myfile) {

    unsigned int total_elements = 1024 * 1024 * 256; // 256 * bytes = 1 GiB.
    VECTOR_SIZE_PER_ITERATION = 2048;
    PARALLEL_FOR_SIZE = total_elements / VECTOR_SIZE_PER_ITERATION; // = 131072

    int imode;
    //MEMCOPY_IS_SYCL = 1;
    SIMD_FOR_LOOP = 0;
    //USE_NAMED_KERNEL = 0;

    for (int imcp = 0; imcp < 2; ++imcp) {
        SIMD_FOR_LOOP = imcp;

        for (int imode = 0; imode <= 2; ++imode) {
            
            switch (imode) {
            case 0: CURRENT_MODE = sycl_mode::shared_USM; break;
            case 1: CURRENT_MODE = sycl_mode::device_USM; break;
            case 2: CURRENT_MODE = sycl_mode::host_USM; break;
            default : break;
            }
            
            log("============    - L = VECTOR_SIZE_PER_ITERATION = " + std::to_string(VECTOR_SIZE_PER_ITERATION));
            log("============    - M = PARALLEL_FOR_SIZE = " + std::to_string(PARALLEL_FOR_SIZE));
            main_sequence(myfile, CURRENT_MODE);
        }
    }
}

void bench_choose_L_M(std::ofstream& myfile) {

    unsigned int total_elements = 1024 * 1024 * 256; // 256 * bytes = 1 GiB.

    int imode;
    //MEMCOPY_IS_SYCL = 1;
    SIMD_FOR_LOOP = 0;
    //USE_NAMED_KERNEL = 0;

    int start_L_size = 64;
    int stop_M_size = 4096; // inclusive
    int stop_L_size = total_elements / stop_M_size;

    for (VECTOR_SIZE_PER_ITERATION = start_L_size; VECTOR_SIZE_PER_ITERATION <= stop_L_size; VECTOR_SIZE_PER_ITERATION *= 2) {
        PARALLEL_FOR_SIZE = total_elements / VECTOR_SIZE_PER_ITERATION;

        for (int imode = 0; imode <= 1; ++imode) {
            
            switch (imode) {
            case 0: CURRENT_MODE = sycl_mode::shared_USM; break;
            case 1: CURRENT_MODE = sycl_mode::device_USM; break;
            //case 2: CURRENT_MODE = sycl_mode::host_USM; break;
            default : break;
            }
            
            log("============    - L = VECTOR_SIZE_PER_ITERATION = " + std::to_string(VECTOR_SIZE_PER_ITERATION));
            log("============    - M = PARALLEL_FOR_SIZE = " + std::to_string(PARALLEL_FOR_SIZE));
            main_sequence(myfile, CURRENT_MODE);
        }
    }
}


int main(int argc, char *argv[])
{
    // Not sure I'll need that
    /*if (argc < 2){
        std::cout << "Not enough arguments, minimum requirement: " << std::endl;
        std::cout << "./exe <data_path>" << std::endl;
        return -1;
    }
    auto data_path = std::string(argv[1]);*/
    
    std::ofstream myfile;
    myfile.open ("/home/data_sync/academique/M2/StageM2/SYCL_tests/mem_bench/" + std::string(OUTPUT_FILE_NAME));
    

    myfile << DATA_VERSION << "\n";

    std::cout << "======= FILE VERSION =======" << std::endl;
    std::cout << "======= FILE VERSION =======" << std::endl;
    std::cout << "   SYCL memory benchmark.   " << std::endl;
    std::cout << "======= FILE VERSION =======" << std::endl;
    std::cout << "======= FILE VERSION =======" << std::endl;
    
    std::cout << OUTPUT_FILE_NAME << std::endl;
    std::cout << OUTPUT_FILE_NAME << std::endl;
    std::cout << OUTPUT_FILE_NAME << std::endl;

    bench_choose_L_M(myfile);

    //PARALLEL_FOR_SIZE = 128;//1024;
    //VECTOR_SIZE_PER_ITERATION = 256 * 1024 * 8;

    /*
    1. Fixer L : taille du vecteur par workitem
    2. Fixer M : nombre de workitems
    3. Lancer ni itérations sur nd datasets distincts, pour comparer le temps pris entre

    Nécessaire de trouver l'endroit où les données tiennent en cache et l'endroit où elles ne tiennent plus
    
    */

    // see above, header file 
    //VECTOR_SIZE_PER_ITERATION = 8 * 1024;
    //PARALLEL_FOR_SIZE = 1024 * 32; // work items number (30M items)

    // todo : affichage graphique de L et M sur les graphiques


    // Do that for each mode

    
    /*log("=============== L = " + std::to_string(VECTOR_SIZE_PER_ITERATION));
    log("=============== M = " + std::to_string(PARALLEL_FOR_SIZE));

    main_sequence(myfile, CURRENT_MODE);*/

    /*
    
    
    
    */

    /*

    //PARALLEL_FOR_SIZE = 0; // = M
    for (VECTOR_SIZE_PER_ITERATION = 1; VECTOR_SIZE_PER_ITERATION < 10; ++VECTOR_SIZE_PER_ITERATION) { // = L
        log("----============    - VECTOR_SIZE_PER_ITERATION = " + std::to_string(VECTOR_SIZE_PER_ITERATION));
        // 134217728
        // 268435456
        // 8388608
        for (PARALLEL_FOR_SIZE = 128; PARALLEL_FOR_SIZE < 128 * 8; PARALLEL_FOR_SIZE += 128) {
        //for (PARALLEL_FOR_SIZE = 1; PARALLEL_FOR_SIZE < 256 * 256 * 128; PARALLEL_FOR_SIZE *= 2) {
            log("============    - PARALLEL_FOR_SIZE = " + std::to_string(PARALLEL_FOR_SIZE));
            main_sequence(myfile);
            //PARALLEL_FOR_SIZE *= 2;
            //VECTOR_SIZE_PER_ITERATION /= 2;
        }
    }*/

    myfile.close();
    log("OK, done.");

    if ( KEEP_SAME_DATASETS ) {
        // Delete local datasets
        delete_datasets(global_persistent_datasets);
        global_persistent_datasets = nullptr;
    }

    return 0;

}

/*
To run with syclcc, set those variables :
export HIPSYCL_TARGETS="cuda:sm_35" && \
export HIPSYCL_GPU_ARCH="sm_35" && \
export HIPSYCL_CUDA_PATH="/usr/local/cuda-10.1"


On Sandor :
export HIPSYCL_TARGETS="cuda:sm_75" && \
export HIPSYCL_GPU_ARCH="sm_75" && \
export HIPSYCL_CUDA_PATH="/usr/local/cuda-10.1"

ERROR :

memory_benchmark_file_output_cmp.cpp:177:73: error: Optional kernel lambda naming requires clang >= 10
    auto e = sycl_q.parallel_for(cl::sycl::range<1>(PARALLEL_FOR_SIZE), [=](cl::sycl::id<1> chunk_index) {
                                                                        ^
2 warnings and 1 error generated when compiling for sm_75.

clangs :

clang                        clang-cl-11                  clang-offload-bundler-11
clang++                      clang-cpp                    clang-offload-wrapper-11
clang++-11                   clang-cpp-11                 clang-query
clang-11                     clang-doc-11                 clang-query-11
clang-9                      clang-extdef-mapping         clang-refactor
clang-apply-replacements     clang-extdef-mapping-11      clang-refactor-11
clang-apply-replacements-11  clang-format                 clang-rename
clang-change-namespace-11    clang-import-test            clang-rename-11
clang-check                  clang-include-fixer-11       clang-reorder-fields-11
clang-check-11               clang-move-11                clang-scan-deps
clang-cl                     clang-offload-bundler        clang-scan-deps-11

*/