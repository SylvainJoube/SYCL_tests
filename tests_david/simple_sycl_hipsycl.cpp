
// SyCL specific includes
#include <CL/sycl.hpp>
#include <iostream>

using namespace cl::sycl;

// ========== Version avec hipSYCL ==========

// syclcc -O2 -std=c++17 -o simple_sycl_hipsycl simple_sycl_hipsycl.cpp && ./simple_sycl_hipsycl



int main() {

  std::cout << "SYCL program starts..." << std::endl;

  uint size = 1024;

  // Prepare host data
  std::vector<double> data(size) ;
  std::vector<double> results(size) ;

  // Initialize data
  for (uint i = 0; i < size; ++i) {
      data[i] = i;
  }

  // Device buffers
  buffer<double, 1> b_data(data.data(), range<1>(size)) ;
  buffer<double, 1> b_results(results.data(), range<1>(size)) ;

  // Optionnel, un device_selector automatique qu'il est possible de remplacer
  // The default device selector will select the most performant device.
  //cl::sycl::default_selector d_selector;

  // Compute
  queue myQueue/*(d_selector)*/;
  //queue myQueue; // fonctionne également, utilise aussi un selector automatique

  myQueue.submit([&](handler &h) {

    // Initialisation via le constructeur des accesseurs
    accessor a_data(b_data, h, read_only);
    accessor a_results(b_results, h, write_only); // noinit non supporté par hipsycl visiblement
      
    // Initialisation via la méthode get_access des buffers
    //auto a_data_bis    = b_data.get_access<access::mode::read>(h);
    //auto a_results_bis = b_results.get_access<access::mode::discard_write>(h);
    // L'argument "h" n'est même pas réellement nécessaire ici

    h.parallel_for(range<1>(size), [=](auto i) {
        uint index = i[0];
        a_results[index] = a_data[index] * a_data[index];
    });
  });

  // Deux manières de faire en sorte que les données du buffer aillent dans
  // le vecteur original passé en argument :
  // - soit lorsque le buffer sort du scope (destruction du buffer)
  // - soit via la méthode suivante :
  b_results.get_access<access::mode::read>();

  // Check the results
  bool failure = false;
  for (uint i = 0; i < size; ++i) {
      double expected = data[i] * data[i];
      double found = results[i];
      if ( expected != found ) {
        failure = true;
        std::cout << "Wrong value, expected " << expected << " but found"
                  << found << std::endl;
      }
  }
  if ( ! failure ) {
      std::cout << "Success !!" << std::endl;
  }

  // Je n'ai pas trop étudié les command_group, mais :
  // myQueue.submit encapsule visiblement un command_group
  // cf : https://developer.codeplay.com/products/computecpp/ce/guides/sycl-guide/hello-sycl
 
  /*command_group(myQueue, [&]() {
      // data accessors
      auto a_data = b_data.get_access<access::read>();
      auto a_results = b_results.get_access<access::write>();
      // kernel
      parallel_for( count, kernel_functor( [=](id<> item) {
        int i = item.get_global(0);
        a_results[i] = a_data[i] * a_data[i];
      }));
    });

    // Process results
    ...*/
  }