/// mem_strategy_to_name(mem strategy id) : string;

var smem = argument0;

switch (smem) {
case 1 : return "g";
case 2 : return "a";
}

return "-nc-";
