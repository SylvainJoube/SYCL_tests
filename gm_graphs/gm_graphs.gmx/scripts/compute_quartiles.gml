/// compute_quartiles(ysort : list of -unsorted- values) : q1, q2, q3 as a list. Destroys ysort.

ysort = argument0;

ds_list_sort(ysort, true);

var ptlen = ds_list_size(ysort);

//show_message("ptlen = " + string(ptlen));
 
var med;
var q3_start_index;
if (ptlen % 2 == 0) {
    var low = ds_list_find_value(ysort, floor(ptlen / 2) - 1);
    var high = ds_list_find_value(ysort, floor(ptlen / 2));
    //show_message("high = " + string(high) + "  low = " + string(low));
    med = (low + high) / 2;
    q3_start_index = ptlen / 2; // floor not needed
} else {
    med = ds_list_find_value(ysort, floor(ptlen / 2));
    q3_start_index = floor(ptlen / 2) + 1;
}

var q1, q2, q3;
var qlen = floor(ptlen / 2);

q2 = med;

// Q1
if (qlen % 2 == 0) {
    var low = ds_list_find_value(ysort, floor(qlen / 2) - 1);
    var high = ds_list_find_value(ysort, floor(qlen / 2));
    q1 = (low + high) / 2;
} else {
    q1 = ds_list_find_value(ysort, floor(qlen / 2));
}

// Q3

if (qlen % 2 == 0) {
    var low = ds_list_find_value(ysort, floor(qlen / 2) - 1 + q3_start_index);
    var high = ds_list_find_value(ysort, floor(qlen / 2) + q3_start_index);
    q3 = (low + high) / 2;
} else {
    q3 = ds_list_find_value(ysort, floor(qlen / 2) + q3_start_index);
}

var result = ds_list_create();
ds_list_add(result, q1, q2, q3);
ds_list_destroy(ysort);
return result;
