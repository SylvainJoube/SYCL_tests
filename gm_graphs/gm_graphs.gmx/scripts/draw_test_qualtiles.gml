/// draw_test_qualtiles();

// sort and delete strange values
var ysort = ds_list_create();

//ds_list_add(ysort, 48, 72, 111, 1840);

ds_list_add(ysort, 1, 2, 3, 4);

/*
ds_list_sort(ysort, true);

var ptlen = ds_list_size(ysort);

var med;
var q3_start_index;
if (ptlen % 2 == 0) {
    var low = ds_list_find_value(ysort, floor(ptlen / 2) - 1);
    var high = ds_list_find_value(ysort, floor(ptlen / 2));
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
}*/

var quartiles = compute_quartiles(ysort);
var q1 = lfind(quartiles, 0);
var q2 = lfind(quartiles, 1);
var q3 = lfind(quartiles, 2);

show_message("q1(" + string(q1) + ") q2(" + string(q2) + ") q3(" + string(q3) + ")");