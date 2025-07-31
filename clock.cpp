#include "clock.h"
//
// Created by ckjsuperhh6602 on 25-7-30.
//
// int clock::a=1;
// int clock::answer[11]={};
bool clock::execute_once() {

    // if (ticker==228) {
    //     Memory::show_mem();
    // }
    // std::cerr<<"ra="<<std::hex<<Register::regs[1]<<std::endl;
    // std::cerr<<"sp="<<std::hex<<Register::regs[2]<<std::endl;
    // std::cerr<<"s0="<<std::hex<<Register::regs[8]<<std::endl;
    // std::cerr<<"s1="<<std::hex<<Register::regs[9]<<std::endl;
    // std::cerr<<"a0="<<std::hex<<Register::regs[10]<<std::endl;
    // std::cerr<<"a1="<<std::hex<<Register::regs[11]<<std::endl;
    // std::cerr<<"a2="<<std::hex<<Register::regs[12]<<std::endl;
    // std::cerr<<"a3="<<std::hex<<Register::regs[13]<<std::endl;
    // std::cerr<<"a4="<<std::hex<<Register::regs[14]<<std::endl;
    // std::cerr<<"a5="<<std::hex<<Register::regs[15]<<std::endl;
    // std::cerr<<"s2="<<std::hex<<Register::regs[18]<<std::endl;
    // std::cerr<<"s3="<<std::hex<<Register::regs[19]<<std::endl;
    // std::cerr<<"s4="<<std::hex<<Register::regs[20]<<std::endl;
    // std::cerr<<"s5="<<std::hex<<Register::regs[21]<<std::endl;
    // std::cerr<<"s6="<<std::hex<<Register::regs[22]<<std::endl;
    // std::cerr<<"s7="<<std::hex<<Register::regs[23]<<std::endl;
    // std::cerr<<"s8="<<std::hex<<Register::regs[24]<<std::endl;
    // std::cerr<<"s9="<<std::hex<<Register::regs[25]<<std::endl;
    // std::cerr<<"pc="<<std::hex<<Register::pc<<std::endl;
    // RS::show_everything();
    // Register::show_reg();
    // if (a!=Register::regs[20]) {
    //     a=Register::regs[20];
    //     answer[a]=Memory::read4(0x135c);
    // }
    Register::set_zero();
    CDB::execute();
    ticker++;
    Ins_Cache::check();//每一个时钟都要检查一下指令缓存有没有做
    return ROB::execute_1();//很明显，只要有运行，end就会是true，那么就要继续进行下一个回合的运行
}

int clock::ticker=0;