#include <iostream>

// SyCL specific includes
#include <CL/sycl.hpp>
#include <array>
#include <sys/time.h>


#define PARALLEL_FOR_SIZE 1024
#define VECTOR_SIZE_PER_ITERATION 200 * 1024


#define DATA_TYPE int
#define REPEAT_COUNT 4


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

void main_sequence() {

    uint64_t t_start, t_start2;
    uint64_t t_data_generation_and_ram_allocation = 0;
    uint64_t t_queue_creation = 0;
    uint64_t t_allocation = 0;
    uint64_t t_copy_to_device = 0;
    uint64_t t_parallel_for = 0;
    uint64_t t_read_from_device = 0;
    uint64_t t_free_gpu = 0;
    uint64_t t_gpu = 0;

    log("Generating data...");
    t_start = get_ms();

    data_type *data_input  = new data_type[INPUT_DATA_LENGTH];
    data_type *data_output = new data_type[OUTPUT_DATA_LENGTH];
    data_type final_result_verif = 0;

    // Fills the array with random data
    for (int i = 0; i < INPUT_DATA_LENGTH; ++i) {
        data_input[i] = i % 10;
        final_result_verif += data_input[i];
    }

    t_data_generation_and_ram_allocation = get_ms() - t_start;

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
        t_queue_creation = get_ms() - t_start;

        // Print out the device information used for the kernel code.
        std::cout << "Running on device: "
                << sycl_q.get_device().get_info<cl::sycl::info::device::name>() << "\n";

        // ========== IMPLICIT USM ==========
        if (mode == implicit_USM) {

            for (int rpt = 0; rpt < REPEAT_COUNT; ++rpt) {
                log("Iteration " + std::to_string(rpt) + " on " + std::to_string(REPEAT_COUNT));

                t_start = get_ms();
                data_type *sdata_input  = cl::sycl::malloc_shared<data_type>(INPUT_DATA_SIZE, sycl_q);
                data_type *sdata_output = cl::sycl::malloc_shared<data_type>(OUTPUT_DATA_SIZE, sycl_q);

                if (wait_queue) sycl_q.wait_and_throw();
                t_allocation = get_ms() - t_start;
                t_start = get_ms();

                // Copy input data
                for (int i = 0; i < INPUT_DATA_LENGTH; ++i) {
                    sdata_input[i] = data_input[i];
                } 

                sycl_q.wait_and_throw();
                t_copy_to_device = get_ms() - t_start;
                t_start = get_ms();

                // Starts a kernel
                sycl_q.parallel_for(cl::sycl::range<1>(PARALLEL_FOR_SIZE), [=](cl::sycl::id<1> chunk_index) {
                    int cindex = chunk_index[0];
                    int start_index = cindex * VECTOR_SIZE_PER_ITERATION;
                    int stop_index = start_index + VECTOR_SIZE_PER_ITERATION;
                    data_type sum = 0;

                    for (int i = start_index; i < stop_index; ++i) {
                        sum += sdata_input[i];
                    }

                    sdata_output[cindex] = sum;
                });

                //if (wait_queue) 
                sycl_q.wait_and_throw();
                t_parallel_for = get_ms() - t_start;
                t_start = get_ms();

                // Value verification
                data_type total_sum = 0;
                for (int i = 0; i < OUTPUT_DATA_LENGTH; ++i) {
                    total_sum += sdata_output[i];
                }

                t_read_from_device = get_ms() - t_start;
                t_start = get_ms();

                if (total_sum == final_result_verif) {
                    log("VALID - Right data size ! (" + std::to_string(total_sum) + ")");
                } else {
                    log("ERROR - expected size " + std::to_string(final_result_verif) + " but found " + std::to_string(total_sum) + ".");
                }

                t_start = get_ms();
                cl::sycl::free(sdata_input, sycl_q);
                cl::sycl::free(sdata_output, sycl_q);
                if (wait_queue) sycl_q.wait_and_throw();
                t_free_gpu = get_ms() - t_start;
            
                t_gpu = t_allocation + t_copy_to_device + t_read_from_device
                        + t_queue_creation + t_parallel_for + t_free_gpu;
                std::cout 
                        << "t_gpu - - - - - - - - - -  = " << t_gpu << std::endl
                        //<< "t_queue_creation           = " << t_queue_creation << std::endl
                        << "t_allocation - - - - - - - = " << t_allocation << std::endl
                        << "t_copy_to_device           = " << t_copy_to_device << std::endl
                        << "t_parallel_for - - - - - - = " << t_parallel_for << std::endl
                        << "t_read_from_device         = " << t_read_from_device << std::endl
                        << "t_free_gpu - - - - - - - - = " << t_free_gpu << std::endl
                        ;

                log("");
            }
        }


        // ========== EXPLICIT USM ==========
        if (mode == explicit_USM) {

            log("Mode : EXPLICIT USM");
            
            log("============== alloc / free each time ==============");

            // Allocation and free on device, for each iteration
            for (int rpt = 0; rpt < REPEAT_COUNT; ++rpt) {
                log("Iteration " + std::to_string(rpt) + " on " + std::to_string(REPEAT_COUNT));

                t_start = get_ms();
                data_type *ddata_input = static_cast<data_type *> (cl::sycl::malloc_device(INPUT_DATA_SIZE, sycl_q));
                data_type *ddata_output = static_cast<data_type *> (cl::sycl::malloc_device(OUTPUT_DATA_SIZE, sycl_q));

                if (wait_queue) sycl_q.wait_and_throw();
                t_allocation = get_ms() - t_start;
                t_start = get_ms();

                sycl_q.memcpy(ddata_input, data_input, INPUT_DATA_SIZE);

                sycl_q.wait_and_throw();
                t_copy_to_device = get_ms() - t_start;
                t_start = get_ms();

                // Starts a kernel
                sycl_q.parallel_for(cl::sycl::range<1>(PARALLEL_FOR_SIZE), [=](cl::sycl::id<1> chunk_index) {
                    int cindex = chunk_index[0];
                    int start_index = cindex * VECTOR_SIZE_PER_ITERATION;
                    int stop_index = start_index + VECTOR_SIZE_PER_ITERATION;
                    data_type sum = 0;

                    for (int i = start_index; i < stop_index; ++i) {
                        sum += ddata_input[i];
                    }

                    ddata_output[cindex] = sum;
                });

                //if (wait_queue) 
                sycl_q.wait_and_throw();
                t_parallel_for = get_ms() - t_start;
                t_start = get_ms();

                sycl_q.memcpy(data_output, ddata_output, OUTPUT_DATA_SIZE);
                sycl_q.wait_and_throw();

                // Value verification
                data_type total_sum = 0;
                for (int i = 0; i < OUTPUT_DATA_LENGTH; ++i) {
                    total_sum += data_output[i];
                }

                t_read_from_device = get_ms() - t_start;
                t_start = get_ms();

                if (total_sum == final_result_verif) {
                    log("VALID - Right data size ! (" + std::to_string(total_sum) + ")");
                } else {
                    log("ERROR - expected size " + std::to_string(final_result_verif) + " but found " + std::to_string(total_sum) + ".");
                }

                t_start = get_ms();
                cl::sycl::free(ddata_input, sycl_q);
                cl::sycl::free(ddata_output, sycl_q);
                if (wait_queue) sycl_q.wait_and_throw();
                t_free_gpu = get_ms() - t_start;
            
                t_gpu = t_allocation + t_copy_to_device + t_read_from_device
                        + t_queue_creation + t_parallel_for + t_free_gpu;
                std::cout 
                        << "t_gpu - - - - - - - - - -  = " << t_gpu << std::endl
                        //<< "t_queue_creation           = " << t_queue_creation << std::endl
                        << "t_allocation - - - - - - - = " << t_allocation << std::endl
                        << "t_copy_to_device           = " << t_copy_to_device << std::endl
                        << "t_parallel_for - - - - - - = " << t_parallel_for << std::endl
                        << "t_read_from_device         = " << t_read_from_device << std::endl
                        << "t_free_gpu - - - - - - - - = " << t_free_gpu << std::endl
                        ;

                log("");
            }



            log("============== alloc / free only once ==============");

            t_start = get_ms();
            data_type *ddata_input = static_cast<data_type *> (cl::sycl::malloc_device(INPUT_DATA_SIZE, sycl_q));
            data_type *ddata_output = static_cast<data_type *> (cl::sycl::malloc_device(OUTPUT_DATA_SIZE, sycl_q));

            if (wait_queue) sycl_q.wait_and_throw();
            t_allocation = get_ms() - t_start;
            t_start = get_ms();

            sycl_q.memcpy(ddata_input, data_input, INPUT_DATA_SIZE);

            sycl_q.wait_and_throw();
            t_copy_to_device = get_ms() - t_start;


            // Allocation and free on device, for each iteration
            for (int rpt = 0; rpt < REPEAT_COUNT; ++rpt) {
                log("Iteration " + std::to_string(rpt) + " on " + std::to_string(REPEAT_COUNT));

                t_start = get_ms();

                // Starts a kernel
                sycl_q.parallel_for(cl::sycl::range<1>(PARALLEL_FOR_SIZE), [=](cl::sycl::id<1> chunk_index) {
                    int cindex = chunk_index[0];
                    int start_index = cindex * VECTOR_SIZE_PER_ITERATION;
                    int stop_index = start_index + VECTOR_SIZE_PER_ITERATION;
                    data_type sum = 0;

                    for (int i = start_index; i < stop_index; ++i) {
                        sum += ddata_input[i];
                    }

                    ddata_output[cindex] = sum;
                });

                //if (wait_queue) 
                sycl_q.wait_and_throw();
                t_parallel_for = get_ms() - t_start;
                t_start = get_ms();

                sycl_q.memcpy(data_output, ddata_output, OUTPUT_DATA_SIZE);
                sycl_q.wait_and_throw();

                // Value verification
                data_type total_sum = 0;
                for (int i = 0; i < OUTPUT_DATA_LENGTH; ++i) {
                    total_sum += data_output[i];
                }

                t_read_from_device = get_ms() - t_start;

                if (total_sum == final_result_verif) {
                    log("VALID - Right data size ! (" + std::to_string(total_sum) + ")");
                } else {
                    log("ERROR - expected size " + std::to_string(final_result_verif) + " but found " + std::to_string(total_sum) + ".");
                }

            
                t_gpu = t_parallel_for + t_read_from_device;
                std::cout 
                        << "t_gpu - - - - - - - - - -  = " << t_gpu << std::endl
                        << "t_parallel_for - - - - - - = " << t_parallel_for << std::endl
                        << "t_read_from_device         = " << t_read_from_device << std::endl
                        ;

                log("");
            }


            t_start = get_ms();
            cl::sycl::free(ddata_input, sycl_q);
            cl::sycl::free(ddata_output, sycl_q);
            if (wait_queue) sycl_q.wait_and_throw();
            t_free_gpu = get_ms() - t_start;

            int t_mutualisation = t_allocation + t_copy_to_device + t_free_gpu;
            std::cout 
            << "t_mutualisation  - - - - - = " << t_mutualisation << std::endl
            << "t_allocation - - - - - - - = " << t_allocation << std::endl
            << "t_copy_to_device           = " << t_copy_to_device << std::endl
            << "t_free_gpu - - - - - - - - = " << t_free_gpu << std::endl
            ;
            log("");


        }

        

        std::cout 
            << "t_data_ram_init            = " << t_data_generation_and_ram_allocation << std::endl
            << "t_queue_creation           = " << t_queue_creation << std::endl
            ;

        log("");



    } catch (cl::sycl::exception const &e) {
            std::cout << "An exception is caught while processing SyCL code.\n";
            std::terminate();
        }


    delete[] data_input;
    delete[] data_output;

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


    std::cout << "SYCL memory benchmark." << std::endl;

    main_sequence();

    return 0;

}