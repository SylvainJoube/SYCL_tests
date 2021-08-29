/// batch_add_file( graph, path, curve_name, computer_id );


var graph = argument0;
var path = argument1;
var curve_name = argument2; // nom de la courbe associée
var computer_id = argument3; 

if ( ! graph.valid ) {
    error_add("Graphe invalide, fichier non ajouté. (" + path + ")", 10);
    return -1;
}

var file = instance_create(0, 0, o_input_file);
ds_list_add(graph.files_list, file);
file.path = path;
file.curve_name = curve_name; // nom de la courbe associée
file.computer_id = computer_id; // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor

++g_multiple_load_file_number;

return file;
