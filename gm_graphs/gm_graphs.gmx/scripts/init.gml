/// init();


g_display_device = true;
g_display_shared = true;
g_display_host   = true;
g_ymax_impose = -1;
g_ptrVsFlat_firstStep = false;
g_draw_shared_before_shared = true;
g_split_thousands_USE_MS = true; // instead of us

// Sets the global variables

g_graph_title = "Mon zouli graphe.";
g_graph_surface = -1;
g_display_error_list = ds_list_create();
g_graph_object = -1;
g_display_error_list = ds_list_create();

// Dimensions du graphe
//g_graph_yoffset = 160;
g_graph_yoffset = 90;
g_graph_label_ystart = 70;
g_origin_arrow_size = 15;
//g_graph_height = 700;
//g_graph_width = 800;
g_graph_height = 600;//360; // + 160
g_graph_width = 800; // 600 x 360 dans rapport de base

g_line_pts_link_width = 1;

g_xorig = 136;
g_yorig = g_graph_height + g_graph_yoffset;
g_surface_width = g_graph_width + g_xorig + 100;
g_surface_height = g_graph_height + g_graph_yoffset + 70;

// Distance minimale entre deux labels dessinés
g_same_xgroup_min_label_distance = 12; // 12

g_citer = 0; // debug only

// true if each xgroup should have its own x axis
// false if only one x axis on global xorig
g_multiple_xaxis = false;

// une même échelle pour un x donné (true)
// ou une échelle commune pour tous les points du graphe (false)
g_xgroup_has_own_scale = false;


var base_dir_load = "H:\SYNCTHING\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench";
//var base_dir_save = "H:\SYNCTHING\data_sync\academique\M2\StageM2\SYCL_tests\gm_graphs\gm_graphs.gmx\save_png\";
var base_dir_save = "H:\SYNCTHING\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\mem_benck\";

var h_version = "53";

g_multiple_load_LM = true;
g_graph_display_name = "Nom inconnu";
g_multiple_load_file_number = 1;
g_multiple_load_file_current_count = 0;

// Dessiner ou non les temps d'allocation et de copie
// depuis le buffer temporaire
DISPLAY_TEMP_COPY_BUFF_TIMES = false;



// ======== TRACCC ========

// Variables modifiées lors du lancement des benchmarks
g_traccc_draw_graph_ptr = true;
g_traccc_draw_flatten = true;
g_traccc_ignore_allocation_time = false; // ne prendre que le temps de fill
g_traccc_ptrVsFlat_memLocation = -1;
traccc_repeat_load_count = 1; // nombre de fois que les fichiers sont chargés (pour simuler + de données)
traccc_hide_host = false;




// Pour le chargement rapide d'un fichier sans toutes les étapes intermédiaires
g_debug_fast_load = true;

// TODO : rendre compatible la nouvelle version avec le graphe de L et M.

//g_load_path = base_dir_load + "sh_output_bench_h" + h_version + ".shared_txt";

//g_load_path = base_dir_load + "sandor_h59_L_M_1G.t";
//g_load_path = base_dir_load + "sandor_h60_L_M_3GiB_O2.t";
//g_load_path = base_dir_load + "msi_h60_L_M_128MiB_O0.t";
//g_load_path = base_dir_load + "msi_h60_alloclib_1GiB_O2.t";
//g_load_path = base_dir_load + "msi_h60_simd_1GiB_O2_20pts.t";
g_load_path = base_dir_load + "fichier"; //"sandor_h60_L_M_4GiB_O2.t";

DEFAULT_COMPUTER_ID = 0; // unknown
DEFAULT_MEMORY_BANDWIDTH = 1; // GiB/s, unknown

// Bande passante en GiB/s
COMPUTER_GPU_BANDWIDTH[0] = 1; // on connu
COMPUTER_GPU_BANDWIDTH[1] = 37.5; // thinkpad CPU bandwidth
COMPUTER_GPU_BANDWIDTH[2] = 1;    // msi intel : non connu
COMPUTER_GPU_BANDWIDTH[3] = 80;   // msi nvidia GeForce GTX 960M
COMPUTER_GPU_BANDWIDTH[4] = 448;  // sandor Quadro RTX 5000

DATA_ITEM_SIZE = 4; // a float or an int, accounting for 4 bytes in each case

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
