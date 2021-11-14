/// draw_graph(points_list, x_space_left, y_space_left, xlabel, ylabel, ymin, ymax);

var graph_list = ds_list_create();

var gp = instance_create(0, 0, graph_points);
ds_list_add(graph_list, gp);
gp.color = c_black;
gp.points = argument0;

draw_graph_objs(graph_list, argument1, argument2, argument3, argument4, argument5, argument6, argument7, argument8);
