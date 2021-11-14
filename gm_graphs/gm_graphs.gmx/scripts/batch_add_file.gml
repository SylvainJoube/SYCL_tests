/// batch_add_file( graph, base_path, file_name, curve_name, computer_id );


var graph = argument0;
var base_path = argument1;
var file_name = argument2;
var curve_name = argument3; // nom de la courbe associée
var computer_id = argument4; 

if ( ! graph.valid ) {
    error_add("Graphe invalide, fichier non ajouté. (" + base_path + file_name + ")", 10);
    return -1;
}

var file = instance_create(0, 0, o_input_file);
ds_list_add(graph.files_list, file);
file.path = base_path + file_name;
file.curve_name = curve_name; // nom de la courbe associée
file.computer_id = computer_id; // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor

++g_multiple_load_file_number;

return file;
