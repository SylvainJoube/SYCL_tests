/// load_data_v105(text_file);

// Benchmark issu de traccc


var file = argument0;

var delimiter = " ";
//show_message("load_data_v105");

var limit_iterations = g_limit_iterations;

while ( ! file_text_eof(file) ) {
    
    var header_str = file_text_readln(file);
    var header_vars = list_to_real(split_string(header_str, " "));
    
    var j = instance_create(0, 0, jobs_fixed);
    init_job_variables(j);
    ds_list_add(ctrl.jobs_fixed_list, j);
    j.DATASET_NUMBER = ds_list_find_value(header_vars, 0);
    //show_message("j.DATASET_NUMBER = " + string(j.DATASET_NUMBER));
    j.INPUT_DATA_SIZE = ds_list_find_value(header_vars, 1);
    j.OUTPUT_DATA_SIZE = ds_list_find_value(header_vars, 2);
    if (g_input_data_size < j.INPUT_DATA_SIZE) g_input_data_size = j.INPUT_DATA_SIZE;
    if (g_output_data_size < j.OUTPUT_DATA_SIZE) g_output_data_size = j.OUTPUT_DATA_SIZE;
    j.PARALLEL_FOR_SIZE = ds_list_find_value(header_vars, 3);
    j.VECTOR_SIZE_PER_ITERATION = ds_list_find_value(header_vars, 4);
    //show_message("L = VECTOR_SIZE_PER_ITERATION = " + string(j.VECTOR_SIZE_PER_ITERATION));
    j.REPEAT_COUNT_REALLOC = ds_list_find_value(header_vars, 5);
    j.REPEAT_COUNT_ONLY_PARALLEL = ds_list_find_value(header_vars, 6);
    j.t_data_generation_and_ram_allocation = ds_list_find_value(header_vars, 7);
    j.t_queue_creation = ds_list_find_value(header_vars, 8);
    
    // NEW with load data V3 :
    j.MEMORY_LOCATION = ds_list_find_value(header_vars, 9); // memory allocated 0 shared ; 1 on device ; 2 host ; 3 buffers
    
    //show_message("Mem location = " + string(j.MEMORY_LOCATION));
    
    // NEW with load data V4 :
    j.MEMCOPY_IS_SYCL = ds_list_find_value(header_vars, 10); // flag to indicate if sycl mem copy or glibc mem copy
    j.SIMD_FOR_LOOP = ds_list_find_value(header_vars, 11);   // flag to indicate wether a traditional for loop was used, or a SIMD GPU-specific loop
    
    // NEW with load data V5 :
    j.USE_NAMED_KERNEL = ds_list_find_value(header_vars, 12);   // flag to indicate wether the kernel was named or not
    
    // NEW with load data V6 :
    j.USE_HOST_SYCL_BUFFER = ds_list_find_value(header_vars, 13);
    
    // NEW with load data V7 : nombre de fois que les accès aux données d'entrée doivent être répétés (test caches et vitesse d'accès aux données)
    j.REPEAT_COUNT_SUM = ds_list_find_value(header_vars, 14);
    
    // NEW with load data v100 - traccc
    j.MEMORY_STRATEGY = ds_list_find_value(header_vars, 15); // 0 graphe de pointeurs ; 1 flatten
    
    // NEW with load data v105 - traccc
    j.IMPLICIT_USE_UNIQUE_MODULE = ds_list_find_value(header_vars, 16);
    
    // NEW with load data v103 - traccc
    //j.IGNORE_ALLOC_TIME = ds_list_find_value(header_vars, 16); // 1 oui ignorer les temps d'allocation ; 0 les prendre en compte
    
    //show_message("j chargé :  MEMORY_STRATEGY(" + string(j.MEMORY_STRATEGY) + ") MEMORY_LOCATION(" + string(j.MEMORY_LOCATION) + ") " + chr(10)
    //+ " REPEAT_COUNT_REALLOC(" + string(j.REPEAT_COUNT_REALLOC) + ") REPEAT_COUNT_ONLY_PARALLEL(" +string(j.REPEAT_COUNT_ONLY_PARALLEL) + ")");
    
    g_display_LM = false;
    g_display_REPEAT_COUNT_SUM = false;
    
    j.datasets = ds_list_create();
    //show_message("j.REPEAT_COUNT_ONLY_PARALLEL = " + string(j.REPEAT_COUNT_ONLY_PARALLEL));
    
    
    if (g_display_LM) {
        // Initialization
        if (g_VECTOR_SIZE_PER_ITERATION_common == -1) {
            g_VECTOR_SIZE_PER_ITERATION_common = j.VECTOR_SIZE_PER_ITERATION;
            g_PARALLEL_FOR_SIZE_common = j.PARALLEL_FOR_SIZE;
        } else {
            // Already been set, and diffrent values : no common value, do not display
            if ( (g_VECTOR_SIZE_PER_ITERATION_common != j.VECTOR_SIZE_PER_ITERATION)
            or   (g_PARALLEL_FOR_SIZE_common != j.PARALLEL_FOR_SIZE) ) {
                g_display_LM = false;
            }
        }
    }
    
    // Only updated from v7, so no display on earlier versions
    // and only display if j.REPEAT_COUNT_SUM != 1
    if (g_display_REPEAT_COUNT_SUM) {
    
        if ( (g_REPEAT_COUNT_SUM_common == -1) ) {
            g_REPEAT_COUNT_SUM_common = j.REPEAT_COUNT_SUM;
        } else {
            if ( g_REPEAT_COUNT_SUM_common != j.REPEAT_COUNT_SUM ) {
                g_REPEAT_COUNT_SUM_common = -1;
                g_display_REPEAT_COUNT_SUM = false;
            }
        }
        
        if (g_REPEAT_COUNT_SUM_common == 1) {
            g_REPEAT_COUNT_SUM_common = -1;
            g_display_REPEAT_COUNT_SUM = false;
        }
    }
    
    
    for (var i_dataset = 0; i_dataset < j.DATASET_NUMBER; ++i_dataset) {
        var seed_str = "0"; //file_text_readln(file);
        //var seed_vars = list_to_real(seed_str);
        var d = instance_create(0, 0, dataset_fixed);
        ds_list_add(j.datasets, d);
        d.seed = real(seed_str); //ds_list_find_value(seed_vars, 0); // equals real(seed_str) in fact
        d.iterations = ds_list_create();
        d.iterations_only_parallel = ds_list_create();
        
        for (var i_iteration = 0; i_iteration < j.REPEAT_COUNT_REALLOC; ++i_iteration) {
            var values_str = file_text_readln(file);
            
            if (limit_iterations != -1) {
                if (i_iteration >= limit_iterations) continue;
            }
            
            var iter = instance_create(0, 0, iteration);
            ds_list_add(d.iterations, iter);
            var values = list_to_real(split_string(values_str, " "));
            iter.t_allocation = ds_list_find_value(values, 0);
            
            iter.t_alloc_only = iter.t_allocation; // raccourci acat
            
            // used if USE_HOST_SYCL_BUFFER = true
            iter.t_sycl_host_alloc = ds_list_find_value(values, 1); // new in v6
            iter.t_sycl_host_copy = ds_list_find_value(values, 2); // new in v6
            // if USE_HOST_SYCL_BUFFER, this is malloc_host -> shared/device/host
            // otherwise this is (classic buffer alocated with new) -> shared/device/host
            iter.t_copy_to_device = ds_list_find_value(values, 3);
            
            iter.t_fill_only = iter.t_copy_to_device; // raccourci acat
            
            iter.t_sycl_host_free = ds_list_find_value(values, 4); // new in v6
            iter.t_parallel_for = ds_list_find_value(values, 5);
            iter.t_read_from_device = ds_list_find_value(values, 6);
            iter.t_free_gpu = ds_list_find_value(values, 7);
            
            // Seules ces valeurs sont utilisées :
            iter.t_alloc_fill = ds_list_find_value(values, 8);
            iter.t_copy_kernel = ds_list_find_value(values, 9);
            iter.t_read = ds_list_find_value(values, 10);
            iter.t_free_mem = ds_list_find_value(values, 11);
            iter.t_flatten_alloc = ds_list_find_value(values, 12);
            iter.t_flatten_fill = ds_list_find_value(values, 13);
            // + nouvelles variables acat : iter.t_alloc_only et iter.t_fill_only 
        }
        
    }
    
    // REPEAT_COUNT_ONLY_PARALLEL n'est pas utilisé dans traccc
    
    if (false)
    for (var i_dataset = 0; i_dataset < j.DATASET_NUMBER; ++i_dataset) {
        var seed_str = file_text_readln(file);
        //var seed_str2 = file_text_readln(file);
        //show_message("seed str 1 et 2 : " + seed_str + "  -  " + seed_str2);
        
        var d = ds_list_find_value(j.datasets, i_dataset);
        //d.iterations_only_parallel = ds_list_create();
        
        for (var i_iteration = 0; i_iteration < j.REPEAT_COUNT_ONLY_PARALLEL; ++i_iteration) {
            var values_str = file_text_readln(file);
            
            if (limit_iterations != -1) {
                if (i_iteration >= limit_iterations) continue;
            }
            var iter = instance_create(0, 0, iteration_only_parallel);
            ds_list_add(d.iterations_only_parallel, iter);
            var values = list_to_real(split_string(values_str, " "));
            //iter.t_allocation = ds_list_find_value(values, 0);
            //iter.t_copy_to_device = ds_list_find_value(values, 1);
            //iter.t_parallel_for = ds_list_find_value(values, 2);
            //iter.t_read_from_device = ds_list_find_value(values, 3);
            //iter.t_free_gpu = ds_list_find_value(values, 4);
            iter.t_parallel_for = ds_list_find_value(values, 0);
            iter.t_read_from_device = ds_list_find_value(values, 1);
        }
        
        var common_values_str = file_text_readln(file);
        var values = list_to_real(split_string(common_values_str, " "));
        var t_allocation = ds_list_find_value(values, 0);
        // used if USE_HOST_SYCL_BUFFER = true
        var t_sycl_host_alloc = ds_list_find_value(values, 1); // new in v6
        var t_sycl_host_copy = ds_list_find_value(values, 2); // new in v6
        // if USE_HOST_SYCL_BUFFER, this is malloc_host -> shared/device/host
        // otherwise this is (classic buffer alocated with new) -> shared/device/host
        var t_copy_to_device = ds_list_find_value(values, 3);
        var t_sycl_host_free = ds_list_find_value(values, 4);
        var t_free_gpu = ds_list_find_value(values, 5);
        //show_message("t_free_gpu = " + string(t_free_gpu) + "  t_allocation = " + string(t_allocation));
        
        for (var i_iteration = 0; i_iteration < j.REPEAT_COUNT_ONLY_PARALLEL; ++i_iteration) {
            var iter = ds_list_find_value(d.iterations_only_parallel, i_iteration);
            iter.t_allocation = t_allocation;
            iter.t_sycl_host_alloc = t_sycl_host_alloc;
            iter.t_sycl_host_copy = t_sycl_host_copy;
            iter.t_copy_to_device = t_copy_to_device;
            iter.t_sycl_host_free = t_sycl_host_free;
            iter.t_free_gpu = t_free_gpu;
        }
    }
    
    if (limit_iterations != -1) {
        j.REPEAT_COUNT_ONLY_PARALLEL = min(j.REPEAT_COUNT_ONLY_PARALLEL, limit_iterations);
        j.REPEAT_COUNT_REALLOC = min(j.REPEAT_COUNT_REALLOC, limit_iterations);
    }
}

//var eof = file_text_eof(file);
//show_message("eof = " + string(eof));

//show_message("ctrl.jobs_fixed_list size = " + string(ds_list_size(ctrl.jobs_fixed_list)));

//check_load_data();
