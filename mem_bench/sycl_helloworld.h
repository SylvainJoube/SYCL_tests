

#include <iostream>

// SyCL specific includes
#include <CL/sycl.hpp>

#pragma once


//class sycl_hello_main {

void sycl_hello_main() {

    // SyCL asynchronous exception handler
    // Create an exception handler for asynchronous SYCL exceptions
    static auto exception_handler = [](cl::sycl::exception_list e_list) {
        for (std::exception_ptr const &e : e_list) {
            try {
                std::rethrow_exception(e);
            } catch (std::exception const &e) {
                std::cout << "Failure" << std::endl;
                std::terminate();
            }
        }
    };

    // The default device selector will select the most performant device.
    //cl::sycl::default_selector d_selector;
    cl::sycl::default_selector d_selector;

    using data_type = int;
    
    try {
        // Simple addition of two vectors
        
        cl::sycl::queue sycl_q(d_selector, exception_handler);
        sycl_q.wait_and_throw();

        // Declare the host vectors
        const uint vector_size = 10;
        data_type input_a[vector_size];
        data_type input_b[vector_size];
        data_type output [vector_size];
        
        // Fill the host vectors with some numbers
        for (uint i = 0; i < vector_size; ++i) {
            input_a[i] = i;
            input_b[i] = 10 + i * 2;
        }

        // Allocation of sycl memory
        data_type * input_a_sycl = cl::sycl::malloc_host<data_type>(vector_size, sycl_q);
        data_type * input_b_sycl = cl::sycl::malloc_host<data_type>(vector_size, sycl_q);
        data_type * output_sycl  = cl::sycl::malloc_host<data_type>(vector_size, sycl_q);

        // Copy from host memory to sycl memory
        sycl_q.memcpy(input_a_sycl, input_a, vector_size * sizeof(data_type)).wait();
        sycl_q.memcpy(input_b_sycl, input_b, vector_size * sizeof(data_type)).wait();

        // Kernel : vectors sum
        class SyclHelloworldKernel;
        
        sycl_q.parallel_for(cl::sycl::range<1>(vector_size), [=](cl::sycl::id<1> cell_index) {
            auto i = cell_index.get(0);
            output_sycl[i] = input_a_sycl[i] + input_b_sycl[i];
        });

        // Copy from sycl memory to host memory
        sycl_q.memcpy(output, output_sycl, vector_size * sizeof(data_type)).wait();

        bool has_error = false;
        // Check the result
        for (uint i = 0; i < vector_size; ++i) {
            data_type sum = input_a[i] + input_b[i];
            if (output[i] != sum) {
                has_error = true;
                std::cout << "SYCL HELLOWORLD ERROR : at index " << i << " expected "
                            << sum << " but found " << output[i] << "."
                            << std::endl;
            }
        }

        // Free sycl memory
        cl::sycl::free(input_a_sycl, sycl_q);
        cl::sycl::free(input_b_sycl, sycl_q);
        cl::sycl::free(output_sycl,  sycl_q);

        if (has_error) {
            std::cout << "SYCL HELLOWORLD FAILED." << std::endl;
        } else {
            std::cout << "SYCL HELLOWORLD SUCCESS !" << std::endl;
        }

    } catch (cl::sycl::exception const &e) {
        std::cout << "SYCL HELLOWORLD ERROR : An exception has been caught while processing SyCL code.\n";
    }
}

//};