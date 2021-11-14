#include <iostream>
#include <filesystem>
#include <fstream>
#include <chrono>

// file
#include <sys/stat.h>
#include <unistd.h>
#include <string>

// SyCL specific includes
#include <CL/sycl.hpp>
#include <array>
#include <sys/time.h>
#include <stdlib.h>

#include "utils.h"
#include "constants.h"
#include "traccc_fcts.h"
#include "sycl_helloworld.h"



// Comparison of various USM flavours


void generic_USM_compute(cl::sycl::queue &sycl_q, host_dataset* dataset,
                          gpu_timer& timer, sycl_mode mode) {
    stime_utils chrono;
    chrono.start();

    //  data_type* ddata_input, data_type* ddata_output,
    data_type* ddata_input = dataset->device_input;
    data_type* ddata_output = dataset->device_output;

    //data_type* ddata_output_verif = static_cast<data_type *> (cl::sycl::malloc_device(OUTPUT_DATA_SIZE, sycl_q));

    // SYCL kernel needs const variables
    const unsigned int local_VECTOR_SIZE_PER_ITERATION = VECTOR_SIZE_PER_ITERATION;
    const unsigned int local_PARALLEL_FOR_SIZE = PARALLEL_FOR_SIZE;
    const unsigned int local_REPEAT_COUNT_SUM = REPEAT_COUNT_SUM;

    // Sandor does not support anonymous kernels.

    class MyKernel_a;
    class MyKernel_b;

    if (SIMD_FOR_LOOP == 0) {
        //logs(" -NOT simd- ");
        // Starts a kernel - traditional for loop
        auto e = sycl_q.parallel_for<MyKernel_a>(cl::sycl::range<1>(PARALLEL_FOR_SIZE), [=](cl::sycl::id<1> chunk_index) {
            int cindex = chunk_index[0];
            int start_index = cindex * local_VECTOR_SIZE_PER_ITERATION;
            int stop_index = start_index + local_VECTOR_SIZE_PER_ITERATION;
            data_type sum = 0;

            /*if (REPEAT_COUNT_SUM == 1) { // Repeat 
                for (int i = start_index; i < stop_index; ++i) {
                    sum += ddata_input[i];
                }
            } else {*/
            // Repeat data access REPEAT_COUNT_SUM times
            for (int rp = 0; rp < local_REPEAT_COUNT_SUM; ++rp) {
                for (int i = start_index; i < stop_index; ++i) {
                    sum += ddata_input[i];
                }
            }

            ddata_output[cindex] = sum;
            //ddata_output_verif[cindex] = sum;
        });
        e.wait();
    } else {
        //logs(" -IS simd- ");
        // Starts a kernel - SIMD optimized for loop
        auto e = sycl_q.parallel_for<MyKernel_b>(cl::sycl::range<1>(PARALLEL_FOR_SIZE), [=](cl::sycl::id<1> chunk_index) {
            int cindex = chunk_index[0];
            data_type sum = 0;

            // Repeat data access REPEAT_COUNT_SUM times
            for (int rp = 0; rp < local_REPEAT_COUNT_SUM; ++rp) {
                for (int it = 0; it < local_VECTOR_SIZE_PER_ITERATION; ++it) {
                    int iindex = cindex + it * local_PARALLEL_FOR_SIZE;
                    sum += ddata_input[iindex];
                }
            }

            ddata_output[cindex] = sum;
            //ddata_output_verif[cindex] = sum;
        });
        e.wait();
    }

    sycl_q.wait_and_throw();
    timer.t_parallel_for = chrono.reset();

    //std::cout << ver_prefix + " - COMPUTE MODE = " + mode_to_string(mode) << std::endl;
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

    if ( total_sum == (dataset->final_result_verif * REPEAT_COUNT_SUM) ) {
        //log("VALID - Right data size ! (" + std::to_string(total_sum) + ")", 1);
    } else {
        log("ERROR on compute - expected size " + std::to_string(dataset->final_result_verif * REPEAT_COUNT_SUM)
            + " but found " + std::to_string(total_sum) + ". (REPEAT_COUNT_SUM(" + std::to_string(REPEAT_COUNT_SUM) + ")", 1);
    }
}





void generic_USM_allocation(cl::sycl::queue &sycl_q, host_dataset *dataset, gpu_timer& timer, sycl_mode mode) {
    stime_utils chrono;
    chrono.start();
    
    //std::cout << ver_prefix + " - ALLOC MODE = " + mode_to_string(mode) << std::endl;
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
    default : break; // TODO : add buffers/accessors
    }
    
    sycl_q.wait_and_throw();
    timer.t_allocation = chrono.reset();

    if (USE_HOST_SYCL_BUFFER_DMA) {
        //logs("SYCL hbuffer");
        // Copy from glibc buffer to SYCL malloc_host
        // and then from this malloc_host buffer to shared/device/host buffer

        // Only supports MEMCOPY_IS_SYCL, so I'll assume (MEMCOPY_IS_SYCL == true)

        // USM
        if ( (mode == sycl_mode::device_USM)
        ||   (mode == sycl_mode::host_USM)
        ||   (mode == sycl_mode::shared_USM) ) {
            // copy from glibc buffer to SYCL buffer malloc_host
            data_type* sycl_host_ds = static_cast<data_type *> (cl::sycl::malloc_host(INPUT_DATA_SIZE, sycl_q));
            timer.t_sycl_host_alloc = chrono.reset();

            sycl_q.memcpy(sycl_host_ds, dataset->data_input, INPUT_DATA_SIZE).wait();
            sycl_q.wait_and_throw();
            timer.t_sycl_host_copy = chrono.reset();

            // Copy from host SYCL buffer malloc_host to shared/device/host buffer
            sycl_q.memcpy(dataset->device_input, sycl_host_ds, INPUT_DATA_SIZE).wait();
            sycl_q.wait_and_throw();
            timer.t_copy_to_device = chrono.reset();

            cl::sycl::free(sycl_host_ds, sycl_q);
            sycl_q.wait_and_throw();
            timer.t_sycl_host_free = chrono.reset();
        }

    } else { // directly copy from glibc buffer to SYCL
        //logs("classic buffer");
        if ( (mode == sycl_mode::device_USM)
        ||   (mode == sycl_mode::host_USM)
        ||   (mode == sycl_mode::shared_USM) ) {

            // The only way to copy data to the device is to use sycl_q.memcpy
            // For shared memory (host and shared), glibc memcpy can be used.
            if ( (MEMCOPY_IS_SYCL == 1) || (mode == sycl_mode::device_USM) ) {
                log("MEM - SYCL MEMCOPY ----", 2);
                sycl_q.memcpy(dataset->device_input, dataset->data_input, INPUT_DATA_SIZE).wait();
                sycl_q.wait_and_throw();
            } else {
                log("MEM - GLIBC MEMCOPY ----", 2);
                // Probably works with host and shared, most likely does not work with device
                memcpy(dataset->device_input, dataset->data_input, INPUT_DATA_SIZE);
            }
            timer.t_copy_to_device = chrono.reset();
            //timer.t_sycl_host_alloc = 0;
            //timer.t_sycl_host_copy = 0;
            //timer.t_sycl_host_free = 0;
        }
    }
    // TODO : also copy data from dataset to shared memory.
}



