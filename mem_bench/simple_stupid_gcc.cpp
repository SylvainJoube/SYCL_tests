#include <iostream>
#include <array>
#include <sys/time.h>

#define ITEMS_COUNT 100000000
#define REPEAT_LOOP 10

uint64_t get_ms() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    uint64_t ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    return ms;
}

void log(std::string str) {
    std::cout << str << std::endl;
}
void logs(std::string str) {
    std::cout << str << std::flush;
}


void main_sequence() {

    float *arr = new float[ITEMS_COUNT];

    for (int i = 0; i < ITEMS_COUNT; ++i) {
        arr[i] = i % 10;
    }

    for (int it = 0; it < REPEAT_LOOP; ++it) {
        logs("iteration " + std::to_string(it + 1) + "/" + std::to_string(REPEAT_LOOP) + " (sum ");
        float sum = 0;
        for (int i = 0; i < ITEMS_COUNT; ++i) {
            sum += arr[i];
        }
        logs(std::to_string(int(sum)) + ") - ");
    }
    log("");

    delete[] arr;

    log("Bye.");
}


int main(int argc, char *argv[])
{
    // Not sure I'll need that
    /*if (argc < 2){
        std::cout << "Not enough arguments, minimum requirement: " << std::endl;
        std::cout << "./exe <data_path>" << std::endl;
        return -1;
    }
    auto data_path = std::string(argv[1]);*/


    std::cout << "Simple test program. Is vtune working ? Ver B." << std::endl;

    main_sequence();

    return 0;

}