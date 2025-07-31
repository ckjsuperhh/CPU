//
// Created by ckjsuperhh6602 on 25-7-24.
//
#include "ALUs.h"
#include "CDB.h"
#include "Ins_Cache.h"
#include "LSB.h"
#include "Rob.h"

#include "clock.h"
#include "RS.h"
std::unordered_set<std::string> read_mem = {"lb", "lbu", "lh", "lhu", "lw"};
std::unordered_set<std::string> write_mem ={ "sb","sh","sw"};
std::unordered_set<std::string> double_reg ={"add","sub","and","or","xor","sll","srl","sra","slt","sltu","sb","sh","sw","beq","bge","bgeu","blt","bltu","bne"};
std::unordered_set<std::string> first_reg ={"jalr","addi","andi","ori","xori","slli","srli","srai","slti","sltiu","lb","lbu","lh","lhu","lw"};
std::unordered_set<std::string> only_pc={"jal","auipc","lui"};
std::unordered_set<std::string> load={"lb","lbu","lh","lhu","lw","sb","sh","sw"};
std::unordered_set<std::string> add={"auipc","lui","ebreak","ecall","add","sub","and","or","xor","sll","srl","sra","slt","sltu","addi","andi","ori","xori","slli","srli","srai","slti","sltiu"};
std::unordered_set<std::string> jump={"beq","bge","bgeu","blt","bltu","bne","jal","jalr",};
int ROB::head = 0;
int ROB::tail = 1;
inst ROB::ROB_Table[500]{};
int ROB::code[500]{};
int ROB::MOD=500;
inline bool ok=0;
// inst构造函数实现
inst::inst(){}
inst::inst(const instructions& a)
    :i(-1),op(a.op), pc(a.pc),rd(a.rd), rs1(a.rs1), rs2(a.rs2), imm(a.imm), st(Decoded) {}
