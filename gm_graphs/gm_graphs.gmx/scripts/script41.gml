/// init_iter_variables(iter : object of type iteration) : void


iter.t_allocation = 0;
iter.t_sycl_host_alloc = 0;
iter.t_sycl_host_copy = 0;
iter.t_copy_to_device = 0; // from malloc_host then and not the traditional buffer allocated with new
iter.t_parallel_for = 0;
iter.t_read_from_device = 0;
iter.t_free_gpu = 0;
