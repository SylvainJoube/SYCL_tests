/// run_batch_msi_1();

// Dessin des graphes et sauvegardes en png, pour plusieurs graphiques.

// DMA

//Graph info
var graph = batch_add_graph(
/*output_path*/  "C:\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\bench9_msi_dma_1GiB_O2.png",
/*use_script*/   draw_some_graph_13dbg,
/*display_name*/ "bench_msi_dma_1GiB_O2 !new!"
);

//Input files
batch_add_file(
/*graph*/       graph,
/*path*/        "C:\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\msi_dma_1GiB_O2.t",
/*curve_name*/  "aucun nom", // nom de la courbe associée
/*computer_id*/ 3 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
);

g_multiple_xaxis = true;
load_draw_save_graph(graph);


// L & M

var graph = batch_add_graph(
/*output_path*/  "C:\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\bench9_msi_L_M_1GiB_O2.png",
/*use_script*/   draw_some_graph_11,
/*display_name*/ "msi_L_M_1GiB_O2 !new!"
);

batch_add_file(
/*graph*/       graph,
/*path*/        "C:\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\msi_L_M_1GiB_O2.t",
/*curve_name*/  "aucun nom", // nom de la courbe associée
/*computer_id*/ 3 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
);

g_multiple_xaxis = false;
load_draw_save_graph(graph);


// Comparaison L & M sur MSI avec et sans SIMD
var graph = batch_add_graph(
/*output_path*/  "C:\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\bench16_msi_L_M_1GiB_O2_vs_SIMD.png",
/*use_script*/   draw_some_graph_11,
/*display_name*/ "Comparaison MSI for classique vs optim vs Sandor optim"
);

batch_add_file(
/*graph*/       graph,
/*path*/        "C:\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\msi_L_M_1GiB_O2.t",
/*curve_name*/  "MSI Nvidia classique", // nom de la courbe associée
/*computer_id*/ 3 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
);

batch_add_file(
/*graph*/       graph,
/*path*/        "C:\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\msi_L_M_512MiB_O2_SIMD_2.t",
/*curve_name*/  "MSI Nvidia optim", // nom de la courbe associée
/*computer_id*/ 3 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
);

batch_add_file(
/*graph*/       graph,
/*path*/        "C:\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\sandor_L_M_512MiB_O2_SIMD_2.t",
/*curve_name*/  "Sandor optim", // nom de la courbe associée
/*computer_id*/ 4 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
);

g_multiple_xaxis = false;
load_draw_save_graph(graph);