void generic_USM_free(cl::sycl::queue &sycl_q, host_dataset* dataset, gpu_timer& timer, sycl_mode mode) {
    stime_utils chrono;
    chrono.start();
    //std::cout << ver_prefix + " - FREE MODE = " + mode_to_string(mode) << std::endl;
    cl::sycl::free(dataset->device_input, sycl_q);
    cl::sycl::free(dataset->device_output, sycl_q);
    sycl_q.wait_and_throw();
    timer.t_free_gpu = chrono.reset();
    dataset->device_input = nullptr;
    dataset->device_output = nullptr;
}



void generic_accessors_allocation(cl::sycl::queue &sycl_q, host_dataset *dataset, gpu_timer& timer, sycl_mode mode) {
    stime_utils chrono;
    chrono.start();
    timer.t_allocation = chrono.reset();

    // TODO : réussir à créer le buffer puis à le remplir ensuite et ne pas
    //        faire les deux en même temps comme c'est le cas actuellement.
    
    //std::cout << ver_prefix + " - ALLOC MODE = " + mode_to_string(mode) << std::endl;
    switch (mode) {
    case sycl_mode::accessors :
        dataset->buffer_input  = new cl::sycl::buffer<data_type, 1>(dataset->data_input, cl::sycl::range<1>(INPUT_DATA_LENGTH));
        dataset->buffer_output = new cl::sycl::buffer<data_type, 1>(dataset->data_output, cl::sycl::range<1>(OUTPUT_DATA_LENGTH));
        //dataset->device_input = static_cast<data_type *> (cl::sycl::malloc_device(INPUT_DATA_SIZE, sycl_q));
        //dataset->device_output = static_cast<data_type *> (cl::sycl::malloc_device(OUTPUT_DATA_SIZE, sycl_q));
        break;
    default : break; // TODO : add buffers/accessors
    }
    
    sycl_q.wait_and_throw();
    timer.t_copy_to_device = chrono.reset();
}



void generic_accessors_compute(cl::sycl::queue &sycl_q, host_dataset* dataset,
                               gpu_timer& timer, sycl_mode mode) {
    stime_utils chrono;
    chrono.start();

    cl::sycl::buffer<data_type, 1> *buffer_input  = dataset->buffer_input;
    cl::sycl::buffer<data_type, 1> *buffer_output = dataset->buffer_output;

    //  data_type* ddata_input, data_type* ddata_output,
    data_type* ddata_input = dataset->device_input;
    data_type* ddata_output = dataset->device_output;

    //data_type* ddata_output_verif = static_cast<data_type *> (cl::sycl::malloc_device(OUTPUT_DATA_SIZE, sycl_q));

    // SYCL kernel needs const variables
    const unsigned int local_VECTOR_SIZE_PER_ITERATION = VECTOR_SIZE_PER_ITERATION;
    const unsigned int local_PARALLEL_FOR_SIZE = PARALLEL_FOR_SIZE;
    const unsigned int local_REPEAT_COUNT_SUM = REPEAT_COUNT_SUM;

    // Sandor does not support anonymous kernels.

    
    //class MyKernel_ab;

    if (SIMD_FOR_LOOP == 0) {
        //logs(" -NOT simd- ");
        // Starts a kernel - traditional for loop

        sycl_q.submit([&](cl::sycl::handler &h) {

            // Initialisation via le constructeur des accesseurs
            cl::sycl::accessor a_input(*buffer_input, h, cl::sycl::read_only);
            cl::sycl::accessor a_output(*buffer_output, h, cl::sycl::write_only, cl::sycl::noinit); // noinit non supporté par hipsycl visiblement
            
            h.parallel_for<class MyKernel_aa>(cl::sycl::range<1>(PARALLEL_FOR_SIZE), [=](auto chunk_index) { //cl::sycl::id<1>
                int cindex = chunk_index[0];
                int start_index = cindex * local_VECTOR_SIZE_PER_ITERATION;
                int stop_index = start_index + local_VECTOR_SIZE_PER_ITERATION;
                data_type sum = 0;

                // Repeat data access REPEAT_COUNT_SUM times
                for (int rp = 0; rp < local_REPEAT_COUNT_SUM; ++rp) {
                    for (int i = start_index; i < stop_index; ++i) {
                        sum += a_input[i];
                    }
                }

                a_output[cindex] = sum;
            });

        }).wait_and_throw();
    } else {
        //logs(" -IS simd- ");
        // Starts a kernel - SIMD optimized for loop

        sycl_q.submit([&](cl::sycl::handler &h) {

            // Initialisation via le constructeur des accesseurs
            cl::sycl::accessor a_input(*buffer_input, h, cl::sycl::read_only);
            cl::sycl::accessor a_output(*buffer_output, h, cl::sycl::write_only, cl::sycl::noinit); // noinit non supporté par hipsycl visiblement

            // cl::sycl::id<1>
            h.parallel_for<class MyKernel_ab>(cl::sycl::range<1>(PARALLEL_FOR_SIZE), [=](auto chunk_index) {

                int cindex = chunk_index[0];
                data_type sum = 0;

                // Repeat data access REPEAT_COUNT_SUM times
                for (int rp = 0; rp < local_REPEAT_COUNT_SUM; ++rp) {
                    for (int it = 0; it < local_VECTOR_SIZE_PER_ITERATION; ++it) {
                        int iindex = cindex + it * local_PARALLEL_FOR_SIZE;
                        sum += a_input[iindex];
                    }
                }

                a_output[cindex] = sum;
            });

        }).wait_and_throw();
    }

    sycl_q.wait_and_throw();
    timer.t_parallel_for = chrono.reset();

    (*buffer_output).get_access<cl::sycl::access::mode::read>();

    data_type sum_simd_check_cpu = 0;

    // Value verification
    data_type total_sum = 0;
    for (int i = 0; i < OUTPUT_DATA_LENGTH; ++i) {
        total_sum += dataset->data_output[i]; //ddata_output_verif[i];//dataset->data_output[i];
    }

    timer.t_read_from_device = chrono.reset();

    if ( total_sum == (dataset->final_result_verif * REPEAT_COUNT_SUM) ) {
        //log("VALID - Right data size ! (" + std::to_string(total_sum) + ")", 1);
    } else {
        log("ERROR on compute - expected size " + std::to_string(dataset->final_result_verif * REPEAT_COUNT_SUM)
            + " but found " + std::to_string(total_sum) + ". (REPEAT_COUNT_SUM(" + std::to_string(REPEAT_COUNT_SUM) + ")", 1);
    }
}

void generic_accessors_free(cl::sycl::queue &sycl_q, host_dataset* dataset, gpu_timer& timer, sycl_mode mode) {
    stime_utils chrono;
    chrono.start();
    //std::cout << ver_prefix + " - FREE MODE = " + mode_to_string(mode) << std::endl;
    delete dataset->buffer_input;
    delete dataset->buffer_output;
    sycl_q.wait_and_throw();
    timer.t_free_gpu = chrono.reset();
    dataset->buffer_input = nullptr;
    dataset->buffer_output = nullptr;
}





host_dataset* generate_datasets() {
    return
    generate_datasets(DATASET_NUMBER, INPUT_DATA_LENGTH, OUTPUT_DATA_LENGTH,
                     CHECK_SIMD_CPU, PARALLEL_FOR_SIZE, VECTOR_SIZE_PER_ITERATION);
}

