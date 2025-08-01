//
// Created by ckjsuperhh6602 on 25-8-1.
//
#include "Predictor.h"
inline int delta(int a,int b) {
    if (a<=b) {
        return b-a;
    }
    return 500-a+b;
}
void predictor::flush() {
    busy=false;

    for (int i=0;i<32;i++) {//RF的修改
        Register::regs[i]=RF_data[i];
        Reg_status::Reorder[i]=-1;
        Reg_status::Busy[i]=false;
    }
    for (int i=0;i<6;i++) {
        if (RS_occupy[i]) {//如果这条RS被占用
            RS::clear(i);
        }
    }
    ROB::tail-=delta(head,tail);
    if (head<=tail) {
        for (int i=head+1;i<=tail;i++) {
            std::cerr<<std::dec<<i<<"-----Flushing:(pc)"<<std::hex<<ROB::ROB_Table[i].pc<<" (ins)"<<ROB::ROB_Table[i].ins<<std::endl;
            ROB::ROB_Table[i]=inst{};
        }
    }else {
        for (int i=head+1;i<500;i++) {
            std::cerr<<std::dec<<i<<"-----Flushing:(pc)"<<std::hex<<ROB::ROB_Table[i].pc<<" (ins)"<<ROB::ROB_Table[i].ins<<std::endl;
           ROB::ROB_Table[i]=inst{};
        }
        for (int i=0;i<=tail;i++) {
           std::cerr<<std::dec<<i<<"-----Flushing:(pc)"<<std::hex<<ROB::ROB_Table[i].pc<<" (ins)"<<ROB::ROB_Table[i].ins<<std::endl;
             ROB::ROB_Table[i]=inst{};
        }
    }
    head=tail=-1;
    Reg_status::Busy_pc=false;
}

void predictor::get_busy(const int i) {//当读入jump语句的时候，记得变忙
    predicting_times++;
    //如果没有占住位置，那么我就开始预测
    busy=true;
    head=tail=i;
    for (int i1=0;i1<32;i1++) {//复制一次data
        RF_data[i1]=Register::regs[i1];
    }

}

void predictor::add_tail() {
    tail++;
}
void predictor::reserve_data(int rd, int value) {
    RF_data[rd]=value;
}
void predictor::clean_predicting() {
    busy=false;
    std::cerr<<"predict right!----"<<head<<"~"<<tail<<std::endl;
    success_times++;
    if ((head!=tail&&ROB::ROB_Table[tail].op!="jal"&&ROB::ROB_Table[tail].op!="jalr")||(head==tail)) {
        Reg_status::Busy_pc=false;
    }//bne等相关跳转结束后，可能会修正busy_pc导致原有的jal的跳转bubble失效，所以需要加入这一条
    for (int i=head+1;i<500;i++) {
        ROB::ROB_Table[i].predicting=false;
    }
    for (int i=0;i<=tail;i++) {
        ROB::ROB_Table[i].predicting=false;
    }
}
//我的想法是一旦读入了第二个jump指令，我就不再往后预测，相当于bubble，直到jump的ALU运行完了，我才能发现自己预测是否正确,对于jal和jalr这类直接跳转的指令我们就直接bubble,保持现状(如果是在预测期间遇到了jal和jalr了我们也直接停止)
//还有load相关的指令，我不认为可以执行，要不就不执行?
int predictor::head=-1;
int predictor::tail=-1;
bool predictor::busy=false;
int predictor::predicting_times=0;
int predictor::success_times=0;
int predictor::RF_data[32]={};
bool predictor::RS_occupy[6]={};