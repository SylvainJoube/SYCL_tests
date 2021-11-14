/// ignore_alloc_time_to_name(ignore_alloc_time) : string;

var ignore_alloc_time = argument0;

switch (ignore_alloc_time) {
case 1 : return "sans alloc";
case 0 : return "avec alloc";
}

return "alloc inconnu";