void delete_datasets(host_dataset* hdata) {
    delete_datasets(hdata, DATASET_NUMBER);
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

    case sycl_mode::accessors :
        sycl_allocation = generic_accessors_allocation;
        sycl_compute = generic_accessors_compute;
        sycl_free = generic_accessors_free;
        break;

    default : break; // TODO : add buffers/accessors
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

    gtimer.t_data_generation_and_ram_allocation = global_t_data_generation_and_ram_allocation; //chrono.reset(); //get_ms() - t_start;

    log("Input data size  : " + std::to_string(INPUT_DATA_SIZE)
        + " (" + std::to_string(INPUT_DATA_SIZE / (1024*1024)) + " MiB)", 1);
    log("Output data size : " + std::to_string(OUTPUT_DATA_SIZE)
        + " (" + std::to_string(OUTPUT_DATA_SIZE / (1024*1024)) + " MiB)", 1);

    // The default device selector will select the most performant device.
    //cl::sycl::default_selector d_selector;
    custom_device_selector d_selector;

    try {
        chrono.reset(); //t_start = get_ms();
        cl::sycl::queue sycl_q(d_selector, exception_handler);
        sycl_q.wait_and_throw();
        gtimer.t_queue_creation = chrono.reset();//get_ms() - t_start;

        // Print out the device information used for the kernel code.
        log("--   " + sycl_q.get_device().get_info<cl::sycl::info::device::name>() + "   --");
        
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
                    << USE_NAMED_KERNEL << " " // flag to indicate if the named kernel was used or traditional lambda kernel
                    // wether there is a copy from SYCL host to device, or normal (allocated by new) copy to device, to test DMA access
                    << USE_HOST_SYCL_BUFFER_DMA << " "
                    // How many times the sum should be repeated
                    << REPEAT_COUNT_SUM
                    << "\n";

        log("\n######## ALLOCATION, COPY AND FREE FOR EACH ITERATION ########", 2);

        // Allocation, copy and free each time
        for (int ids = 0; ids < DATASET_NUMBER; ++ids) {
            
            host_dataset* dataset = &hdata[ids];
            write_file << dataset->seed << "\n";

            log("------- DATASET SEED " + std::to_string(dataset->seed) + " -------\n", 2);

            // Allocation and free on device, for each iteration
            for (int rpt = 0; rpt < REPEAT_COUNT_REALLOC; ++rpt) {
                log("Iteration " + std::to_string(rpt+1) + " on " + std::to_string(REPEAT_COUNT_REALLOC), 2);

                sycl_allocation(sycl_q, dataset, gtimer, mode);
                sycl_compute(sycl_q, dataset, gtimer, mode);
                sycl_free(sycl_q, dataset, gtimer, mode);

                write_file << gtimer.t_allocation << " "
                            << gtimer.t_sycl_host_alloc << " " // v6
                            << gtimer.t_sycl_host_copy << " " // v6
                            << gtimer.t_copy_to_device << " "
                            << gtimer.t_sycl_host_free << " " // v6
                            // TODO : do the same with alloc/cpy only once
                            << gtimer.t_parallel_for << " " 
                            << gtimer.t_read_from_device << " "
                            << gtimer.t_free_gpu
                            << "\n";
                
                // A new line for each repeat count :
                // t_allocation t_copy_to_device t_parallel_for t_read_from_device t_free_gpu
                print_timer_iter_alloc(gtimer);

                ++current_iteration_count;
                print_total_progress();
            }
        }

        log("\n######## ALLOCATION, COPY AND FREE ONLY ONCE ########", 2);

        // Allocation, copy and free once
        for (int ids = 0; ids < DATASET_NUMBER; ++ids) {
            
            host_dataset* dataset = &hdata[ids];
            write_file << dataset->seed << "\n";

            log("------- DATASET SEED " + std::to_string(dataset->seed) + " -------\n", 2);
            sycl_allocation(sycl_q, dataset, gtimer, mode);

            // Allocation and free on device, for each iteration
            for (int rpt = 0; rpt < REPEAT_COUNT_ONLY_PARALLEL; ++rpt) {
                log("Iteration " + std::to_string(rpt+1) + " on " + std::to_string(REPEAT_COUNT_ONLY_PARALLEL), 2);
                
                sycl_compute(sycl_q, dataset, gtimer, mode);
                
                write_file << gtimer.t_parallel_for << " " 
                            << gtimer.t_read_from_device << " "
                            << "\n";

                // A new line for each repeat count :
                // t_allocation t_copy_to_device t_parallel_for t_read_from_device t_free_gpu
                print_timer_iter(gtimer);

                ++current_iteration_count;
                print_total_progress();
            }
            sycl_free(sycl_q, dataset, gtimer, mode);

            write_file << gtimer.t_allocation << " "
                        << gtimer.t_sycl_host_alloc << " " // v6
                        << gtimer.t_sycl_host_copy << " " // v6
                        << gtimer.t_copy_to_device << " "
                        << gtimer.t_sycl_host_free << " " // v6
                        << gtimer.t_free_gpu
                        << "\n";

            print_timer_alloc(gtimer);
        }

        if (MAX_SHOWN_LEVEL >= 2) {
            std::cout 
                << "t_data_ram_init            = " << gtimer.t_data_generation_and_ram_allocation << std::endl
                << "t_queue_creation           = " << gtimer.t_queue_creation << std::endl
                ;
            log("");
        }



    } catch (cl::sycl::exception const &e) {
        std::cout << "An exception has been caught while processing SyCL code.\n";
        std::terminate();
    }

    if ( ! KEEP_SAME_DATASETS ) {
        // Delete local datasets
        delete_datasets(hdata);
    }

    log("done.");
}

//int percent_div_factor = 1;

void bench_smid_modes(std::ofstream& myfile) {

    //unsigned int total_elements = 1024 * 1024 * 256; // 256 * bytes = 1 GiB.
    VECTOR_SIZE_PER_ITERATION = BASE_VECTOR_SIZE_PER_ITERATION;
    PARALLEL_FOR_SIZE = total_elements / VECTOR_SIZE_PER_ITERATION; // = 131072

    int imode;
    //MEMCOPY_IS_SYCL = 1;
    //SIMD_FOR_LOOP = 0;
    //USE_NAMED_KERNEL = 0;

    log("============    - L = VECTOR_SIZE_PER_ITERATION = " + std::to_string(VECTOR_SIZE_PER_ITERATION));
    log("============    - M = PARALLEL_FOR_SIZE = " + std::to_string(PARALLEL_FOR_SIZE));
    
    total_main_seq_runs = 2 * 3;
    
    //percent_div_factor = 2 * 3;

    for (int imcp = 0; imcp <= 1; ++imcp) {
        SIMD_FOR_LOOP = imcp;

        for (int imode = 0; imode <= 2; ++imode) {
            
            switch (imode) {
            case 0: CURRENT_MODE = sycl_mode::shared_USM; break;
            case 1: CURRENT_MODE = sycl_mode::device_USM; break;
            case 2: CURRENT_MODE = sycl_mode::host_USM; break;
            default : break;
            }
            
            log("Mode(" + mode_to_string(CURRENT_MODE) + ")  SIMD_FOR_LOOP(" + std::to_string(SIMD_FOR_LOOP) + ")");
            main_sequence(myfile, CURRENT_MODE);
            log("");
        }
    }
}

