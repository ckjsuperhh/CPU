//
// Created by ckjsuperhh6602 on 25-7-22.
//

#ifndef PREDICTOR_H
#define PREDICTOR_H

class predictor {
public:
    static int head;//flush的时候还需要特判循环，那就到时候再说吧
    static int tail;//表明如果发生了错判应该清理什么(ALU算完之后就可以开始爆炸了)
    static bool busy;//如果busy的话我就不再读入jump语句,
    static int RF_data[32];//需要用来恢复RF中的数据，如果预测失败的，我就会把这个数据导入进去
    static bool RF_change[32];//预测期间我是否有修改Reorder,也就是标记了reorder，那么上面的RF_data就导入对应的寄存器(我觉得应该在预测前的指令提交的时候才修改RF_data)
    static void flush() {
        busy=false;
        head=tail=-1;

    }
    static void get_busy(const int i) {//当读入jump语句的时候，记得变忙
//如果没有占住位置，那么我就开始预测
    busy=true;
        head=tail=i;



    }
    static void add_tail() {
        tail++;
    }
};//我的想法是一旦读入了第二个jump指令，我就不再往后预测，相当于bubble，直到jump的ALU运行完了，我才能发现自己预测是否正确,对于jal和jalr这类直接跳转的指令我们就直接bubble,保持现状(如果是在预测期间遇到了jal和jalr了我们也直接停止)
//还有load相关的指令，我不认为可以执行，要不就不执行?
int predictor::head=-1;
int predictor::tail=-1;
bool predictor::busy=false;

#endif //PREDICTOR_H
