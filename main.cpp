#include <iostream>

#include "clock.h"
#include  "Decoder.h"
#include "Memory.h"
#include "Rob.h"
#pragma GCC optimize("O3")
std::map<__uint32_t, __uint8_t> Memory::mem;



int main() {

    //     int clk_max;
    // std::cin>>clk_max;
Memory::store_ins();
    // Memory::show_mem();
    // while (clock::execute_once()&&clock::ticker<clk_max) {
        // if (!clock::ticker%100000) {
            // std::cerr<<std::dec<<"clk:"<<clock::ticker<<std::endl;
        // }


    while (clock::execute_once()) {
std::cerr<<std::dec<<"clk:"<<clock::ticker<<std::endl;
    }

    return 0;
}

