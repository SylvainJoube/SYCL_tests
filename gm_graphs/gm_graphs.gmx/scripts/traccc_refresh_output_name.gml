/// traccc_refresh_output_name();


var bench_test_nb = "A";
var debug_run_prefix = "";
var debug_verid = "g";

//var citer = 1; // iteration count

//for (var current_run = 1; current_run <= run_number; ++current_run) {

fname_prefix_output_short = "" + bench_test_nb + "_";// + bench_version + "_";
//var fname_prefix_output = "b" + bench_test_nb + "_" + bench_version + "_";
//var fname_prefix_input  = bench_version + "_";

//var fname_suffix_common = "_" + computer_name + "_" + size_str + "_O2_RUN" + string(current_run);
fname_suffix_common_traccc = "_" + computer_name + "_ld" + string(traccc_repeat_load_count) + "_RUN" + string(current_run);

//var fname_suffix_output = fname_suffix_common + "_q1.5" + ".png";
//var fname_suffix_input  = fname_suffix_common + ".t";

fname_suffix_output_traccc = fname_suffix_common_traccc + "_q1.5" + ".png";
fname_suffix_input_traccc  = fname_suffix_common_traccc + ".t";

if (traccc_hide_host) {
    fname_suffix_output_traccc = fname_suffix_common_traccc + "_q1.5_hideHost.png";
} else {
    fname_suffix_output_traccc = fname_suffix_common_traccc + "_q1.5.png";
}

//var file_name_const_part = common_file_name + "_RUN" + string(current_run);// + ".t";
//var local_common_path = common_path + bench_version;
//var file_name_const_part_ouptut_png = file_name_const_part + "_" + bench_test_nb + ".png";


//show_message("run_batch_job_traccc current_test = " + string(current_test)
//+ chr(10) + "step = " + string(step) + " traccc_total_iterations = " + string(traccc_total_iterations));

// in out file name suffix
benchmark_version_traccc = "acts06_";
bvt = benchmark_version_traccc;
fname_prefix_output_short += benchmark_version_traccc;
