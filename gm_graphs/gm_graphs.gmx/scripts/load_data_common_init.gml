/// load_data_common_init();
// common initialization, just before the load_data_vX(file).

ctrl.jobs_fixed_list = ds_list_create();

// used to draw L and M if they are the same every time
g_VECTOR_SIZE_PER_ITERATION_common = -1; // L
g_PARALLEL_FOR_SIZE_common = -1; // M

g_display_LM = true;

g_input_data_size = 0;
g_output_data_size = 0;