/// load_data_common(fpath);

var fpath = argument0;

fpath = get_open_filename("", fpath);

var file = file_text_open_read(fpath);
//show_message("file = " + string(file) + " - path = " + fpath);

var version_str = file_text_readln(file);
var version = real(version_str);

if (version == 2) load_data_v2(file);

file_text_close(file);