void bench_mem_alloc_modes(std::ofstream& myfile) {

    //unsigned int total_elements = 1024 * 1024 * 256; // 256 * bytes = 1 GiB.
    VECTOR_SIZE_PER_ITERATION = BASE_VECTOR_SIZE_PER_ITERATION;
    PARALLEL_FOR_SIZE = total_elements / VECTOR_SIZE_PER_ITERATION; // = 131072

    // how many times main_sequence will be run
    total_main_seq_runs = 2 * 3;

    int imode;
    //MEMCOPY_IS_SYCL = 1;
    //SIMD_FOR_LOOP = 0;
    //USE_NAMED_KERNEL = 0;

    log("============    - L = VECTOR_SIZE_PER_ITERATION = " + std::to_string(VECTOR_SIZE_PER_ITERATION));
    log("============    - M = PARALLEL_FOR_SIZE = " + std::to_string(PARALLEL_FOR_SIZE));
    
    //percent_div_factor = 2 * 2;

    for (int imcp = 0; imcp <= 1; ++imcp) {
        MEMCOPY_IS_SYCL = imcp;

        for (int imode = 0; imode <= 3; ++imode) {
            
            switch (imode) {
            case 0: CURRENT_MODE = sycl_mode::shared_USM; break;
            case 1: CURRENT_MODE = sycl_mode::device_USM; break;
            case 2: CURRENT_MODE = sycl_mode::host_USM; break;
            case 3: CURRENT_MODE = sycl_mode::accessors; break;
            default : break;
            }

            // No "non-sycl copy" for buffers-accessors. (not yet at least)
            if ( (CURRENT_MODE == sycl_mode::accessors) && (MEMCOPY_IS_SYCL == 0) ) continue;

            log("Mode(" + mode_to_string(CURRENT_MODE) + ")  MEMCOPY_IS_SYCL(" + std::to_string(MEMCOPY_IS_SYCL) + ")");
            //log("============    - L = VECTOR_SIZE_PER_ITERATION = " + std::to_string(VECTOR_SIZE_PER_ITERATION));
            
            main_sequence(myfile, CURRENT_MODE);
            log("");
        }
    }
}

void bench_host_copy_buffer(std::ofstream& myfile) {

    VECTOR_SIZE_PER_ITERATION = BASE_VECTOR_SIZE_PER_ITERATION;
    PARALLEL_FOR_SIZE = total_elements / VECTOR_SIZE_PER_ITERATION;

    // how many times main_sequence will be run
    total_main_seq_runs = 2 * 3;

    int imode;
    //MEMCOPY_IS_SYCL = 1;
    //SIMD_FOR_LOOP = 0;
    //USE_NAMED_KERNEL = 0;

    log("============    - L = VECTOR_SIZE_PER_ITERATION = " + std::to_string(VECTOR_SIZE_PER_ITERATION));
    log("============    - M = PARALLEL_FOR_SIZE = " + std::to_string(PARALLEL_FOR_SIZE));
    
    //percent_div_factor = 2 * 2;

    for (int imcp = 0; imcp <= 1; ++imcp) {
        USE_HOST_SYCL_BUFFER_DMA = imcp;

        for (int imode = 0; imode <= 2; ++imode) {
            
            switch (imode) {
            case 0: CURRENT_MODE = sycl_mode::shared_USM; break;
            case 1: CURRENT_MODE = sycl_mode::device_USM; break;
            case 2: CURRENT_MODE = sycl_mode::host_USM; break;
            default : break;
            }
            log("Mode(" + mode_to_string(CURRENT_MODE) + ")  USE_HOST_SYCL_BUFFER_DMA(" + std::to_string(USE_HOST_SYCL_BUFFER_DMA) + ")");
            //log("============    - L = VECTOR_SIZE_PER_ITERATION = " + std::to_string(VECTOR_SIZE_PER_ITERATION));
            
            main_sequence(myfile, CURRENT_MODE);
            log("");
        }
    }
}

// Optimisé pour Sandor et MSI seulement
/*
1. Faire varier REPEAT_COUNT_SUM à L fixé (grand).
   Le temps pris par le parallel_for devrait augmenter de manière linéaire,
   avec un coefficient plus grand en host et plus faible en device.
2. Faire varier L à REPEAT_COUNT_SUM (grand) fixé.
   Pour évaluer la mise en cache s'il y a mise en cache, et le comportement
   des caches en explicite, shared et host. A L petit on devrait avoir les données
   dans les caches, donc être avec une pente très faible, plus la pente devrait
   augmenter peu à peu (ou par palier), en fonction de la taille des caches des
   unités faisant exécuter les workitems.
*/
void bench_data_access_time_with_repeat(std::ofstream& myfile) {

    VECTOR_SIZE_PER_ITERATION = 16384; // 128 un gros nombre pour dissimuler la mise en cache si elle advient
    PARALLEL_FOR_SIZE = total_elements / VECTOR_SIZE_PER_ITERATION;


    int imode;
    //MEMCOPY_IS_SYCL = 1;
    //SIMD_FOR_LOOP = 0;
    //USE_NAMED_KERNEL = 0;

    log("============    - L = VECTOR_SIZE_PER_ITERATION = " + std::to_string(VECTOR_SIZE_PER_ITERATION));
    log("============    - M = PARALLEL_FOR_SIZE = " + std::to_string(PARALLEL_FOR_SIZE));
    
    //percent_div_factor = 2 * 2;

    // Lorsque je vais modifier L :
    // Se mettre en échelle log en x (comme dans le plot classique L M)
    // et en y. Devrait augmenter en affine plus en exponentielle

    // Modification de REPEAT_COUNT_SUM :
    // Probable doublement des temps de parallel_for pour avec REPEAT_COUNT_SUM = 2.
    // Mais comme les accès device sont beaucoup plus rapides on a amorti le temps de copie initiale
    // alors qu'en host on se tape à nouveau une copie des données, ce qui est super
    // couteux.
    // Bilan supposé :
    // Host c'est super pour des données côté hôte qui bougent beaucoup et peu
    // d'accès côté device.
    // Device c'est super pour des données qui sont accédées très souvent.

    uint max_repeat_sum = 30; // changer le nom du fichier d'ouput

    // how many times main_sequence will be run
    total_main_seq_runs = max_repeat_sum * 3;

    for (REPEAT_COUNT_SUM = 1; REPEAT_COUNT_SUM <= max_repeat_sum; ++REPEAT_COUNT_SUM) {
        //REPEAT_COUNT_SUM = imcp;

        for (int imode = 0; imode <= 2; ++imode) {
            
            switch (imode) {
            case 0: CURRENT_MODE = sycl_mode::shared_USM; break;
            case 1: CURRENT_MODE = sycl_mode::device_USM; break;
            case 2: CURRENT_MODE = sycl_mode::host_USM; break;
            default : break;
            }
            log("Mode(" + mode_to_string(CURRENT_MODE) + ")  REPEAT_COUNT_SUM(" + std::to_string(REPEAT_COUNT_SUM) + " on "
                + std::to_string(max_repeat_sum) + ")");
            //log("============    - L = VECTOR_SIZE_PER_ITERATION = " + std::to_string(VECTOR_SIZE_PER_ITERATION));
            
            main_sequence(myfile, CURRENT_MODE);
            log("");
        }
    }
}

