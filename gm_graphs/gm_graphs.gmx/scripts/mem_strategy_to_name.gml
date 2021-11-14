/// mem_strategy_to_name(mem strategy id) : string;

var smem = argument0;

switch (smem) {
case 1 : return "ptr graph";
case 2 : return "flat";
}

return "stratégie mémoire non connue";
