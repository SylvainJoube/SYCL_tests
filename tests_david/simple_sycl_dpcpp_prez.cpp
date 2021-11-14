
// ========== Version pour la présentation avec dpc++ ==========
// (version simplifiée mais qui compile)

// dpcpp -O2 -std=c++17 -o simple_sycl_dpcpp_prez simple_sycl_dpcpp_prez.cpp && ./simple_sycl_dpcpp_prez

const int DATA_SIZE = 1024;

// début du code à copier :

#include <CL/sycl.hpp>

using namespace cl::sycl;

int main() {

  // Prepare host data
  std::vector<double> data(DATA_SIZE) ;
  std::vector<double> results(DATA_SIZE) ;

  // SYCL buffers
  buffer b_data(data) ;
  buffer b_results(results) ;

  // Selects the most performant device.
  queue myQueue;

  myQueue.submit([&](handler &h) {

    // Data accessors
    accessor a_data(b_data, h, read_only);
    accessor a_results(b_results, h, write_only, no_init);
    
    // Kernel
    h.parallel_for(range<1>(DATA_SIZE), [=](auto i) {
        uint index = i[0];
        a_results[i] = a_data[i] * a_data[i];
    });
  });

  // Retrieve data from the buffer into the original vector
  b_results.get_access<access::mode::read>();

  // Process results ...

  }