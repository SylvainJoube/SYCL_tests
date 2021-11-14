
// SyCL specific includes
#include <CL/sycl.hpp>
#include <iostream>

using namespace cl::sycl;

// ========== Version avec hipSYCL ==========
// fonctionne avec dpcpp et hipsycl

// dpcpp -O2 -std=c++17 -o hipsycl_complete.bin hipsycl_complete.cpp && ./hipsycl_complete.bin



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

  buffer<double, 1> *pb_data;
  buffer<double, 1> *pb_results;

  std::cout << "Buffer creation..." << std::endl;

  // Buffer allocation
  pb_data    = new buffer<double, 1>(data.data(), range<1>(size));
  pb_results = new buffer<double, 1>(results.data(), range<1>(size));

  queue myQueue;

  std::cout << "Buffer filling..." << std::endl;
  std::cout << "Buffer size = " << pb_data->get_size() << "." << std::endl;
  
  /*myQueue.submit([&](handler &h) {

    // Initialisation via le constructeur des accesseurs
    //accessor a_data(*pb_data, h, read_write);

    auto a_data = (*pb_data).get_access<access::mode::discard_write>(h);

    for (uint i = 0; i < 1; ++i) {
      a_data[i] = 1;//data[i];
    }

  }).wait();*/


  // Device buffers
  //buffer<double, 1> b_data(data.data(), range<1>(size)) ;
  //buffer<double, 1> b_results(results.data(), range<1>(size)) ;


  std::cout << "Compute..." << std::endl;

  
  myQueue.submit([&](handler &h) {

    // Initialisation via le constructeur des accesseurs
    accessor a_data(*pb_data, h, read_only);
    accessor a_results(*pb_results, h, write_only); // noinit non supporté par hipsycl visiblement
      
    // Initialisation via la méthode get_access des buffers
    //auto a_data_bis    = b_data.get_access<access::mode::read>(h);
    //auto a_results_bis = b_results.get_access<access::mode::discard_write>(h);
    // L'argument "h" n'est même pas réellement nécessaire ici

    h.parallel_for<class hello>(range<1>(size), [=](auto i) {
        uint index = i[0];
        a_results[index] = a_data[index] * a_data[index];
    });
  }).wait();


  /*{
    // Initialisation via le constructeur des accesseurs
    accessor a_data(*pb_data, read_only);
    accessor a_results(*pb_results, write_only); // noinit non supporté par hipsycl visiblement
      
    // Initialisation via la méthode get_access des buffers
    //auto a_data_bis    = b_data.get_access<access::mode::read>(h);
    //auto a_results_bis = b_results.get_access<access::mode::discard_write>(h);
    // L'argument "h" n'est même pas réellement nécessaire ici

    myQueue.parallel_for(range<1>(size), [=](auto i) {
        uint index = i[0];
        a_results[index] = a_data[index] * a_data[index];
    }).wait();
  }*/

  std::cout << "Get access to results..." << std::endl;

  // Deux manières de faire en sorte que les données du buffer aillent dans
  // le vecteur original passé en argument :
  // - soit lorsque le buffer sort du scope (destruction du buffer)
  // - soit via la méthode suivante :
  (*pb_results).get_access<access::mode::read>();

  std::cout << "Checking the results..." << std::endl;

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

  std::cout << "Buffer destruction..." << std::endl;

  delete pb_data;
  delete pb_results;
  


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