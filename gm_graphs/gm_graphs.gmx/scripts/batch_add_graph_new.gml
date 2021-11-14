/// batch_add_graph( output_path, use_script, display_name );

var output_path = argument0;
var use_script = argument1;
var display_name = argument2;

var graph = instance_create(0, 0, o_graph);
graph.valid = true;
/* ! global variable ->*/ g_graph_object = graph;
graph.files_list = ds_list_create();
graph.output_path = output_path;
graph.use_script = use_script;
graph.display_name = display_name;

if ( dFileExists(graph.output_path) ) {
    error_add("Fichier de sortie déjà présent : " + graph.output_path);
    graph.valid = false;
}

g_multiple_load_file_number = 0;

return graph;