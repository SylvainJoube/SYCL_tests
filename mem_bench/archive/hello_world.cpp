#include <iostream>

// SyCL specific includes
#include <CL/sycl.hpp>
#include <array>
#include <sys/time.h>

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


void main_sequence() {
    // SyCL test code
    // The default device selector will select the most performant device.
    cl::sycl::default_selector d_selector;

    try {
        cl::sycl::queue sycl_q(d_selector, exception_handler);

        // Print out the device information used for the kernel code.
        std::cout << "Running on device: "
                << sycl_q.get_device().get_info<cl::sycl::info::device::name>() << "\n";
        
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


    std::cout << "SYCL memory benchmark." << std::endl;

    main_sequence();

    return 0;

}