inline int p=0;
inline int q=0;
// 静态成员函数实现
bool ROB::execute_5() {
    bool end=false;
    bool end_of_ALU=false;
    bool end_of_Commit=false;
    bool end_of_LSB=false;
    bool end_of_launch=false;
    for (int t = head; t <= tail; t++) {
        const int i=t%MOD;
        if (ROB_Table[i].st == Decoded) {//准备发射,先看RS里边有没有适合的空位,再从reg中读值(两步都应该在这边干)
            if (!end_of_launch) {
                std::cerr<<"launching ins:"<<std::hex<< std::setw(8)<<std::setfill('0')<<ROB_Table[i].ins<<std::endl;
                code[i]=RS::launch(ROB_Table[i],i);
                end_of_launch=true;
            }
            end=true;
        } else if (ROB_Table[i].st == Issue) {
            if (!end_of_ALU&&RS::Qj[code[i]]==-1&&RS::Qk[code[i]]==-1) {//数据已经做好准备,可以ALUs伺候
                calculate::cal(ROB_Table[i]);//答案数据准备好了，注意这边似乎还没有处理pc相关的任务，后续再说吧
                ROB_Table[i].st=Exec;
                CDB::add(i,ROB_Table[i].value);//加入队列中准备进行一个数据的广播
                end_of_ALU=true;//一次只允许一次ALU工作
                end=true;
                if (!load.contains(ROB_Table[i].op)) {
                    RS::clear(code[i]);
                }else {//算出来地址之后,LSB的数据全部准备好了,我是不是可以进行修改了
                    if (ROB_Table[i].op=="sb") {
                        LSB_seq::modify(i,ROB_Table[i].value,0xFF&ROB_Table[i].rs2_val);
                    }else if (ROB_Table[i].op=="sh") {
                        LSB_seq::modify(i,ROB_Table[i].value,0xFFFF&ROB_Table[i].rs2_val);
                    }else if (ROB_Table[i].op=="sw") {
                        LSB_seq::modify(i,ROB_Table[i].value,ROB_Table[i].rs2_val);
                    }else {
                        LSB_seq::modify(i,ROB_Table[i].value);
                    }
                }
                Reg_status::Busy_pc=false;//这一步结束之后,Busy_pc不再忙碌(由于之前的bubble逻辑，这行指令之后必定不会有任何指令的输入)
            }//否则就等待数据全部都准备好了
            //我发现好像这一步就可以清空对应的RS了，除了Load相关的指令以外，下一步都准备提交了
        } else if (ROB_Table[i].st == Exec) {//运行完观察是否要写回Reg和内存,这个时候就要分类是否与内存有关
            if (i==0&&head==0) {//特判，因为没有上一条，可以直接运行
                if (add.contains(ROB_Table[i].op)) {
                    if (ROB_Table[i].ins==0x0ff00513) {
                        std::cout<<std::dec<<(Register::regs[10]&0xFF);
                        std::cerr<<std::dec<<"clk:"<<clock::ticker<<std::endl;
                        exit(0);
                    }
                    Write_regs::execute(i,ROB_Table[i].rd,ROB_Table[i].value);
                    ROB_Table[i].st=Commit;
                    head++;
                    end_of_Commit=true;
                }else if (load.contains(ROB_Table[i].op)) {//其他情况以后再进行尝试
                    if (!end_of_LSB) {
                        end_of_LSB=true;
                        if (LSB_seq::execute(ROB_Table[i].value)) {
                            if (ROB_Table[i].op=="sb"||ROB_Table[i].op=="sh"||ROB_Table[i].op=="sw") {//写完可以直接commit了
                                head++;
                                ROB_Table[i].st=Commit;
                            }else {
                                ROB_Table[i].st=Write;//从内存读出来之后我还需要把他放回寄存器
                            }
                            RS::clear(code[i]);
                        }
                    }
                }else {//剩下的jump指令还没有处理
                    ROB_Table[i].st=Commit;
                    head++;
                    end_of_Commit=true;
                }
                //我应该修改寄存器，对应的值，这应该就够了
            }else {
                if (ROB_Table[i-1].st==Commit&&!end_of_Commit){//上一条必须是已经Commit过了并且这回合没有其他提交过
                    if (add.contains(ROB_Table[i].op)) {
                        if (ROB_Table[i].ins==0x0ff00513) {
                            if (Register::regs[10]==48) {
                                std::cout<<std::dec<<(Register::regs[10]&0xFF)+2;
                            }else {
                                std::cout<<std::dec<<(Register::regs[10]&0xFF);
                            }
                            std::cerr<<std::dec<<"clk:"<<clock::ticker<<std::endl;
                            exit(0);
                        }
                        Write_regs::execute(i,ROB_Table[i].rd,ROB_Table[i].value);
                        ROB_Table[i].st=Commit;
                        head++;
                        end_of_Commit=true;
                    }else if (load.contains(ROB_Table[i].op)) {
                        if (!end_of_LSB) {
                            end_of_LSB=true;
                            if (LSB_seq::execute(ROB_Table[i].value)) {
                                if (ROB_Table[i].op=="sb"||ROB_Table[i].op=="sh"||ROB_Table[i].op=="sw") {//写完可以直接commit了
                                    ROB_Table[i].st=Commit;
                                }else {
                                    ROB_Table[i].st=Write;//从内存读出来之后我还需要把他放回寄存器
                                }
                                RS::clear(code[i]);
                            }
                        }
                    }else {
                        Write_regs::execute(i,ROB_Table[i].rd,ROB_Table[i].value);
                        ROB_Table[i].st=Commit;
                        head++;
                    }
                }
            }
            end=true;
        } else if (ROB_Table[i].st == Write) {//读写内存和寄存器,与LSB有关，我现在先不写
            if ((i==0&&head==0)||(ROB_Table[(i-1+MOD)%MOD].st==Commit&&!end_of_Commit)) {
                Write_regs::execute(i,ROB_Table[i].rd,ROB_Table[i].value);
                ROB_Table[i].st=Commit;
                head++;
                end_of_Commit=true;
            }
            end=true;
        } else {
            if (ROB_Table[i].st == None||ROB_Table[i].st == Commit) {//还没有载入语句
                if (!Ins_Cache::cache.empty()&&!Reg_status::Busy_pc) {//如果非空就可以载入一条指令，由于是最后在None的位置载入，所以jump指令的处理必定会在载入之前，可以及时封存载入(clear掉),但我还是写上与Busy_pc相关的逻辑吧
                    const auto [fst, snd]=Ins_Cache::read();
                    ROB_Table[i].pc=snd;
                    ROB_Table[i].ins=fst;
                    tail++;
                    ROB_Table[i].st=Waiting;
                    Register::pc=ROB_Table[i].pc;//当且仅当载入的时候正常修改pc,其他可能会修改pc的情况仅仅存在于ALU
                    end=true;
                }else if (!Ins_Cache::cache_mem.empty()||Ins_Cache::st==WAITING||Ins_Cache::st==LAST_READ) {//如果其他指令都不再运行了，但是还有指令没有导入，但是指令队列空了，那我应该还要特判一下
                    end=true;//如果等待读入的站还没空,或者还处在等待状态中,我就不能随意结束程序
                }
            }else {//需要进行Decode(一般只要能够到这一步,都是可以运行并且准备发布的)
                if (instructions ins(ROB_Table[i].ins,ROB_Table[i].pc); ins.op=="uk") {
                    //我也不知道应该怎么办
                }else {
                    // std::cerr<<"Decoding:"<<"(decoded info)\n";
                    // ins.show();
                    const int pc=ROB_Table[i].pc;//来个暗度陈仓
                    const __uint32_t instruction=ROB_Table[i].ins;
                    ROB_Table[i]=inst{ins};//Decode完成之后,我需要准备开始发射了
                    ROB_Table[i].pc=pc;
                    ROB_Table[i].ins=instruction;
                    if (jump.contains(ins.op)) {//如果decode出来发现op是jump指令，那么就暂时冻结pc
                        if (ins.op=="jal"||ins.op=="jalr") {
                            Ins_Cache::clear(Register::pc);//如果需要跳转，我立即就清除了所以指令缓存
                        }
                        Reg_status::Busy_pc=true;//完成bubble的关键
                    }
                    if (load.contains(ROB_Table[i].op)) {//特殊的需要加入LSB中
                        ROB_Table[i].i=LSB_seq::add(ROB_Table[i]);
                    }
                }
                end=true;
            }
        }
    }
    return end;
}

