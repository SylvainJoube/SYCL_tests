/// check_load_data();

var spath = "E:\L3 DANT disque E\mem_benck\verif.txt";
spath = get_save_filename("", spath);
var file = file_text_open_write(spath);

for (var ij = 0; ij < ds_list_size(ctrl.jobs_fixed_list); ++ij) {
    var j = ds_list_find_value(ctrl.jobs_fixed_list, ij);
    //show_message("j save = " + string(j));
    var str = "";
    str +=  string(j.DATASET_NUMBER) + " ";
    str +=  string(j.INPUT_DATA_SIZE) + " ";
    str +=  string(j.OUTPUT_DATA_SIZE) + " ";
    str +=  string(j.PARALLEL_FOR_SIZE) + " ";
    str +=  string(j.VECTOR_SIZE_PER_ITERATION) + " ";
    str +=  string(j.REPEAT_COUNT_REALLOC) + " ";
    str +=  string(j.REPEAT_COUNT_ONLY_PARALLEL) + " ";
    str +=  string(j.t_data_generation_and_ram_allocation) + " ";
    str +=  string(j.t_queue_creation);
    file_text_write_string(file, str);
    file_text_writeln(file);
    
    for (var ids = 0; ids < ds_list_size(j.datasets); ++ids) {
        var d = ds_list_find_value(j.datasets, ids);
        file_text_write_string(file, string(d.seed));
        file_text_writeln(file);
        
        for (var i_iteration = 0; i_iteration < ds_list_size(d.iterations); ++i_iteration) {
            var iter = ds_list_find_value(d.iterations, i_iteration);
            var str = "";
            str +=  string(iter.t_allocation) + " ";
            str +=  string(iter.t_copy_to_device) + " ";
            str +=  string(iter.t_parallel_for) + " ";
            str +=  string(iter.t_read_from_device) + " ";
            str +=  string(iter.t_free_gpu);
            file_text_write_string(file, str);
            file_text_writeln(file);
        }
    }
    
    
    for (var ids = 0; ids < ds_list_size(j.datasets); ++ids) {
        var d = ds_list_find_value(j.datasets, ids);
        file_text_write_string(file, string(d.seed));
        file_text_writeln(file);
        
        for (var i_iteration = 0; i_iteration < ds_list_size(d.iterations_only_parallel); ++i_iteration) {
            var iter = ds_list_find_value(d.iterations_only_parallel, i_iteration);
            var str = "";
            str +=  string(iter.t_parallel_for) + " ";
            str +=  string(iter.t_read_from_device);
            file_text_write_string(file, str);
            file_text_writeln(file);
        }
    }
}

file_text_close(file);
