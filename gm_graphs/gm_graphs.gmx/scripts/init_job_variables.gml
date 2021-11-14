/// init_job_variables(job);

// to provide retrocompatibility of old tests

var j = argument0;

j.MEMORY_LOCATION = 1; // Default : memory allocated on device, explicit transfert
j.MEMCOPY_IS_SYCL = 1;
j.SIMD_FOR_LOOP = 0;
j.USE_NAMED_KERNEL = 0;
// do a SYCL copy from buffer to malloc_host, and then
// copy from this malloc_host to malloc_device or shared.
j.USE_HOST_SYCL_BUFFER = 0;
j.REPEAT_COUNT_SUM = 1;
j.FILE_NAME = g_file_name; // in case multiple files were loaded
j.FILE_COUNT = g_multiple_load_file_current_count;

j.COMPUTER_ID = DEFAULT_COMPUTER_ID;
// DEFAULT_COMPUTER_ID initialisé dans Init()
j.memory_bandwidth = DEFAULT_MEMORY_BANDWIDTH; // GiB/s

j.IGNORE_ALLOC_TIME = 0; // temps d'allocation pris en compte

// IMPLICIT_USE_UNIQUE_MODULE je veux que ça plante si j'ai pas initialisé ça !

return 0;
