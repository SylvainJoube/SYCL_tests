/// load_data(fpath);

var fpath = argument0;
var delimiter = " ";

ctrl.jobs_fixed_list = ds_list_create();

fpath = get_open_filename("", fpath);

var file = file_text_open_read(fpath);

//show_message("file = " + string(file) + " - path = " + fpath);


while ( ! file_text_eof(file) ) {
    
    var header_str = file_text_readln(file);
    var header_vars = list_to_real(split_string(header_str, " "));
    
    var j = instance_create(0, 0, jobs_fixed);
    ds_list_add(ctrl.jobs_fixed_list, j);
    j.DATASET_NUMBER = ds_list_find_value(header_vars, 0);
    //show_message("j.DATASET_NUMBER = " + string(j.DATASET_NUMBER));
    j.INPUT_DATA_SIZE = ds_list_find_value(header_vars, 1);
    j.OUTPUT_DATA_SIZE = ds_list_find_value(header_vars, 2);
    j.PARALLEL_FOR_SIZE = ds_list_find_value(header_vars, 3);
    j.VECTOR_SIZE_PER_ITERATION = ds_list_find_value(header_vars, 4);
    j.REPEAT_COUNT_REALLOC = ds_list_find_value(header_vars, 5);
    j.REPEAT_COUNT_ONLY_PARALLEL = ds_list_find_value(header_vars, 6);
    j.t_data_generation_and_ram_allocation = ds_list_find_value(header_vars, 7);
    j.t_queue_creation = ds_list_find_value(header_vars, 8);
    j.datasets = ds_list_create();
    
    
    for (var i_dataset = 0; i_dataset < j.DATASET_NUMBER; ++i_dataset) {
        var seed_str = file_text_readln(file);
        //var seed_vars = list_to_real(seed_str);
        var d = instance_create(0, 0, dataset_fixed);
        ds_list_add(j.datasets, d);
        d.seed = real(seed_str); //ds_list_find_value(seed_vars, 0); // equals real(seed_str) in fact
        d.iterations = ds_list_create();
        d.iterations_only_parallel = ds_list_create();
        
        for (var i_iteration = 0; i_iteration < j.REPEAT_COUNT_REALLOC; ++i_iteration) {
            var iter = instance_create(0, 0, iteration);
            ds_list_add(d.iterations, iter);
            var values_str = file_text_readln(file);
            var values = list_to_real(split_string(values_str, " "));
            iter.t_allocation = ds_list_find_value(values, 0);
            iter.t_copy_to_device = ds_list_find_value(values, 1);
            iter.t_parallel_for = ds_list_find_value(values, 2);
            iter.t_read_from_device = ds_list_find_value(values, 3);
            iter.t_free_gpu = ds_list_find_value(values, 4);
        }
        
        
    }
    
    
    for (var i_dataset = 0; i_dataset < j.DATASET_NUMBER; ++i_dataset) {
        var seed_str = file_text_readln(file);
        var d = ds_list_find_value(j.datasets, i_dataset);
        //d.iterations_only_parallel = ds_list_create();
        
        for (var i_iteration = 0; i_iteration < j.REPEAT_COUNT_ONLY_PARALLEL; ++i_iteration) {
            var iter = instance_create(0, 0, iteration_only_parallel);
            ds_list_add(d.iterations_only_parallel, iter);
            var values_str = file_text_readln(file);
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
        var values = list_to_real(split_string(values_str, " "));
        var t_allocation = ds_list_find_value(values, 0);
        var t_copy_to_device = ds_list_find_value(values, 1);
        var t_free_gpu = ds_list_find_value(values, 2);
        
        for (var i_iteration = 0; i_iteration < j.REPEAT_COUNT; ++i_iteration) {
            var iter = ds_list_find_value(d.iterations_only_parallel, i_iteration);
            iter.t_allocation = t_allocation;
            iter.t_copy_to_device = t_copy_to_device;
            iter.t_free_gpu = t_free_gpu;
        }
    }
}

//var eof = file_text_eof(file);
//show_message("eof = " + string(eof));

//show_message("ctrl.jobs_fixed_list size = " + string(ds_list_size(ctrl.jobs_fixed_list)));

file_text_close(file);

check_load_data();



