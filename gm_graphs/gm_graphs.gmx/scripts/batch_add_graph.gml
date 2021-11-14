/// batch_add_graph( output_base_path, output_file_name, use_script, display_name );

var output_base_path = argument0;
var output_file_name = argument1;
var use_script = argument2;
var display_name = argument3;

var graph = instance_create(0, 0, o_graph);
graph.valid = true;
/* ! global variable ->*/ g_graph_object = graph;
graph.files_list = ds_list_create();
graph.output_path = output_base_path + output_file_name;
graph.use_script = use_script;
graph.display_name = display_name;

if ( dFileExists(graph.output_path) ) {
    error_add("Fichier de sortie déjà présent : " + graph.output_path, 0);
    graph.valid = false;
}

g_multiple_load_file_number = 0;

return graph;