// TODO : ne refaire les benchmarks que si les fichiers n'existent pas,
// ne pas remplacer les fichiers existants.

void bench_cache_size(std::ofstream& myfile) {

    REPEAT_COUNT_SUM = 10;

    long long start_L_size = 1;

    long long stop_M_size = 256; // inclusive
    long long stop_L_size = total_elements / stop_M_size;

    // how many times main_sequence will be run
    total_main_seq_runs = 0;
    for (VECTOR_SIZE_PER_ITERATION = start_L_size; VECTOR_SIZE_PER_ITERATION <= stop_L_size; VECTOR_SIZE_PER_ITERATION *= 2) {
        for (int imode = 0; imode <= 2; ++imode) {
            total_main_seq_runs += 1;
        }
    }
    //current_main_seq_runs = 0;

    for (VECTOR_SIZE_PER_ITERATION = start_L_size; VECTOR_SIZE_PER_ITERATION <= stop_L_size; VECTOR_SIZE_PER_ITERATION *= 2) {
        PARALLEL_FOR_SIZE = total_elements / VECTOR_SIZE_PER_ITERATION;

        for (int imode = 0; imode <= 2; ++imode) { // only device to make it go faster
            
            switch (imode) {
            case 0: CURRENT_MODE = sycl_mode::shared_USM; break;
            case 1: CURRENT_MODE = sycl_mode::device_USM; break;
            case 2: CURRENT_MODE = sycl_mode::host_USM; break;
            default : break;
            }
            
            log("========================================="); // + ver_prefix); ver_prefix is now obsolete
            log(" - MEMORY = " + mode_to_string(CURRENT_MODE));
            log(" - L = " + std::to_string(VECTOR_SIZE_PER_ITERATION));
            log(" - M = " + std::to_string(PARALLEL_FOR_SIZE));
            main_sequence(myfile, CURRENT_MODE);
            log("");
            //++current_main_seq_runs;
        }
    }




}


void bench_choose_L_M(std::ofstream& myfile) {

    //long long total_elements = 1024L * 1024L * 256L * 1L; // 256 * bytes = 1 GiB.

    //int imode;
    //MEMCOPY_IS_SYCL = 1;
    //SIMD_FOR_LOOP = 0;
    //USE_NAMED_KERNEL = 0;
    
    // start with 2 as I want to use 1GiB but MSI has only 2 (1.5 GiB ok, 2 GiB not ok)
    // for input (1 GiB and output 512MiB)
    long long start_L_size = 1;

    long long stop_M_size = 256; // inclusive
    long long stop_L_size = total_elements / stop_M_size;

    // how many times main_sequence will be run
    total_main_seq_runs = 0;
    for (VECTOR_SIZE_PER_ITERATION = start_L_size; VECTOR_SIZE_PER_ITERATION <= stop_L_size; VECTOR_SIZE_PER_ITERATION *= 2) {
        for (int imode = 1; imode <= 1; ++imode) {
            total_main_seq_runs += 1;
        }
    }
    //current_main_seq_runs = 0;

    for (VECTOR_SIZE_PER_ITERATION = start_L_size; VECTOR_SIZE_PER_ITERATION <= stop_L_size; VECTOR_SIZE_PER_ITERATION *= 2) {
        PARALLEL_FOR_SIZE = total_elements / VECTOR_SIZE_PER_ITERATION;

        for (int imode = 1; imode <= 1; ++imode) {
            
            switch (imode) {
            //case 0: CURRENT_MODE = sycl_mode::shared_USM; break;
            case 1: CURRENT_MODE = sycl_mode::device_USM; break;
            //case 2: CURRENT_MODE = sycl_mode::host_USM; break;
            default : break;
            }
            
            log("========================================="); // + ver_prefix); ver_prefix is now obsolete
            log(" - MEMORY = " + mode_to_string(CURRENT_MODE));
            log(" - L = " + std::to_string(VECTOR_SIZE_PER_ITERATION));
            log(" - M = " + std::to_string(PARALLEL_FOR_SIZE));
            main_sequence(myfile, CURRENT_MODE);
            log("");
            //++current_main_seq_runs;
        }
    }
}





class bench_sycl_glibc_mem_speed_run {
public :
    uint64_t min_time, max_time;
    uint64_t count; // number of runs
    uint64_t total_time;
    void init() {
        min_time = 0;
        max_time = 0;
        count = 0;
        total_time = 0;
    }
    void add(uint64_t time) {
        if (count == 0) { // init
            min_time = time;
            max_time = time;
        } else {
            if (min_time > time) min_time = time;
            if (max_time < time) max_time = time;
        }
        ++count;
        total_time += time;
    }
    void print(std::string name) {
        uint64_t moy = total_time / count;
        uint64_t div = 1000;
        uint64_t tolerate_fact = 140;
        if (min_time * tolerate_fact / 100 >= max_time) {
            log("   " + name + "   " + std::to_string(moy / div));
        } else {
            log("   " + name + "   " + std::to_string(min_time / div)
            + " -> " + std::to_string(max_time / div)
            + " : " + std::to_string(moy / div));
        }
    }

};

class bench_sycl_glibc_mem_speed_main {

private :
    const uint64_t ecount = 1024L * 1024L * 128L * 4L;
    const std::string size_str = "512MiB";
    uint run_count = 3;

    const uint64_t size = ecount * sizeof(DATA_TYPE); // 128 milions elements * 4 bytes => 512 MiB
    custom_device_selector d_selector;
    uint mem_type_src, mem_type_dest;
    bench_sycl_glibc_mem_speed_run t_alloc_src, t_alloc_dest, t_fill, t_copy, t_free_src, t_free_dest;


public :
    void init() {
        

    }

    uint sum(DATA_TYPE* mem) {
        DATA_TYPE s = 0;
        for (uint i = 0; i < ecount; ++i) {
            s += mem[i];
        }
        return s;
    }

    DATA_TYPE* alloc(uint mem_type, cl::sycl::queue sycl_q) {
        switch (mem_type) {
        case 1: return new DATA_TYPE[ecount]; // glibc
        case 2: return static_cast<data_type *> (cl::sycl::malloc_host(INPUT_DATA_SIZE, sycl_q)); // sycl
        default : return nullptr;
        }
    }

    void fill(DATA_TYPE* mem) {
        for (uint i = 0; i < ecount; ++i) {
            mem[i] = i;
        }
    }

    void freemem(uint mem_type, DATA_TYPE* mem, cl::sycl::queue sycl_q) {
        switch (mem_type) {
        case 1: delete[] mem; break; // glibc
        case 2: cl::sycl::free(mem, sycl_q); break; // sycl
        default : break;
        }
    }

    void init_timers() {
        t_alloc_src.init();
        t_fill.init();
        t_alloc_dest.init();
        t_copy.init();
        t_free_src.init();
        t_free_dest.init();
    }

