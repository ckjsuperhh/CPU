//
// Created by ckjsuperhh6602 on 25-7-22.
//
#ifndef RF_H
#define RF_H

#include <string>
#include <cstdint>      // 推荐：标准的<uint32_t>头，而非<bits/types.h>

#include "CDB.h"
#include "Decoder.h"
#include "RS.h"

// 前置声明，避免循环依赖
struct inst;
class instructions;
enum class Posi;

class Register {
public:
    static uint32_t regs[32];
    static uint32_t Rename_regs[32];
    static std::string reg_num[32];
    Register();
    static void initial_set();

    static void set_zero();

    static std::string get_name(int reg);
    static uint32_t read(int reg);
    static void write(int reg, uint32_t num);
    static int read_pc();

    static void show_reg();

    static int pc;
};

class Reg_status {
public:
    static bool Busy[32];
    static bool Busy_pc;
    static int Reorder[32];
    static bool get_busy(int idx);   // 建议把接口声明清楚
};

class Read_regs {
public:
    static void execute(inst &ins, int &Vj, int &Vk, int &Qj, int &Qk, Posi &pj, Posi &pk);
};

class Write_regs {
public:
    static void execute(int i, int Reg, int value);

    static void mark_Reg(int Reg, int i);
};

#endif //RF_H