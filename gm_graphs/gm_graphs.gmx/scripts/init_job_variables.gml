/// init_job_variables(job);

// to provide retrocompatibility of old tests

var j = argument0;

j.MEMORY_LOCATION = 1; // Default : memory allocated on device, explicit transfert
j.MEMCOPY_IS_SYCL = 1;
j.SIMD_FOR_LOOP = 0;
j.USE_NAMED_KERNEL = 0;

return 0;