    void run(uint mem_type_src, uint mem_type_dest, uint cpy_type, cl::sycl::queue sycl_q) {

        /*log("run src" + std::to_string(mem_type_src) + " - dest" + std::to_string(mem_type_src)
            + " - ecount" + std::to_string(ecount)
            + " - size" + std::to_string(size)
            );*/

        stime_utils chrono;
        chrono.reset(); //t_start = get_ms();

        DATA_TYPE* mem_src = alloc(mem_type_src, sycl_q);
        t_alloc_src.add(chrono.reset());

        fill(mem_src);
        t_fill.add(chrono.reset());

        DATA_TYPE* mem_dest = alloc(mem_type_dest, sycl_q);
        t_alloc_dest.add(chrono.reset());

        // On a vu que temps de copie SYCL = temps de copie glibc pour host
        
        if (cpy_type == 1) memcpy(mem_dest, mem_src, size);
        if (cpy_type == 2) sycl_q.memcpy(mem_dest, mem_src, size);
        t_copy.add(chrono.reset());

        logs("    s" + std::to_string(sum(mem_dest)));

        freemem(mem_type_src, mem_src, sycl_q);
        t_free_src.add(chrono.reset());

        freemem(mem_type_dest, mem_dest, sycl_q);
        t_free_dest.add(chrono.reset());


    }

    void multiple_runs(uint mem_type_src, uint mem_type_dest, uint cpy_type, cl::sycl::queue sycl_q) {

        init_timers();

        for (uint ir = 0; ir < run_count; ++ir) {
                run(1, 1, 1, sycl_q);
            }
            // print result timer(min, max, moy)
            t_alloc_src .print(" alloc src");
            t_fill      .print("      fill");
            t_alloc_dest.print("alloc dest");
            t_copy      .print("      copy");
            t_free_src  .print("  free src");
            t_free_dest .print(" free dest");
            log("\n");
    }

    void main() {
        // 1) Allocation de la mémoire (src) sycl / glibc
        // 2) Remplissage de la mémoire avec des nombres aléatoires (ou toujours le même nombre)
        // 3) Allocation de la mémoire (dest) sycl/glibc
        // 4) Copie de src vers dest
        // 5) Libération de la mémoire src
        // 6) Libération de la mémoire dest

        
        try {
            
            cl::sycl::queue sycl_q(d_selector, exception_handler);
            sycl_q.wait_and_throw();

            logs("glibc -> glibc (copie glibc)");
            multiple_runs(1, 1, 1, sycl_q);

            logs("sycl -> sycl : (copie glibc)");
            multiple_runs(2, 2, 1, sycl_q);

            logs("sycl -> sycl : (copie sycl)");
            multiple_runs(2, 2, 2, sycl_q);

            logs("glibc -> sycl : (copie glibc)");
            multiple_runs(1, 2, 1, sycl_q);

            logs("glibc -> sycl : (copie sycl)");
            multiple_runs(1, 2, 2, sycl_q);

            logs("sycl -> glibc : (copie glibc)");
            multiple_runs(2, 1, 1, sycl_q);

            logs("sycl -> glibc : (copie sycl)");
            multiple_runs(2, 1, 2, sycl_q);

        } catch (cl::sycl::exception const &e) {
            std::cout << "An exception has been caught while processing SyCL code.\n";
            std::terminate();
        }
    }

};



/*
Seulement un print sur la console, pour afficher les temps de transfert.
*/
void bench_sycl_glibc_mem_speed() { // std::ofstream& myfile

    bench_sycl_glibc_mem_speed_main c;
    c.main();
}





