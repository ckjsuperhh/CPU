//
// Created by ckjsuperhh6602 on 25-7-22.
//

#ifndef ALUS_H
#define ALUS_H
#include <iostream>

#include "Decoder.h"
#include "Ins_Cache.h"
#include "RF.h"
#include "Rob.h"

class calculate {
public:
    static void cal(inst & ins) {
        if (ins.op == "add") {
            ins.value=ins.rs1_val+ins.rs2_val;
        }else if (ins.op == "sub") {
            ins.value=ins.rs1_val-ins.rs2_val;
        }else if (ins.op == "and") {
            ins.value=ins.rs1_val&ins.rs2_val;
        }else if (ins.op == "or") {
            ins.value=ins.rs1_val|ins.rs2_val;
        } else if (ins.op == "xor") {
            ins.value=ins.rs1_val^ins.rs2_val;
        } else if (ins.op == "sll") {
            ins.value = ins.rs1_val << ins.rs2_val;
        }
        // 逻辑右移（srl）：高位补0（无符号处理）
        else if (ins.op == "srl") {
            // 1. 移位量取rs2的低5位（RISC-V规定，32位最大移位31）
            uint32_t shift_amount = static_cast<uint32_t>(ins.rs2_val) & 0x1F;
            // 2. 无符号处理，高位补0
            uint32_t rs1_unsigned = static_cast<uint32_t>(ins.rs1_val);
            ins.value = rs1_unsigned >> shift_amount;
        }
        // 算术右移（sra）：高位补符号位（有符号处理）
        else if (ins.op == "sra") {
            // 1. 移位量同样取rs2的低5位
            uint32_t shift_amount = static_cast<uint32_t>(ins.rs2_val) & 0x1F;
            // 2. 有符号处理，高位补符号位
            int32_t rs1_signed = static_cast<int32_t>(ins.rs1_val);
            ins.value = rs1_signed >> shift_amount;
        } else if (ins.op == "slt") {
            ins.value = (ins.rs1_val < ins.rs2_val) ? 1 : 0;
        } else if (ins.op == "sltu") {
            ins.value = (static_cast<uint32_t>(ins.rs1_val) < static_cast<uint32_t>(ins.rs2_val)) ? 1 : 0;
        } else if (ins.op == "addi") {
            ins.value = ins.rs1_val+ins.imm;
        } else if (ins.op == "andi") {
            ins.value = ins.rs1_val&ins.imm;
        } else if (ins.op == "ori") {
            ins.value = ins.rs1_val|ins.imm;
        } else if (ins.op == "xori") {
            ins.value = ins.rs1_val^ins.imm;
        }
        // 逻辑右移立即数（srli）：高位补0（零扩展）
        else if (ins.op == "srli") {
            // 1. 移位量取imm的低5位（RISC-V规定，32位最大移位31）
            uint32_t shift_amount = static_cast<uint32_t>(ins.imm) & 0x1F;
            // 2. 按无符号数处理，右移后高位补0
            uint32_t rs1_unsigned = static_cast<uint32_t>(ins.rs1_val);
            ins.value = rs1_unsigned >> shift_amount;
        }
        // 算术右移立即数（srai）：高位补符号位（符号扩展）
        else if (ins.op == "srai") {
            // 1. 移位量同样取imm的低5位
            uint32_t shift_amount = static_cast<uint32_t>(ins.imm) & 0x1F;
            // 2. 按有符号数处理，右移后高位补符号位
            int32_t rs1_signed = static_cast<int32_t>(ins.rs1_val);
            ins.value = rs1_signed >> shift_amount;
        }
        else if (ins.op == "slli") {
            ins.value = ins.rs1_val<<ins.imm;
        }
        // 有符号比较：slti（Set Less Than Immediate）
        // 规则：将rs1和imm都视为有符号数，若rs1 < imm则结果为1，否则为0
        else if (ins.op == "slti") {
            // 显式转换为有符号整数，按有符号规则比较（考虑符号位）
            int32_t rs1_signed = ins.rs1_val;
            int32_t imm_signed = ins.imm;  // imm是12位符号扩展后的有符号数
            ins.value = (rs1_signed < imm_signed) ? 1 : 0;
        }
        // 无符号比较：sltiu（Set Less Than Immediate Unsigned）
        // 规则：将rs1和imm都视为无符号数，若rs1 < imm则结果为1，否则为0
        else if (ins.op == "sltiu") {
            // 显式转换为无符号整数，按无符号规则比较（不考虑符号位，所有位都为数值）
            uint32_t rs1_unsigned = static_cast<uint32_t>(ins.rs1_val);
            uint32_t imm_unsigned = static_cast<uint32_t>(ins.imm);  // imm按无符号解析
            ins.value = (rs1_unsigned < imm_unsigned) ? 1 : 0;
        } else if (ins.op == "slti") {
            ins.value = (ins.rs1_val < ins.imm) ? 1 : 0;
        } else if (ins.op == "sltiu") {
            ins.value = (static_cast<uint32_t>(ins.rs1_val) < static_cast<uint32_t>(ins.imm)) ? 1 : 0;
        } else if (ins.op == "lb") {//往下许多是用来计算地址
            ins.value = ins.rs1_val+ins.imm;
        } else if (ins.op == "lbu") {
            ins.value = ins.rs1_val+ins.imm;
        } else if (ins.op == "lh") {
            ins.value = ins.rs1_val+ins.imm;
        } else if (ins.op == "lhu") {
            ins.value = ins.rs1_val+ins.imm;
        } else if (ins.op == "lw") {
            ins.value = ins.rs1_val+ins.imm;
        } else if (ins.op == "sb") {//这三个是计算存储地址
            ins.value = ins.rs1_val+ins.imm;
        } else if (ins.op == "sh") {
            ins.value = ins.rs1_val+ins.imm;
        } else if (ins.op == "sw") {
            ins.value = ins.rs1_val+ins.imm;
        } else if (ins.op == "beq") {//对于有条件的跳转，我就等到跳转完成之后再清空
            if (ins.rs1_val==ins.rs2_val) {
                ins.value=ins.pc+ins.imm;
                Register::pc=ins.value;
                Ins_Cache::clear(Register::pc);
            }
            Reg_status::Busy_pc=false;
        } else if (ins.op == "bge") {
            if (ins.rs1_val>=ins.rs2_val) {
                ins.value=ins.pc+ins.imm;
                Register::pc=ins.value;
                Ins_Cache::clear(Register::pc);
            }
            Reg_status::Busy_pc=false;
        } else if (ins.op == "bgeu") {
            if (static_cast<uint32_t>(ins.rs1_val)>=static_cast<uint32_t>(ins.rs2_val)) {
                ins.value=ins.pc+ins.imm;
                Register::pc=ins.value;
                Ins_Cache::clear(Register::pc);
            }
            Reg_status::Busy_pc=false;
        } else if (ins.op == "blt") {
            if (ins.rs1_val<ins.rs2_val) {
                ins.value=ins.pc+ins.imm;
                Register::pc=ins.value;
                Ins_Cache::clear(Register::pc);
            }
            Reg_status::Busy_pc=false;
        } else if (ins.op == "bltu") {
            if (static_cast<uint32_t>(ins.rs1_val)<static_cast<uint32_t>(ins.rs2_val)) {
                ins.value=ins.pc+ins.imm;
                Register::pc=ins.value;
                Ins_Cache::clear(Register::pc);
            }
            Reg_status::Busy_pc=false;
        } else if (ins.op == "bne") {
            if (ins.rs1_val!=ins.rs2_val) {
                ins.value=ins.pc+ins.imm;
                Register::pc=ins.value;
                Ins_Cache::clear(Register::pc);//clear顺带已经修改过pc的值了(不然内部的值已经维护好了，就是pc读到的位置+4)
            }
            Reg_status::Busy_pc=false;
        } else if (ins.op == "jal") {
            ins.value=ins.pc+4;
            Register::pc=ins.pc+ins.imm;
            Reg_status::Busy_pc=false;
            Ins_Cache::modify(Register::pc);
        } else if (ins.op == "jalr") {
            ins.value=ins.pc+4;
            Register::pc=ins.rs1_val+ins.imm;
            Reg_status::Busy_pc=false;
            Ins_Cache::modify(Register::pc);//把Ins_Cache的指针修改成对应的跳转值
        } else if (ins.op == "auipc") {
            ins.imm<<=12;
            ins.value=ins.imm+ins.pc;
        } else if (ins.op == "lui") {
            ins.imm<<=12;
            ins.value=ins.imm;
        } else if (ins.op == "ebreak") {
            std::cout<<"Asking the debugger to do something"<<std::endl;
        } else if (ins.op == "ecall") {
            std::cout<<"Asking the OS to do something"<<std::endl;
        } else {
            std::cerr<<"unknown instruction"<<std::endl;
        }
    }
};


#endif //ALUS_H
