/// load_data_common(fpath);

g_multiple_load_file_current_count = 1;


// g_multiple_load_LM is declared in init()

var nbfiles = 1;
if ( g_multiple_load_LM && ( ! g_debug_fast_load ) ) {
    nbfiles = get_integer("Nombre de fichiers à lire :", nbfiles);
}

var l_file_name = ds_list_create();
var output_base_path = "";
g_multiple_load_file_number = nbfiles;

load_data_common_init();

// \output_bench

for (var ifile = 0; ifile < nbfiles; ++ifile) {
    var fpath = argument0 + "_file_" + string(ifile);
    
    fpath = get_open_filename("", "C:\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\sandor_dma_1GiB_O2.t");
    
    
    var file = file_text_open_read(fpath);
    //show_message("file = " + string(file) + " - path = " + fpath);
    
    g_save_path_default_base_path = fpath + "_default";
    
    var wlist = split_string(fpath, "\");
    g_file_name = "Fichier inconnu.";
    if (ds_list_size(wlist) != 0) {
        g_file_name = ds_list_find_value(wlist, ds_list_size(wlist) - 1);
        
        // output_png
        var char_count = string_length(fpath) - string_length(g_file_name);
        var base_path = string_copy(fpath, 1, char_count);
        output_base_path = base_path; // should be the same for every file
        //
        //show_message("g_save_path_default_base_path = " + g_save_path_default_base_path);
        ds_list_add(l_file_name, g_file_name);
    }
    ds_list_destroy(wlist);
    
    var version_str = file_text_readln(file);
    var version = real(version_str);
    
    if ( ! g_debug_fast_load ) {
        DEFAULT_COMPUTER_ID = get_integer("DEFAULT_COMPUTER_ID ? (1 T, 2 M, 3 S)", DEFAULT_COMPUTER_ID);
    }
    
    if (version == 2) load_data_v2(file);
    if (version == 3) load_data_v3(file);
    if (version == 4) load_data_v4(file);
    if (version == 5) load_data_v5(file);
    if (version == 6) load_data_v6(file);
    if (version == 7) load_data_v7(file);
    
    file_text_close(file);
    ++g_multiple_load_file_current_count;
}

// Mise à jour du nom du graphe : tous les fichiers chargés sont affichés (fichier1 x fichier2 x ...)

g_graph_display_name = "";
g_save_path_default_base_path = output_base_path + "output_png\";
for (var ifile = 0; ifile < ds_list_size(l_file_name); ++ifile) {
    var fname = ds_list_find_value(l_file_name, ifile);
    if (ifile != 0) {
        g_save_path_default_base_path += "_x_";
        g_graph_display_name += " x ";
    }
    g_save_path_default_base_path += fname;
    g_graph_display_name += fname;
}

if ( ! g_debug_fast_load ) {
    g_graph_display_name = get_string("Nom du graphe :", g_graph_display_name);
}
