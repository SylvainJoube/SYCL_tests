/// run_batch_msi_1();

// Dessin des graphes et sauvegardes en png, pour plusieurs graphiques.

// DMA

// == Graph info ==
/*var graph = instance_create(0, 0, o_graph);
g_graph_object = graph;
graph.files_list = ds_list_create();
graph.output_path = "C:\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\bench8_msi_dma_1GiB_O2.png";
graph.use_script = draw_some_graph_13dbg;
graph.display_name = "bench_msi_dma_1GiB_O2 !new!";*/

var graph = batch_add_graph_v1(
/*output_path*/  "C:\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\bench9_msi_dma_1GiB_O2.png",
/*use_script*/   draw_some_graph_13dbg,
/*display_name*/ "bench_msi_dma_1GiB_O2 !new!"
);

batch_add_file_v1(
/*graph*/       graph,
/*path*/        "C:\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\msi_dma_1GiB_O2.t",
/*curve_name*/  "aucun nom", // nom de la courbe associée
/*computer_id*/ 3 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
);

// == Input files ==
/*var file = instance_create(0, 0, o_input_file);
ds_list_add(graph.files_list, file);
file.path = "C:\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\msi_dma_1GiB_O2.t";
file.curve_name = "aucun nom"; // nom de la courbe associée
file.computer_id = 3; // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
*/
g_multiple_xaxis = true;
load_draw_save_graph(graph);


// L & M

var graph = batch_add_graph_v1(
/*output_path*/  "C:\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\bench9_msi_L_M_1GiB_O2.png",
/*use_script*/   draw_some_graph_11,
/*display_name*/ "msi_L_M_1GiB_O2 !new!"
);

batch_add_file_v1(
/*graph*/       graph,
/*path*/        "C:\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\msi_L_M_1GiB_O2.t",
/*curve_name*/  "aucun nom", // nom de la courbe associée
/*computer_id*/ 3 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
);

// == Graph info ==
/*var graph = instance_create(0, 0, o_graph);
g_graph_object = graph;
graph.files_list = ds_list_create();
graph.output_path = "C:\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\bench8_msi_L_M_1GiB_O2.png";
graph.use_script = draw_some_graph_11;
graph.display_name = "msi_L_M_1GiB_O2 !new!";

// == Input files ==
var file = instance_create(0, 0, o_input_file);
ds_list_add(graph.files_list, file);
file.path = "C:\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\msi_L_M_1GiB_O2.t";
file.curve_name = "aucun nom"; // nom de la courbe associée
file.computer_id = 3; // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
*/
g_multiple_xaxis = false;
load_draw_save_graph(graph);


// Comparaison L & M sur MSI avec et sans SIMD

