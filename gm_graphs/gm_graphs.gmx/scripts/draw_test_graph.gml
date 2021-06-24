/// draw_test_graph();

// Input points
var list = ds_list_create();
ds_list_add(list, 0, 0);
ds_list_add(list, 20, 40);
ds_list_add(list, 10, 20);
ds_list_add(list, 10, 40);
ds_list_add(list, 10, 30);
ds_list_add(list, 10, 26);

var xlabel = "Itération n°";
var ylabel = "Temps pris parallel_for (ms)";

draw_graph(list, 20, 20, xlabel, ylabel, -1, -1);


