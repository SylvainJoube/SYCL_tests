/// mem_location_to_str_prefix(mode : int) : string

var mode = argument0;

switch (mode) {
case 0 : return "s";
case 1 : return "d";
case 2 : return "h";
case 3 : return "b";
default : return "ukn";
}

/*switch (mode) {
case 0 : return "s";
case 1 : return "d";
case 2 : return "h";
case 3 : return "b";
default : return "ukn";
}*/
