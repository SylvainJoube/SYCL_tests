/// load_draw_save_graph(graph object);

// Chargement des données, dessin du graphe et sauvegarde en png

// Un exemple de script complet :

//g_multiple_load_file_current_count = 1;
// g_multiple_load_LM is declared in init()

var graph = argument0;

if ( ! graph.valid ) {
    error_add("Graphe invalide, non dessiné.", 10);
    return -1;
}

// == Load phase ==

/*var nbfiles = 1;
if ( g_multiple_load_LM && ( ! g_debug_fast_load ) ) {
    nbfiles = get_integer("Nombre de fichiers à lire :", nbfiles);
}

var l_file_name = ds_list_create();
var output_base_path = "";
g_multiple_load_file_number = nbfiles;*/

load_data_common_init();

// Load files

var nbfiles = ds_list_size(graph.files_list);

for (var ifile = 0; ifile < nbfiles; ++ifile) {
    var ofile = ds_list_find_value(graph.files_list, ifile);
    var fpath = ofile.path;
    g_file_name = ofile.curve_name; // nom de la courbe dessinée
    g_multiple_load_file_current_count = ifile;
    
    if ( ! dFileExists(fpath) ) {
        error_add("Fichier d'entrée introuvable : " + fpath, 0);
        return 2;
    }
    
    fpath = get_open_filename("", fpath);
    var file = file_text_open_read(fpath);
    //show_message("file = " + string(file) + " - path = " + fpath);
    
    /*g_save_path_default_base_path = fpath + "_default";
    
    var wlist = split_string(fpath, "\");
    g_file_name = "Fichier inconnu.";
    if (ds_list_size(wlist) != 0) {
        g_file_name = ds_list_find_value(wlist, ds_list_size(wlist) - 1);
        
        // output_png
        var char_count = string_length(fpath) - string_length(g_file_name);
        var base_path = string_copy(fpath, 1, char_count);
        output_base_path = base_path; // should be the same for every file
        
        //show_message("g_save_path_default_base_path = " + g_save_path_default_base_path);
        ds_list_add(l_file_name, g_file_name);
    }
    ds_list_destroy(wlist);*/
    
    var version_str = file_text_readln(file);
    var version = real(version_str);
    
    DEFAULT_COMPUTER_ID = ofile.computer_id;
    DEFAULT_MEMORY_BANDWIDTH = COMPUTER_GPU_BANDWIDTH[DEFAULT_COMPUTER_ID];// GPU ou PCIe 3.0 en fonction de la localisation de la mémoire
    
    //show_message("load_draw_save_graph - dataversion = " + string(version));
    
    load_data_execute_right_version(version, file);
    
    file_text_close(file);
    //++g_multiple_load_file_current_count;
}

g_graph_display_name = graph.display_name;
// g_save_path_default_base_path ??
// output_base_path ???

// == Draw phase ==
var graph_height = g_graph_height;
var graph_width = g_graph_width;

// J'ai un un problème avec lenombre de vertex dessinés par GM à une step donnée
// je l'ai contourné en faisant un draw étendu sur plusieurs steps,
// probablement que GM gère mal le dessin de beaucoup de vertex à une même "step".
var must_recreate = false;
if ( (g_graph_surface == -1) || ( ! surface_exists(g_graph_surface) ) ) {
    must_recreate = true;
    //show_message("Surf = " + string(g_graph_surface) + "  exists = " + string(surface_exists(g_graph_surface)));
} else {
    if (
    surface_get_height(g_graph_surface) != g_surface_height
    || surface_get_width(g_graph_surface) != g_surface_width) {
        must_recreate = true;
        //show_message("Surf = " + string(g_graph_surface) + "  bad size. Must recreate.");
        surface_free(g_graph_surface);
    }
}
if (must_recreate) {
    //show_message("recreate surf (load_draw_save_graph)");
    g_graph_surface = surface_create(g_surface_width, g_surface_height);
} else {
    //show_message("reuse surf (load_draw_save_graph)");
}

surface_set_target(g_graph_surface);
draw_clear(c_white);
script_execute(graph.use_script); // no argument //draw_some_graph_common();
surface_reset_target();

if ( dFileExists(graph.output_path) ) {
    error_add("Fichier de sortie déjà présent : " + graph.output_path, 0);
    return 2;
} else {
    //get_string("Ok, fichier non présent : ", graph.output_path);
}

//get_string("Will save to : ", graph.output_path);
surface_save(g_graph_surface, get_save_filename("", graph.output_path));
//surface_free(g_graph_surface);
