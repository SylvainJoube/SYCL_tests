/// ignore_alloc_time_to_name_prefix(ignore_alloc_time) : string;

var ignore_alloc_time = argument0;

switch (ignore_alloc_time) {
case 1 : return "";
case 0 : return "-al";
}

return "alloc inconnu";
