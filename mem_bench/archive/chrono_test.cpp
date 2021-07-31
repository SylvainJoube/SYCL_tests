#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

// SyCL specific includes
#include <CL/sycl.hpp>
#include <array>
#include <sys/time.h>
#include <stdlib.h>

class stime_utils {
private :
    std::chrono::_V2::steady_clock::time_point _start, _stop;

public :
    
    void start() {
        _start = std::chrono::steady_clock::now();
    }

    // Gets the us count since last start or reset.
    uint64_t reset() {
        std::chrono::duration<int64_t, std::nano> dur = std::chrono::steady_clock::now() - _start;
        _start = std::chrono::steady_clock::now();
        int64_t ns = dur.count();
        int64_t us = ns / 1000;
        return us;
    }

};

/*std::unique_ptr<stime_utils> start_chrono() {
    auto s = std::unique_ptr<stime_utils>(); //new stime_utils;
    s->start = std::chrono::steady_clock::now();
    return s;
}


uint64_t stop_chrono(std::unique_ptr<stime_utils> s) {
    std::chrono::duration<int64_t, std::nano> result = std::chrono::steady_clock::now() - s->start;
    int64_t ns = result.count();
    int64_t us = ns / 1000;
    return us;
}

uint64_t restart_chrono(std::unique_ptr<stime_utils> s) {
    std::chrono::duration<int64_t, std::nano> result = std::chrono::steady_clock::now() - s->start;
    int64_t ns = result.count();
    int64_t us = ns / 1000;
    return us;
}*/


uint64_t get_ms() {
    auto tm = std::chrono::steady_clock::now();
    std::chrono::duration<double> s = tm - tm;


    struct timeval tp;
    gettimeofday(&tp, NULL);
    uint64_t ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    return ms;
}

void log(std::string str) {
    std::cout << str << std::endl;
}
void logs(std::string str) {
    std::cout << str;
}

int main(int argc, char *argv[])
{
    uint64_t t_start;
    stime_utils chrono;

    t_start = get_ms();
    chrono.start();

    uint64_t tm_time, tm_chrono, us;

    int count = 20;

    for (int i = 0; i < count; ++i) {

        std::this_thread::sleep_for (std::chrono::milliseconds(i * 10 + 2));
        us = chrono.reset();
        tm_chrono = us / 1000;
        tm_time = get_ms() - t_start;
        t_start = get_ms();
        if (tm_time == tm_chrono) logs("OK - " + std::to_string(tm_chrono));
        else                      logs("ERROR - chrono " + std::to_string(tm_chrono) + " != " + std::to_string(tm_time) + " time");

        log("  us " + std::to_string(us));
    }
    


    return 0;

}