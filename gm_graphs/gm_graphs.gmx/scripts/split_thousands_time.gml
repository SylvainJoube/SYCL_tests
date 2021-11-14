/// split_thousands(real) : string

var a = argument0;
if (g_split_thousands_USE_MS) {
    a = round(a / 1000) + 1; // 0 does not look nice on a graph
}

var s = string(a);
var len = string_length(s);
var len_left = len;
var result = "";

var rep = floor(len / 3);
var remains_before = len - rep * 3;
if (remains_before != 0)
    result = string_copy(s, 1, remains_before);


for (var i = 0; i < rep; ++i) {
    if (result != "") result += " ";
    var start_index = remains_before + 1 + i * 3;
    result += string_copy(s, start_index, 3);
    //show_message("for " + s + " copy from " + string(start_index) + " (count 3)" + " (rep " + string(rep) + " rem " + string(remains_before) + ")");
}

return result;

//return "(rep " + string(rep) + " rem " + string(remains_before) + ") -" + result + "-";
