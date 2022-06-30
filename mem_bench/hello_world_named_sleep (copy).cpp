#include <iostream>

// SyCL specific includes
#include <CL/sycl.hpp>
#include <array>
#include <sys/time.h>
#include <unistd.h>


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

using data_type = int;

void main_sequence() {
    // SyCL test code
    // The default device selector will select the most performant device.
    cl::sycl::default_selector d_selector;

    try {
        cl::sycl::queue sycl_q(d_selector, exception_handler);

        // Print out the device information used for the kernel code.
        std::cout << "Running on device: "
                << sycl_q.get_device().get_info<cl::sycl::info::device::name>() << "\n";
        
        class MyKernel;

        const int INPUT_DATA_LENGTH = 4;
        const int INPUT_DATA_SIZE = sizeof(data_type) * INPUT_DATA_LENGTH;
        const int OUTPUT_DATA_SIZE = sizeof(data_type) * 1;

        data_type * shared_mem_input = static_cast<data_type *> (cl::sycl::malloc_shared(INPUT_DATA_SIZE, sycl_q));
        data_type * shared_mem_output = static_cast<data_type *> (cl::sycl::malloc_shared(OUTPUT_DATA_SIZE, sycl_q));

        for (int i = 0; i < INPUT_DATA_LENGTH; ++i) {
            shared_mem_input[i] = i + 1;
        }

        sycl_q.submit([&](cl::sycl::handler& h) {

            // Explicitly name kernel with previously forward declared type
            h.single_task<MyKernel>([=]{
                // [kernel code]
                data_type sum = 0;
                for (int i = 0; i < INPUT_DATA_SIZE; ++i) {
                    sum += shared_mem_input[i];
                }

                shared_mem_output[0] = sum;
            });

            // Explicitly name kernel without forward declaring type at
            // namespace scope.  Must still be forward declarable at
            // namespace scope, even if not declared at that scope
            /*h.single_task<class MyOtherKernel>([=]{
                // [kernel code]
            });*/
        }).wait();

        data_type from_device = shared_mem_output[0];
        
        data_type res_verif = 0;
        for (int i = 0; i < INPUT_DATA_SIZE; ++i) {
            res_verif += shared_mem_input[i];
        }

        if (res_verif != from_device) {
            std::cout << "v ERROR v ERROR v ERROR v\n"
                      << "ERROR sums do not match : verif("
                      << res_verif
                      << ")  dev("
                      << from_device
                      << ")\n"
                      << "^ ERROR ^ ERROR ^ ERROR ^\n";
        } else {
            std::cout << "VALID sums do match : "
                      << res_verif
                      << ")\n";
        }

        cl::sycl::free(shared_mem_input, sycl_q);
        cl::sycl::free(shared_mem_output, sycl_q);

        log("It works !");

    } catch (cl::sycl::exception const &e) {
        std::cout << "An exception is caught while processing SyCL code.\n";
        std::terminate();
    }
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


    std::cout << "SYCL memory benchmark + sleep... arg1 = " + std::string(argv[1]) << std::endl;

    unsigned int microseconds = 1000 * 1000; // 1000 ms = 1s

    std::cout << "Sleep ok - arg1 = " + std::string(argv[1]) << std::endl;

    usleep(microseconds);

    main_sequence();

    return 0;

}