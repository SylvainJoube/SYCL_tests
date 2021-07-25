/// init();

// Sets the global variables

var base_dir_load = "H:\SYNCTHING\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\";
//var base_dir_save = "H:\SYNCTHING\data_sync\academique\M2\StageM2\SYCL_tests\gm_graphs\gm_graphs.gmx\save_png\";
var base_dir_save = "H:\SYNCTHING\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\mem_benck\";

var h_version = "53";

//g_load_path = base_dir_load + "sh_output_bench_h" + h_version + ".shared_txt";

//g_load_path = base_dir_load + "sandor_h59_L_M_1G.t";
//g_load_path = base_dir_load + "sandor_h60_L_M_3GiB_O2.t";
//g_load_path = base_dir_load + "msi_h60_L_M_128MiB_O0.t";
//g_load_path = base_dir_load + "msi_h60_alloclib_1GiB_O2.t";
//g_load_path = base_dir_load + "msi_h60_simd_1GiB_O2_20pts.t";
g_load_path = base_dir_load + "sandor_h60_L_M_4GiB_O2.t";



///g_save_path_default = base_dir_load + "mem_benck\sh_save_surf_h" + h_version + ".png";

//g_save_path_default = base_dir + "mem_benck\sh_save_surf_h" + h_version + ".png";

g_save_path_default_base_path = base_dir_save + "sh_save_surf_h" + h_version;
g_save_path_extension = ".png";
g_iteration_count = 0; // default initialization



g_limit_iterations = -1;//12; // -1

var dll_name = working_directory + "GmGraphsDLL.dll";
g_dll_name = dll_name;
//show_message("exists ? " + dll_name + chr(10) + "" + string(file_exists(dll_name)));

dll_dFileExists = external_define(dll_name, "dFileExists", dll_cdecl, ty_real, 2, ty_real, ty_real);

//var ex = dFileExists("H:\SYNCTHING\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\mem_benck\sh_save_surf_h40o.png");
//show_message("ex = " + string(ex));
