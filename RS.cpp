#include "RS.h"

#include "Rob.h"
//
// Created by ckjsuperhh6602 on 25-7-24.
//
bool RS::busy_a[2] = {};
bool RS::busy_l[2] = {};
bool RS::busy_j[2] = {};//0-空闲,1-忙碌
std::string RS::op[6];
int RS::Vj[6] = {};
int RS::Vk[6] = {};
int RS::Qj[6] = {};
int RS::Qk[6] = {};
int RS::A[6] = {};
Posi RS::pk[6] = {};
Posi RS::pj[6] = {};  //这两个数组是为了在广播的时候，修改到ROB内部我知道应该修改rs1还是rs2而不需要再做讨论
int RS::Dest[6] = {};//0,1-load;2,3-add;4,5-jump
//不需要返回值，因为通过ins被修改的状态就可以窥见是否成功launch了
int RS::launch(inst &ins, const int i) {
    //在RS里边写读取寄存器的逻辑,所以传引用进来便于修改
    //先查找是否有空闲的档位
    if (load.contains(ins.op)) {
        if (!busy_l[0]) {
            // std::cerr<<"loading the first position of RS load\n";
            ins.st=Issue;
            busy_l[0]=true;
            op[0]=ins.op;
            Dest[0]=i;
            Read_regs::execute(ins,Vj[0],Vk[0],Qj[0],Qk[0],pj[0],pk[0]);//读取Reg的值,并且修改RS表
            if (ins.op=="lb"||ins.op=="lbu"||ins.op=="lh"||ins.op=="lhu"||ins.op=="lw") {
                Write_regs::mark_Reg(ins.rd,i);
            }//Launch的同时我也应该把RF修改掉
            // std::cerr<<"here are the RS:\n";
            // show(0);
            return 0;
        }else if (!busy_l[1]) {
            // std::cerr<<"loading the second position of RS load\n";
            ins.st=Issue;
            busy_l[1]=true;
            op[1]=ins.op;
            Dest[1]=i;
            Read_regs::execute(ins,Vj[1],Vk[1],Qj[1],Qk[1],pj[1],pk[1]);
            if (ins.op=="lb"||ins.op=="lbu"||ins.op=="lh"||ins.op=="lhu"||ins.op=="lw") {
                Write_regs::mark_Reg(ins.rd,i);
            }
            // std::cerr<<"here are the RS:\n";
            // show(1);
            return 1;
        }else {
            // std::cerr<<"no spare RS position,launch failed"<<std::endl;
            return -1;//没有空闲的档位.不做调整
        }
    }else if (add.contains(ins.op)) {
        if (!busy_a[0]) {
            // std::cerr<<"loading the first position of RS add\n";
            ins.st=Issue;
            busy_a[0]=true;
            op[2]=ins.op;
            Dest[2]=i;
            Read_regs::execute(ins,Vj[2],Vk[2],Qj[2],Qk[2],pj[2],pk[2]);//读取Reg的值,并且修改RS表
            Write_regs::mark_Reg(ins.rd,i);
            // std::cerr<<"here are the RS:\n";
            // show(2);
            return 2;
        }else if (!busy_a[1]) {
            // std::cerr<<"loading the second position of RS add\n";
            ins.st=Issue;
            busy_a[1]=true;
            op[3]=ins.op;
            Dest[3]=i;
            Read_regs::execute(ins,Vj[3],Vk[3],Qj[3],Qk[3],pj[3],pk[3]);
            Write_regs::mark_Reg(ins.rd,i);
            // std::cerr<<"here are the RS:\n";
            // show(3);
            return 3;
        }else {
            // std::cerr<<"no spare RS position,launch failed"<<std::endl;
            return -1;//没有空闲的档位.不做调整
        }
    }else if (jump.contains(ins.op)){
        if (!busy_j[0]) {
            // std::cerr<<"loading the first position of RS jump\n";
            ins.st=Issue;
            busy_j[0]=true;
            op[4]=ins.op;
            Dest[4]=i;
            Read_regs::execute(ins,Vj[4],Vk[4],Qj[4],Qk[4],pj[4],pk[4]);//读取Reg的值,并且修改RS表
            if (ins.op=="jal"||ins.op=="jalr") {
                Write_regs::mark_Reg(ins.rd,i);
            }

            // std::cerr<<"here are the RS:\n";
            // show(4);
            return 4;
        }else if (!busy_j[1]) {
            // std::cerr<<"loading the second position of RS jump\n";
            ins.st=Issue;
            busy_j[1]=true;
            op[5]=ins.op;
            Dest[5]=i;
            Read_regs::execute(ins,Vj[5],Vk[5],Qj[5],Qk[5],pj[5],pk[5]);
            if (ins.op=="jal"||ins.op=="jalr") {
                Write_regs::mark_Reg(ins.rd,i);
            }
            // std::cerr<<"here are the RS:\n";
            // show(5);
            return 5;
        }else {
            // std::cerr<<"no spare RS position,launch failed"<<std::endl;
            return -1;//没有空闲的档位.不做调整
        }
    }
    return -1;
};

void RS::clear(const int i) {//把第i行对应指令删除
    if (i<0||i>=6) {
        std::__throw_runtime_error("ops!");
    }
    if (i==0||i==1) {//删除load指令
        busy_l[i]=false;
    }else if (i==2||i==3) {
        busy_a[i-2]=false;
    }else {
        busy_j[i-4]=false;
    }
    op[i]="";
    Dest[i]=A[i]=Vj[i]=Vk[i]=Qj[i]=Qk[i]=-1;
    pj[i]=pk[i]=Posi::none;
}

void RS::show(const int i) {
if (i==0||i==1) {
    std::cerr<<"pos:(load)"<<i<<"　busy:"<<busy_l[i]<<" op:"<<op[i]<<" Vj:"<<Vj[i]<<" Vk:"<<Vk[i]<<" Qj:"<<Qj[i]<<" Qk:"<<Qk[i]<<" pj:"<<pos_show(pj[i])<<" pk:"<<pos_show(pk[i])<<std::endl;
}else if (i==2||i==3) {
    std::cerr<<"pos:(add)"<<i<<"　busy:"<<busy_a[i-2]<<" op:"<<op[i]<<" Vj:"<<Vj[i]<<" Vk:"<<Vk[i]<<" Qj:"<<Qj[i]<<" Qk:"<<Qk[i]<<" pj:"<<pos_show(pj[i])<<" pk:"<<pos_show(pk[i])<<std::endl;
}else {
    std::cerr<<"pos:(jump)"<<i<<"　busy:"<<busy_j[i-4]<<" op:"<<op[i]<<" Vj:"<<Vj[i]<<" Vk:"<<Vk[i]<<" Qj:"<<Qj[i]<<" Qk:"<<Qk[i]<<" pj:"<<pos_show(pj[i])<<" pk:"<<pos_show(pk[i])<<std::endl;
}
    
}

void RS::show_everything() {
    for (int i=0;i<6;i++) {
        show(i);
    }
}

std::string RS::pos_show(Posi p) {
    std::string a[]{"none","rs1","rs2","imm","rd"};
    return a[static_cast<int>(p)];
}
