#include <iostream>
#include <fstream>

// SyCL specific includes
#include <CL/sycl.hpp>
#include <array>
#include <sys/time.h>
#include <stdlib.h>


/*#define PARALLEL_FOR_SIZE 1024
#define VECTOR_SIZE_PER_ITERATION 200 * 1024*/

unsigned int PARALLEL_FOR_SIZE;// = 1024; = M
unsigned int VECTOR_SIZE_PER_ITERATION;// = 256 * 1024; = L


#define DATA_TYPE int

// number of iterations - no realloc to make it go faster
#define REPEAT_COUNT_REALLOC 0
#define REPEAT_COUNT_ONLY_PARALLEL 6

#define DATA_VERSION 2

// number of diffrent datasets
#define DATASET_NUMBER 1


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

uint64_t get_ms() {
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

enum Sycl_mode {implicit_USM, explicit_USM, accessors};

using data_type = int;
static Sycl_mode mode = explicit_USM;
static bool wait_queue = true;

struct host_dataset {
    data_type *data_input;
    data_type *data_output;
    data_type final_result_verif = 0;
    unsigned int seed;
    // Memory allocated on the device
    data_type *device_input = nullptr;
    data_type *device_output = nullptr;
};

struct gpu_timer {
    uint64_t t_data_generation_and_ram_allocation = 0;
    uint64_t t_queue_creation = 0;
    uint64_t t_allocation = 0;
    uint64_t t_copy_to_device = 0;
    uint64_t t_parallel_for = 0;
    uint64_t t_read_from_device = 0;
    uint64_t t_free_gpu = 0;
};


void explicit_USM_compute(cl::sycl::queue &sycl_q, host_dataset* dataset,
                          gpu_timer& timer) {
    uint64_t t_start;
    t_start = get_ms();

    //  data_type* ddata_input, data_type* ddata_output,
    data_type* ddata_input = dataset->device_input;
    data_type* ddata_output = dataset->device_output;

    unsigned int local_VECTOR_SIZE_PER_ITERATION = VECTOR_SIZE_PER_ITERATION;

    // Starts a kernel
    sycl_q.parallel_for(cl::sycl::range<1>(PARALLEL_FOR_SIZE), [=](cl::sycl::id<1> chunk_index) {
        int cindex = chunk_index[0];
        int start_index = cindex * local_VECTOR_SIZE_PER_ITERATION;
        int stop_index = start_index + local_VECTOR_SIZE_PER_ITERATION;
        data_type sum = 0;

        for (int i = start_index; i < stop_index; ++i) {
            sum += ddata_input[i];
        }

        ddata_output[cindex] = sum;
    });

    //if (wait_queue) 
    sycl_q.wait_and_throw();
    timer.t_parallel_for = get_ms() - t_start;
    t_start = get_ms();

    sycl_q.memcpy(dataset->data_output, ddata_output, OUTPUT_DATA_SIZE);
    sycl_q.wait_and_throw();

    // Value verification
    data_type total_sum = 0;
    for (int i = 0; i < OUTPUT_DATA_LENGTH; ++i) {
        total_sum += dataset->data_output[i];
    }

    timer.t_read_from_device = get_ms() - t_start;
    t_start = get_ms();

    if (total_sum == dataset->final_result_verif) {
        log("VALID - Right data size ! (" + std::to_string(total_sum) + ")");
    } else {
        log("ERROR - expected size " + std::to_string(dataset->final_result_verif) + " but found " + std::to_string(total_sum) + ".");
    }
}

void explicit_USM_allocation(cl::sycl::queue &sycl_q, host_dataset* dataset, gpu_timer& timer) {
    uint64_t t_start = get_ms();
    dataset->device_input = static_cast<data_type *> (cl::sycl::malloc_device(INPUT_DATA_SIZE, sycl_q));
    dataset->device_output = static_cast<data_type *> (cl::sycl::malloc_device(OUTPUT_DATA_SIZE, sycl_q));

    if (wait_queue) sycl_q.wait_and_throw();
    timer.t_allocation = get_ms() - t_start;
    t_start = get_ms();

    sycl_q.memcpy(dataset->device_input, dataset->data_input, INPUT_DATA_SIZE);
    
    sycl_q.wait_and_throw();
    timer.t_copy_to_device = get_ms() - t_start;
}


void explicit_USM_free(cl::sycl::queue &sycl_q, host_dataset* dataset, gpu_timer& timer) {
    uint64_t t_start = get_ms();
    cl::sycl::free(dataset->device_input, sycl_q);
    cl::sycl::free(dataset->device_output, sycl_q);
    if (wait_queue) sycl_q.wait_and_throw();
    timer.t_free_gpu = get_ms() - t_start;
    dataset->device_input = nullptr;
    dataset->device_output = nullptr;
}

void explicit_USM_iteration(cl::sycl::queue &sycl_q, host_dataset* dataset, gpu_timer& timer) {



}

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


void main_sequence(std::ofstream& write_file) {

    

    uint64_t t_start, t_start2;
    /*uint64_t t_data_generation_and_ram_allocation = 0;
    uint64_t t_queue_creation = 0;
    uint64_t t_allocation = 0;
    uint64_t t_copy_to_device = 0;
    uint64_t t_parallel_for = 0;
    uint64_t t_read_from_device = 0;
    uint64_t t_free_gpu = 0;
    uint64_t t_gpu = 0;*/
    gpu_timer gtimer;

    log("Generating data...");
    t_start = get_ms();

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

    }

    gtimer.t_data_generation_and_ram_allocation = get_ms() - t_start;

    log("\n");
    log("Input data size  : " + std::to_string(INPUT_DATA_SIZE)
        + " (" + std::to_string(INPUT_DATA_SIZE / (1024*1024)) + " MiB)");
    log("Output data size : " + std::to_string(OUTPUT_DATA_SIZE)
        + " (" + std::to_string(OUTPUT_DATA_SIZE / (1024*1024)) + " MiB)");
    log("\n");

    // The default device selector will select the most performant device.
    cl::sycl::default_selector d_selector;

    try {
        t_start = get_ms();
        cl::sycl::queue sycl_q(d_selector, exception_handler);
        sycl_q.wait_and_throw();
        gtimer.t_queue_creation = get_ms() - t_start;

        // Print out the device information used for the kernel code.
        std::cout << "Running on device: "
                << sycl_q.get_device().get_info<cl::sycl::info::device::name>() << "\n";

        // ========== RIP IMPLICIT USM RIP ==========
        


        // ========== EXPLICIT USM ==========
        if (mode == explicit_USM) {
            //int change_data_round = REPEAT_COUNT / 2;

            log("Mode : EXPLICIT USM");
            
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
                       << gtimer.t_queue_creation
                       << "\n";

            log("\n######## ALLOCATION, COPY AND FREE FOR EACH ITERATION ########");

            // Allocation, copy and free each time
            for (int ids = 0; ids < DATASET_NUMBER; ++ids) {
                
                host_dataset* dataset = &hdata[ids];
                write_file << dataset->seed << "\n";

                log("------- DATASET SEED " + std::to_string(dataset->seed) + " -------\n");

                // Allocation and free on device, for each iteration
                for (int rpt = 0; rpt < REPEAT_COUNT_REALLOC; ++rpt) {
                    log("Iteration " + std::to_string(rpt) + " on " + std::to_string(REPEAT_COUNT_REALLOC));

                    explicit_USM_allocation(sycl_q, dataset, gtimer);
                    explicit_USM_compute(sycl_q, dataset, gtimer);
                    explicit_USM_free(sycl_q, dataset, gtimer);
                    

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
                explicit_USM_allocation(sycl_q, dataset, gtimer);

                // Allocation and free on device, for each iteration
                for (int rpt = 0; rpt < REPEAT_COUNT_ONLY_PARALLEL; ++rpt) {
                    log("Iteration " + std::to_string(rpt) + " on " + std::to_string(REPEAT_COUNT_ONLY_PARALLEL));
                    
                    explicit_USM_compute(sycl_q, dataset, gtimer);
                    
                    write_file << gtimer.t_parallel_for << " " 
                               << gtimer.t_read_from_device << " "
                               << "\n";

                    // New line in file, output format :
                    // ... see above ...
                    // A new line for each repeat count :
                    // t_allocation t_copy_to_device t_parallel_for t_read_from_device t_free_gpu
                    print_timer_iter(gtimer);
                }
                explicit_USM_free(sycl_q, dataset, gtimer);

                write_file << gtimer.t_allocation << " "
                           << gtimer.t_copy_to_device << " "
                           << gtimer.t_free_gpu
                           << "\n";

                print_timer_alloc(gtimer);
            }

        }

        

        std::cout 
            << "t_data_ram_init            = " << gtimer.t_data_generation_and_ram_allocation << std::endl
            << "t_queue_creation           = " << gtimer.t_queue_creation << std::endl
            ;

        log("");



    } catch (cl::sycl::exception const &e) {
            std::cout << "An exception is caught while processing SyCL code.\n";
            std::terminate();
        }


    for (int i = 0; i < DATASET_NUMBER; ++i) {
        host_dataset *hd = &hdata[i];
        delete[] hd->data_input;
        delete[] hd->data_output;
    }
    delete[] hdata;

    log("Bye.");
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
    myfile.open ("/home/data_sync/academique/M2/StageM2/SYCL_tests/mem_bench/output_bench_h6.txt");

    myfile << DATA_VERSION << "\n";

    std::cout << "======= FILE VERSION =======" << std::endl;
    std::cout << "======= FILE VERSION =======" << std::endl;
    std::cout << "   SYCL memory benchmark.   " << std::endl;
    std::cout << "======= FILE VERSION =======" << std::endl;
    std::cout << "======= FILE VERSION =======" << std::endl;


    //PARALLEL_FOR_SIZE = 128;//1024;
    //VECTOR_SIZE_PER_ITERATION = 256 * 1024 * 8;

    VECTOR_SIZE_PER_ITERATION = 1;
    PARALLEL_FOR_SIZE = 1;

    //PARALLEL_FOR_SIZE = 0; // = M
    //for (VECTOR_SIZE_PER_ITERATION = 1; VECTOR_SIZE_PER_ITERATION < 10; ++VECTOR_SIZE_PER_ITERATION) { // = L
        // 134217728
        // 268435456
        //for (; PARALLEL_FOR_SIZE < 8388608 * 8; PARALLEL_FOR_SIZE += 8388608) {
        //for (PARALLEL_FOR_SIZE = 1; PARALLEL_FOR_SIZE < 256 * 256 * 128; PARALLEL_FOR_SIZE *= 2) {
            log("    - PARALLEL_FOR_SIZE = " + std::to_string(PARALLEL_FOR_SIZE));
            main_sequence(myfile);
            //PARALLEL_FOR_SIZE *= 2;
            //VECTOR_SIZE_PER_ITERATION /= 2;
        /*}
    }*/

    myfile.close();

    return 0;

}