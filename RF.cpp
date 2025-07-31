//
// Created by ckjsuperhh6602 on 25-7-24.
//
#include "RF.h"
#include "Rob.h"
#include "RS.h"

// ==== Register 静态成员初始化 ====
uint32_t Register::regs[32] = {0};
uint32_t Register::Rename_regs[32] = {0};
std::string Register::reg_num[32] = {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};
int Register::pc=0;

// Register成员函数实现
Register::Register() { initial_set(); }

void Register::initial_set() {
    for (auto &reg : regs) reg = 0;
}

void Register::set_zero() {
    regs[0]=0;
}
std::string Register::get_name(int reg) {
    if (reg >= 32) return "uk";
    return reg_num[reg];
}
uint32_t Register::read(const int reg) {
    return regs[reg];
}
void Register::write(const int reg, const uint32_t num) {
    regs[reg] = num;
}
bool Reg_status::Busy_pc=false;
bool Reg_status::Busy[32] = {false};
int Reg_status::Reorder[32] = {
    -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1
};
bool Reg_status::get_busy(const int idx) {
    if (idx < 0 || idx >= 32) return false;
    return Busy[idx];
}


void Read_regs::execute(inst& ins,int & Vj,int&Vk,int & Qj,int & Qk,Posi &pj,Posi &pk) {//不需要顺带读pc了,只需要读需要的部分
    if (double_reg.contains(ins.op)) {
        if (Reg_status::Busy[ins.rs1]) {
            Qj=Reg_status::Reorder[ins.rs1];
            Vj=-1;
        }else {//Vj
            Vj = Register::read(ins.rs1);
            ins.rs1_val=Vj;
            Qj=-1;
        }
        pj=Posi::rs1;
        if (Reg_status::Busy[ins.rs2]) {
            Qk=Reg_status::Reorder[ins.rs2];
            Vk=-1;
        }else {//Vk
            Vk = Register::read(ins.rs2);
            ins.rs2_val=Vk;
            Qk=-1;
        }
        pk= Posi::rs2;
        return;
    }
    if (first_reg.contains(ins.op)) {
        if (Reg_status::Busy[ins.rs1]) {
            Qj=Reg_status::Reorder[ins.rs1];
            Vj=-1;
        }else {//Vj
            Vj = Register::read(ins.rs1);
            ins.rs1_val=Vj;
            Qj=-1;
        }
        pj= Posi::rs1;
        Vk=ins.imm;
        pk=Posi::imm;
        Qk=-1;
        return;
    }
    if (only_pc.contains(ins.op)) {
        pj=Posi::none;
        pk=Posi::none;
        Qk=Qj=-1;
        Vk=ins.imm;
    }
    //"embreak" and "ecall"
}

void Write_regs::execute(const int i, const int Reg, const int value) {//对应ROB中第几条指令，修改哪个寄存器变成多少(因为有可能写后写这个寄存器已经不需要在被写进去了)
        if (Reg_status::Reorder[Reg]==i) {//如果对应寄存器确实仍然要写入这个数值的话
            Reg_status::Busy[Reg]=false;//不忙了
            Reg_status::Reorder[Reg]=-1;
            Register::write(Reg,value);//修改寄存器的值
            std::cerr<<"Writing register "<<Register::get_name(Reg)<<" with "<<std::hex<<value<<std::endl;
        }
};
int Register::read_pc() {//如果失败就返回-1,不然就是正常值
        if (Reg_status::Busy_pc) {
            return -1;
        }
        return pc;
    }
void Write_regs::mark_Reg(int Reg,int i) {
    Reg_status::Reorder[Reg]=i;
    Reg_status::Busy[Reg]=true;
}

void Register::show_reg() {
    std::cerr<<"        ";
    for (int i=0;i<32;i++) {
        std::cerr<<Register::get_name(i)<<" ";
    }
    std::cerr<<std::endl<<"Reorder:";
    for (int i=0;i<32;i++) {
        std::cerr<<Reg_status::get_busy(i)<<"  ";
    }
    std::cerr<<std::endl;
}