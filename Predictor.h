//
// Created by ckjsuperhh6602 on 25-7-22.
//

#ifndef PREDICTOR_H
#define PREDICTOR_H
#include "RF.h"

class predictor {
public:
    static int predicting_times;
    static int success_times;
    static int head;//flush的时候还需要特判循环，那就到时候再说吧
    static int tail;//表明如果发生了错判应该清理什么(ALU算完之后就可以开始爆炸了)
    static bool busy;//如果busy的话我就不再读入jump语句,
    static int RF_data[32];//需要用来恢复RF中的数据，如果预测失败的，我就会把这个数据导入进去(预测对应的语句没有rd，所以如果预测失败，那么RF的所有忙碌状态置空，数据恢复即可)
    static bool RS_occupy[6];//预测期间占用的RS
    static void flush() {
        busy=false;
        head=tail=-1;
        success_times--;
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
    }
    static void get_busy(const int i,const int rd) {//当读入jump语句的时候，记得变忙
        predicting_times++;
        success_times++;
//如果没有占住位置，那么我就开始预测
    busy=true;
        head=tail=i;
for (int i1=0;i1<32;i1++) {//复制一次data
    RF_data[i1]=Register::regs[i1];
}
    }
    static void add_tail() {
        tail++;
    }


    static void reserve_data(int rd, int value) {
        RF_data[rd]=value;
    }
};//我的想法是一旦读入了第二个jump指令，我就不再往后预测，相当于bubble，直到jump的ALU运行完了，我才能发现自己预测是否正确,对于jal和jalr这类直接跳转的指令我们就直接bubble,保持现状(如果是在预测期间遇到了jal和jalr了我们也直接停止)
//还有load相关的指令，我不认为可以执行，要不就不执行?
int predictor::head=-1;
int predictor::tail=-1;
bool predictor::busy=false;

#endif //PREDICTOR_H