int main_of_program(std::function<void(std::ofstream &)> bench_function)
{
    std::ofstream myfile;
    std::string wdir_tmp = std::filesystem::current_path();
    std::string wdir = wdir_tmp + "/output_bench/";
    std::string output_file_name = wdir + std::string(OUTPUT_FILE_NAME);

    if ( file_exists_test0(output_file_name) ) {
        log("\n\n\n\n\nFILE ALREADY EXISTS, SKIPPING TEST");
        log("NAME = " + OUTPUT_FILE_NAME + "\n");
        log("FULL PATH = " + output_file_name + "\n\n\n\n\n");
        return 4;
    }

    myfile.open(output_file_name);
    log("");

    log("current_path     = " + wdir);
    log("output_file_name = " + output_file_name);

    if (myfile.is_open()) {
        log("OK, fichier bien ouvert.");
    } else {
        log("ERREUR : échec de l'ouverture du fichier en écriture.");
        return 10;
    }
    log("");

    myfile << DATA_VERSION << "\n";

    std::cout << "============================" << std::endl;
    std::cout << "   SYCL memory benchmark.   " << std::endl;
    std::cout << "============================" << std::endl;
    
    std::cout << OUTPUT_FILE_NAME << std::endl;
    log("");
    log("-------------- " + ver_indicator + " --------------");
    log("");

    list_devices(exception_handler);

    init_progress();

    bench_function(myfile);

    //bench_mem_alloc_modes(myfile);
    //bench_smid_modes(myfile);
    //bench_host_copy_buffer(myfile);
    //bench_choose_L_M(myfile);

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

/// Reset variables that may have been changed during benchmarking
void reset_bench_variables() {
    MEMCOPY_IS_SYCL = 1;
    SIMD_FOR_LOOP = 1;
    USE_HOST_SYCL_BUFFER_DMA = 0;
}

void run_single_test_generic(std::string size_prefix, std::string computer_name,
                             uint test_id, uint run_count) {
    std::string file_name_prefix = "_" + computer_name + "_" + size_prefix + "_O2";
    std::string file_name_const_part = file_name_prefix + "_RUN" + std::to_string(run_count) + ".t";

    switch (test_id) {
    
    // LMoptim
    case 1:
        OUTPUT_FILE_NAME = BENCHMARK_VERSION + "_LMoptim" + file_name_const_part;
        reset_bench_variables();
        SIMD_FOR_LOOP = 1;
        main_of_program(bench_choose_L_M);
        break;
    
    // DMA
    case 2: 
        OUTPUT_FILE_NAME = BENCHMARK_VERSION + "_dma" + file_name_const_part;
        reset_bench_variables();
        main_of_program(bench_host_copy_buffer);
        break;

    // Mem optimisation SIMD
    case 3: 
        OUTPUT_FILE_NAME = BENCHMARK_VERSION + "_simd" + file_name_const_part;
        reset_bench_variables();
        main_of_program(bench_smid_modes);
        break;

    // SYCL alloc vs glibc alloc
    case 4: 
        OUTPUT_FILE_NAME = BENCHMARK_VERSION + "_alloc" + file_name_const_part;
        reset_bench_variables();
        main_of_program(bench_mem_alloc_modes);
        break;
    
    // LM
    case 5:
        OUTPUT_FILE_NAME = BENCHMARK_VERSION + "_LMclassic" + file_name_const_part;
        reset_bench_variables();
        SIMD_FOR_LOOP = 0;
        main_of_program(bench_choose_L_M);
        break;

    // Data access time, making REPEAT_COUNT_SUM vary.
    case 6:
        OUTPUT_FILE_NAME = BENCHMARK_VERSION + "_sumReadSpeed" + file_name_const_part;
        reset_bench_variables();
        main_of_program(bench_data_access_time_with_repeat);
        break;

    // Cache size evaluation, fixed REPEAT_COUNT_SUM (should be big), making L (and M) vary.
    case 7:
        OUTPUT_FILE_NAME = BENCHMARK_VERSION + "_cacheSize" + file_name_const_part;
        reset_bench_variables();
        main_of_program(bench_cache_size);
        break;
    
    default : break;
    }
}


void profile_mem_alloc_modes(std::ofstream& myfile) {

    REPEAT_COUNT_REALLOC = 1;

    for (int imode = 0; imode <= 2; ++imode)
    for (int imcp = 0; imcp <= 1; ++imcp)  {
        MEMCOPY_IS_SYCL = imcp;

        
        switch (imode) {
        case 0: CURRENT_MODE = sycl_mode::shared_USM; break;
        case 1: CURRENT_MODE = sycl_mode::device_USM; break;
        case 2: CURRENT_MODE = sycl_mode::host_USM; break;
        default : break;
        }
        log("PROFILING - PROFILING - Mode(" + mode_to_string(CURRENT_MODE) + ")  MEMCOPY_IS_SYCL(" + std::to_string(MEMCOPY_IS_SYCL) + ")");
        
        main_sequence(myfile, CURRENT_MODE);

        log("");
    }
}
void profile_mem_alloc_modes_ext(std::ofstream& myfile) { //, sycl_mode mode, bool use_sycl_memcpy) {

    REPEAT_COUNT_REALLOC = 1;
    //MEMCOPY_IS_SYCL = use_sycl_memcpy ? 1 : 0;

    //CURRENT_MODE = mode;

    log("PROFILING - PROFILING - Mode(" + mode_to_string(CURRENT_MODE) + ")  MEMCOPY_IS_SYCL(" + std::to_string(MEMCOPY_IS_SYCL) + ")");
    
    uint repeat_count = 3;
    
    for (uint ir = 0; ir < repeat_count; ++ir) {
        main_sequence(myfile, CURRENT_MODE);
        log("\nNOW SLEEP...");
        unsigned int microseconds = 1000 * 1000; // 1 seconde
        usleep(microseconds);
        log("WOKEN-UP.");
    }
    
    log("");
}

/*void profiling_run_test(std::string size_prefix, std::string computer_name,
                             uint test_id, uint run_count) {
    std::string file_name_prefix = "_" + computer_name + "_" + size_prefix + "_O2";
    std::string file_name_const_part = file_name_prefix + "_RUN" + std::to_string(run_count) + ".t";

    // profiler shared : copie glibc vs sycl
    if (test_id == 1) {

        VECTOR_SIZE_PER_ITERATION = 128;
        PARALLEL_FOR_SIZE = total_elements / VECTOR_SIZE_PER_ITERATION; // = 131072

        OUTPUT_FILE_NAME = BENCHMARK_VERSION + "_alloc_PROFILING" + file_name_const_part;
        reset_bench_variables();
        main_of_program(profile_mem_alloc_modes);
    }
}*/

void profiling_run_test(std::string size_prefix, std::string computer_name,
                             uint test_id, uint run_count) {
    std::string file_name_prefix = "_" + computer_name + "_" + size_prefix + "_O2";
    std::string file_name_const_part = file_name_prefix + "_RUN" + std::to_string(run_count) + ".t";

    // profiler shared : copie glibc vs sycl

    VECTOR_SIZE_PER_ITERATION = BASE_VECTOR_SIZE_PER_ITERATION;
    PARALLEL_FOR_SIZE = total_elements / VECTOR_SIZE_PER_ITERATION; // = 131072
    
    // host sycl
    if (test_id == 1) {
        OUTPUT_FILE_NAME = BENCHMARK_VERSION + "_alloc_PROFILING-host-sycl.protemp";// + file_name_const_part;
        reset_bench_variables();
        MEMCOPY_IS_SYCL = 1;
        CURRENT_MODE = sycl_mode::host_USM;
        main_of_program(profile_mem_alloc_modes_ext);
    }
    // host glibc
    if (test_id == 2) {
        OUTPUT_FILE_NAME = BENCHMARK_VERSION + "_alloc_PROFILING-host-glibc.protemp";// + file_name_const_part;
        reset_bench_variables();
        MEMCOPY_IS_SYCL = 0;
        CURRENT_MODE = sycl_mode::host_USM;
        main_of_program(profile_mem_alloc_modes_ext);
    }
    // host sycl
    if (test_id == 3) {
        OUTPUT_FILE_NAME = BENCHMARK_VERSION + "_alloc_PROFILING-shared-sycl.protemp";// + file_name_const_part;
        reset_bench_variables();
        MEMCOPY_IS_SYCL = 1;
        CURRENT_MODE = sycl_mode::shared_USM;
        main_of_program(profile_mem_alloc_modes_ext);
    }
    // host glibc
    if (test_id == 4) {
        OUTPUT_FILE_NAME = BENCHMARK_VERSION + "_alloc_PROFILING-shared-glibc.protemp";// + file_name_const_part;
        reset_bench_variables();
        MEMCOPY_IS_SYCL = 0;
        CURRENT_MODE = sycl_mode::shared_USM;
        main_of_program(profile_mem_alloc_modes_ext);
    }

    // device
    if (test_id == 5) {
        OUTPUT_FILE_NAME = BENCHMARK_VERSION + "_alloc_PROFILING-device-sycl.protemp";// + file_name_const_part;
        reset_bench_variables();
        MEMCOPY_IS_SYCL = 1;
        CURRENT_MODE = sycl_mode::device_USM;
        main_of_program(profile_mem_alloc_modes_ext);
    }


}

void run_all_test_generic(std::string size_prefix, std::string computer_name, int runs_count = 4) {
    std::string file_name_prefix = "_" + computer_name + "_" + size_prefix + "_O2";
    // new naming convention : vXX_benchType_computer_size.t

    // Tests to compare against, to check graphs validity
    //int test_runs_count = runs_count;
    for (uint irun = 1; irun <= runs_count; ++irun) {
        for (uint itest = 1; itest <= 7; ++itest) {
            run_single_test_generic(size_prefix, computer_name, itest, irun);
        }
    }
    
    /*for (int i = 1; i <= test_runs_count; ++i) {

        std::string file_name_const_part = file_name_prefix + "_RUN" + std::to_string(i) + ".t";
        //OUTPUT_FILE_NAME = "v02_thinkpad_L_M_" + size_prefix + "_O2_RUN" + run_count_str + ".t";

        OUTPUT_FILE_NAME = "v02_LM" + file_name_const_part;
        reset_bench_variables();
        main_of_program(bench_choose_L_M);

        OUTPUT_FILE_NAME = "v02_dma" + file_name_const_part;
        reset_bench_variables();
        main_of_program(bench_host_copy_buffer);

        OUTPUT_FILE_NAME = "v02_simd" + file_name_const_part;
        reset_bench_variables();
        main_of_program(bench_smid_modes);

        OUTPUT_FILE_NAME = "v02_alloc" + file_name_const_part;
        reset_bench_variables();
        main_of_program(bench_mem_alloc_modes);
    }*/
}



// NextCloud sur les ordis + rsync sur prdi portable
/*
void run_all_test_on_thinkpad() {
    std::string size_prefix, computer_name, file_name_prefix;
    // size_prefix could be computed too
    // and computer_name could be found out on run

    log("====== RUN ALL TESTS ON THINKPAD ======");
    log("======                           ======");
    log("====== RUN ALL TESTS ON THINKPAD ======");
    log("======                           ======");

    FORCE_EXECUTION_ON_NAMED_DEVICE = true;
    MUST_RUN_ON_DEVICE_NAME = "Intel(R) UHD Graphics 620 [0x5917]";

    REPEAT_COUNT_REALLOC = 3;
    REPEAT_COUNT_ONLY_PARALLEL = 0;

    //total_elements = 1024L * 1024L * 256L; // 1 GiB
    total_elements = 1024L * 1024L * 128L; // 512 MiB
    run_all_test_generic("512MiB", "thinkpad");

    //bench_mem_alloc_modes(myfile);
    //bench_smid_modes(myfile);
    //bench_host_copy_buffer(myfile);
    //bench_choose_L_M(myfile);

}

void run_all_test_on_msiNvidia() {
    std::string size_prefix, computer_name, file_name_prefix;
    // size_prefix could be computed too
    // and computer_name could be found out on run

    log("====== RUN ALL TESTS ON MSI NVIDIA ======");
    log("======                             ======");
    log("====== RUN ALL TESTS ON MSI NVIDIA ======");
    log("======                             ======");

    FORCE_EXECUTION_ON_NAMED_DEVICE = false;
    //MUST_RUN_ON_DEVICE_NAME = "Intel(R) UHD Graphics 620 [0x5917]";

    REPEAT_COUNT_REALLOC = 12;
    REPEAT_COUNT_ONLY_PARALLEL = 0;

    //total_elements = 1024L * 1024L * 256L; // 1 GiB
    total_elements = 1024L * 1024L * 128L; // 512 MiB
    run_all_test_generic("512MiB", "msiNvidia");


}*/

int main(int argc, char *argv[])
{

    init_computers();
    log("========~~~~~~~ VERSION " + DISPLAY_VERSION + " ~~~~~~~========");
    log("argc = " + std::to_string(argc));
    list_devices(exception_handler); // print the list of avaliable devices
    log("\n=== Currently running on " + get_computer_name(currently_running_on_computer_id) + " ===\n");

    std::string computerName = get_computer_name_ofile(currently_running_on_computer_id);

    // if (argc == 1) no argument, only print devices

    // Common variables declaration

    FORCE_EXECUTION_ON_NAMED_DEVICE = true;
    //MUST_RUN_ON_DEVICE_NAME = "Intel(R) UHD Graphics 620 [0x5917]";
    //REPEAT_COUNT_REALLOC = 12;
    REPEAT_COUNT_REALLOC = 12;

    REPEAT_COUNT_ONLY_PARALLEL = 0;

    //total_elements = 1024L * 1024L * 256L;   // 256 milions elements * 4 bytes => 1 GiB
    //std::string size_str = "1GiB";
    //total_elements = 1024L * 1024L * 128L; // 128 milions elements * 4 bytes => 512 MiB
    //std::string size_str = "512MiB";
    
    // g_size_str and total_elements are defined in list_devices()

    // Run all tests at once
    if (argc == 2) {

        std::string arg = argv[1];

        if (arg.compare("mem") == 0) {
            bench_sycl_glibc_mem_speed();
            return 0;
        }

        if (arg.compare("helloworld") == 0) {
            sycl_hello_main();
            return 0;
        }

        
        
        if (arg.compare("traccc") == 0) {
            std::string runCount = "1";
            log("=> Run all  TRACCC  tests at once, runCount(" + runCount + ") <=");
            //traccc::traccc_bench(sycl_mode::glibc);
            //log("Will now sleep.");
            //unsigned int microseconds = 10000000;
            //usleep(microseconds);
            traccc::run_all_traccc_benchs(computerName + "_AT", std::stoi(runCount));
            //traccc::traccc_bench(sycl_mode::glibc);
            //traccc::traccc_bench(sycl_mode::host_USM, traccc::mem_strategy::flatten);
            return 0;
        }


        if (arg.compare("profile_alloc") == 0) {
            profiling_run_test(g_size_str, computerName + "_AT", 1, 1);
            return 0;
        }


        std::string runCount = argv[1];
        if ( ! is_number(runCount) ) {
            log("ERROR, runCount(" + runCount + ") as argv[2] is not a number.");
            return 3;
        }

        log("=> Run all  SYCL_MEM  tests at once, runCount(" + runCount + ") <=");

        // AT stands for All Tests
        run_all_test_generic(g_size_str, computerName + "_AT", std::stoi(runCount));

        /*
        case 1 : return "T580";
        case 2 : return "MSI Intel";
        case 3 : return "MSI Nvidia";
        case 4 : return "SANDOR";
        */
        /*std::string computerName = argv[1];
        if (computerName.compare("thinkpad") == 0) {
            log("Will run on thinkpad.");
        }
        if (computerName.compare("msiIntel") == 0) {
            log("Will run on MSI : intel.");
        }
        if (computerName.compare("msiNvidia") == 0) {
            log("Will run on MSI : Nvidia.");
        }
        if (computerName.compare("sandor") == 0) {
            log("Will run on Sandor.");
        }*/
    }

    // Run one single test
    if (argc == 3) {

        if (std::string(argv[1]).compare("profile_alloc") == 0) {
            profiling_run_test(g_size_str, computerName + "_AT", std::stoi(argv[2]), 1);
            return 0;
        }

        if (std::string(argv[1]).compare("traccc") == 0) {
            if (std::string(argv[2]).compare("sparse") == 0) {
                traccc::run_single_test_generic_traccc(computerName + "_AT", 5, 1);
                traccc::run_single_test_generic_traccc(computerName + "_AT", 6, 1);
                return 0;
            }
            return 0;
        }



        //std::string computerName = argv[1]; deduces from the device list
        std::string testID = argv[1];
        std::string runCount = argv[2];

        if ( ! is_number(testID) ) {
            log("ERROR, testID(" + testID + ") as argv[1] is not a number.");
            return 3;
        }
        if ( ! is_number(runCount) ) {
            log("ERROR, runCount(" + runCount + ") as argv[2] is not a number.");
            return 3;
        }

        log("=> Run single test, testID(" + testID + "), runCount(" + runCount + ") <=");

        // not useful... std::string deviceIndex = argv[4]; // For MSI only, for Intel and AMD GPU
        // ST for "single test"
        run_single_test_generic(g_size_str, computerName + "_ST", std::stoi(testID), std::stoi(runCount));


    }

    // Un seul test de traccc
    if (argc == 4) {

        if (std::string(argv[1]).compare("traccc") != 0) {
            return 0;
        }

        std::string testID = argv[2];
        std::string runCount = argv[3];

        if ( ! is_number(testID) ) {
            log("ERROR, testID(" + testID + ") as argv[2] is not a number.");
            return 3;
        }
        if ( ! is_number(runCount) ) {
            log("ERROR, runCount(" + runCount + ") as argv[3] is not a number.");
            return 3;
        }

        traccc::run_single_test_generic_traccc(computerName + "_AT", std::stoi(testID), std::stoi(runCount));
    }
    
    //run_all_test_on_msiNvidia();
    return 0;
}


// 


/*
To run with syclcc, set those variables :
export HIPSYCL_TARGETS="cuda:sm_35" && \
export HIPSYCL_GPU_ARCH="sm_35" && \
export HIPSYCL_CUDA_PATH="/usr/local/cuda-10.1"

On Sandor :
export HIPSYCL_TARGETS="cuda:sm_75" && \
export HIPSYCL_GPU_ARCH="sm_75" && \
export HIPSYCL_CUDA_PATH="/usr/local/cuda-10.1"
*/