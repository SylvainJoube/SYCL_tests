/// mem_location_to_str(mode : int) : string

var mode = argument0;

switch (mode) {
case 0 : return "shared";
case 1 : return "device";
case 2 : return "host";
case 3 : return "buffers";
case 20 : return "glibc";
default : return "unknown mem strategy";
}
