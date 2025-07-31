#ifndef ROB_H
#define ROB_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Decoder.h"
#include "Memory.h"
#include "RF.h"

// 声明外部变量，不在头文件定义
extern std::unordered_set<std::string> read_mem ;
extern std::unordered_set<std::string> write_mem ;
extern std::unordered_set<std::string> double_reg;
extern std::unordered_set<std::string> first_reg ;
extern std::unordered_set<std::string> only_pc;
extern std::unordered_set<std::string> load;
extern std::unordered_set<std::string> add;
extern std::unordered_set<std::string> jump;

enum State {
    None,Waiting, Decoded, Issue, Exec, Write, Commit
};

struct inst {
    int i{};
    __uint32_t ins{};
    std::string op{};
    int pc{-1}, rd{}, rs1{}, rs2{},rs1_val{},rs2_val{}, imm{};
    State st{};
    int value{};
    inst();
    explicit inst(const instructions& a);
};

class ROB {
public:
    static int head;
    static int tail;
    static inst ROB_Table[500];
    static int code[500];
    static int MOD;

    static bool execute_5();
    static bool execute_1();
    static std::unordered_map<int,ticker_mem> read_mem;
};

#endif // ROB_H