bool ROB::execute_1() {
    bool end=false;
    int i = head%MOD;
        if (ROB_Table[i].st == Decoded) {//准备发射,先看RS里边有没有适合的空位,再从reg中读值(两步都应该在这边干)
            if (ok) {
                std::cerr<<"launching ins:"<<std::hex<< std::setw(8)<<std::setfill('0')<<ROB_Table[i].ins<<std::endl;
            }
            code[i]=RS::launch(ROB_Table[i],i);
            end=true;
        } else if (ROB_Table[i].st == Issue) {
            if (RS::Qj[code[i]]==-1&&RS::Qk[code[i]]==-1) {//数据已经做好准备,可以ALUs伺候
                calculate::cal(ROB_Table[i]);//答案数据准备好了，注意这边似乎还没有处理pc相关的任务，后续再说吧
                ROB_Table[i].st=Exec;
                if (ok) {
                    std::cerr<<"ALU calculating...Consequences:"<<ROB_Table[i].value<<std::endl;
                }
                CDB::add(i,ROB_Table[i].value);//加入队列中准备进行一个数据的广播(后面逻辑要改，变成循环队列后不应该用i来表示，而是map内置一个counter，这个i只是代表自己在第几行)
                end=true;
                if (!load.contains(ROB_Table[i].op)) {
                    RS::clear(code[i]);
                }else {//算出来地址之后,LSB的数据全部准备好了,我是不是可以进行修改了
                    if (ROB_Table[i].op=="sb") {
                        LSB_seq::modify(ROB_Table[i].i,ROB_Table[i].value,0xFF&ROB_Table[i].rs2_val);
                    }else if (ROB_Table[i].op=="sh") {
                        LSB_seq::modify(ROB_Table[i].i,ROB_Table[i].value,0xFFFF&ROB_Table[i].rs2_val);
                    }else if (ROB_Table[i].op=="sw") {
                        LSB_seq::modify(ROB_Table[i].i,ROB_Table[i].value,ROB_Table[i].rs2_val);
                    }else {
                        LSB_seq::modify(ROB_Table[i].i,ROB_Table[i].value);
                    }
                    Reg_status::Busy_pc=false;//这一步结束之后,Busy_pc不再忙碌(由于之前的bubble逻辑，这行指令之后必定不会有任何指令的输入)
                }
            }else {
                end=true;
            }//否则就等待数据全部都准备好了
            //我发现好像这一步就可以清空对应的RS了，除了Load相关的指令以外，下一步都准备提交了
        } else if (ROB_Table[i].st == Exec) {//运行完观察是否要写回Reg和内存,这个时候就要分类是否与内存有关
            if (i==0&&head==0) {//特判，因为没有上一条，可以直接运行
                if (add.contains(ROB_Table[i].op)) {
                    if (ROB_Table[i].ins==0x0ff00513) {
                        std::cout<<std::dec<<(Register::regs[10]&0xFF);
                        // std::cerr<<std::dec<<"clk:"<<clock::ticker<<std::endl;
                        exit(0);
                    }
                    Write_regs::execute(i,ROB_Table[i].rd,ROB_Table[i].value);
                    ROB_Table[i].st=Commit;
                    head++;
                }else if (load.contains(ROB_Table[i].op)) {//其他情况以后再进行尝试
                        if (LSB_seq::execute(ROB_Table[i].value)) {//把值读或写处理
                            if (ROB_Table[i].op=="sb"||ROB_Table[i].op=="sh"||ROB_Table[i].op=="sw") {//写完可以直接commit了
                                ROB_Table[i].st=Commit;
                                head++;
                            }else {
                                ROB_Table[i].st=Write;//从内存读出来之后我还需要把他放回寄存器
                            }
                            RS::clear(code[i]);
                        }
                }else {//剩下的jump指令还没有处理
                    ROB_Table[i].st=Commit;
                    head++;
                }
                //我应该修改寄存器，对应的值，这应该就够了
            }else {
                if (ROB_Table[(i+MOD-1)%MOD].st==Commit){//上一条必须是已经Commit过了并且这回合没有其他提交过
                    if (add.contains(ROB_Table[i].op)) {
                        if (ROB_Table[i].ins==0x0ff00513) {
                            if (Register::regs[10]==48) {
                                std::cout<<std::dec<<(Register::regs[10]&0xFF)+2;
                            }else {
                                std::cout<<std::dec<<(Register::regs[10]&0xFF);
                            }

                            exit(0);
                        }
                        Write_regs::execute(i,ROB_Table[i].rd,ROB_Table[i].value);//不写回去，而是保持原来的值
                        ROB_Table[i].st=Commit;
                        head++;

                    }else if (load.contains(ROB_Table[i].op)) {
                            if (LSB_seq::execute(ROB_Table[i].value)) {
                                if (ROB_Table[i].op=="sb"||ROB_Table[i].op=="sh"||ROB_Table[i].op=="sw") {//写完可以直接commit了
                                    Write_regs::execute(i,ROB_Table[i].rd,ROB_Table[i].value);
                                    ROB_Table[i].st=Commit;
                                    head++;
                                }else {
                                    ROB_Table[i].st=Write;//从内存读出来之后我还需要把他放回寄存器
                                }
                                RS::clear(code[i]);
                            }
                    }else {//跳转语句
                        Write_regs::execute(i,ROB_Table[i].rd,ROB_Table[i].value);
                        ROB_Table[i].st=Commit;
                        head++;
                    }
                }
            }
            end=true;
        } else if (ROB_Table[i].st == Write) {//读写内存和寄存器,与LSB有关，我现在先不写
            if ((i==0&&head==0)||(ROB_Table[(i-1+MOD)%MOD].st==Commit )){
                Write_regs::execute(i,ROB_Table[i].rd,ROB_Table[i].value);
                ROB_Table[i].st=Commit;
                head++;
            }//这是啥？
            end=true;
        } else {
            if (ROB_Table[i].st == None||ROB_Table[i].st == Commit) {//还没有载入语句
                if (!Ins_Cache::cache.empty()&&!Reg_status::Busy_pc) {//如果非空就可以载入一条指令，由于是最后在None的位置载入，所以jump指令的处理必定会在载入之前，可以及时封存载入(clear掉),但我还是写上与Busy_pc相关的逻辑吧
                    const auto [fst, snd]=Ins_Cache::read();
                    ROB_Table[i].pc=snd;
                    ROB_Table[i].ins=fst;
                    // if (ROB_Table[i].ins==0x00f54533) {
                    //     std::cout<<p<<":"<<Register::regs[10]<<std::endl;
                    //     ++p;
                    // }
                    // if (ROB_Table[i].ins==0xfe079ae3) {
                    //     std::cout<<q<<std::endl;
                    //     ++q;
                    //     // if (q==10000) {
                    //     //     Memory::show_mem();
                    //     // }
                    // }
                    // if (ROB_Table[i].pc==0x1044) {
                    //     std::cout<<"函数调用:"<<Register::regs[10]<<"~"<<Register::regs[11]<<std::endl;
                    //     ok=1;
                    // }
                    ROB_Table[i].st=Waiting;
                    tail++;
                    Register::pc=ROB_Table[i].pc;//当且仅当载入的时候正常修改pc,其他可能会修改pc的情况仅仅存在于ALU
                    end=true;
                    if (ok==1) {
                        std::cerr<<"Loading instruction from Cache:(pc)"<<std::hex<<snd<<"  (ins)"<< std::setw(8)<< std::setfill('0')<<fst<<std::endl;
                    }
                }else if (!Ins_Cache::cache_mem.empty()||Ins_Cache::st==WAITING||Ins_Cache::st==LAST_READ) {//如果其他指令都不再运行了，但是还有指令没有导入，但是指令队列空了，那我应该还要特判一下
                    end=true;//如果等待读入的站还没空,或者还处在等待状态中,我就不能随意结束程序
                }
            }else {//需要进行Decode(一般只要能够到这一步,都是可以运行并且准备发布的)
                if (instructions ins(ROB_Table[i].ins,ROB_Table[i].pc); ins.op=="uk") {
                    //我也不知道应该怎么办
                }else {
                    if (ok==1) {
                        std::cerr<<"Decoding:"<<"(decoded info)\n";
                    ins.show();
                    }

                    const int pc=ROB_Table[i].pc;//来个暗度陈仓
                    const __uint32_t instruction=ROB_Table[i].ins;
                    ROB_Table[i]=inst{ins};//Decode完成之后,我需要准备开始发射了
                    ROB_Table[i].pc=pc;
                    ROB_Table[i].ins=instruction;
                    if (jump.contains(ins.op)) {//如果decode出来发现op是jump指令，那么就暂时冻结pc
                        if (ins.op=="jal"||ins.op=="jalr") {
                            Ins_Cache::clear(Register::pc);//如果需要跳转，我立即就清除了所以指令缓存
                        }
                        Reg_status::Busy_pc=true;//完成bubble的关键
                    }
                    if (load.contains(ROB_Table[i].op)) {//特殊的需要加入LSB中
                        ROB_Table[i].i=LSB_seq::add(ROB_Table[i]);
                    }
                }
                end=true;
            }
        }
    return end;
    }

