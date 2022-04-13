#pragma once

// SyCL specific includes
#include <CL/sycl.hpp>

inline namespace cl {
    namespace sycl {
        constexpr property::noinit no_init = cl::sycl::noinit;
    };
};