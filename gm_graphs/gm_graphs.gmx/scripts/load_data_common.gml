/// load_data_common(fpath);

var fpath = argument0;

fpath = get_open_filename("", fpath);

var file = file_text_open_read(fpath);
//show_message("file = " + string(file) + " - path = " + fpath);

g_save_path_default_base_path = fpath;

var wlist = split_string(fpath, "\");
g_file_name = "Fichier inconnu.";
if (ds_list_size(wlist) != 0) {
    g_file_name = ds_list_find_value(wlist, ds_list_size(wlist) - 1);
    
    // output_png
    var char_count = string_length(fpath) - string_length(g_file_name);
    var base_path = string_copy(fpath, 1, char_count);
    g_save_path_default_base_path = base_path + "output_png\" + g_file_name;
    show_message("g_save_path_default_base_path = " + g_save_path_default_base_path);
    
}


ds_list_destroy(wlist);

var version_str = file_text_readln(file);
var version = real(version_str);

if (version == 2) load_data_v2(file);
if (version == 3) load_data_v3(file);
if (version == 4) load_data_v4(file);
if (version == 5) load_data_v5(file);

file_text_close(file);
