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
    static void flush();
    static void get_busy(const int i);
    static void add_tail();
    static void reserve_data(int rd, int value);
};
#endif //PREDICTOR_H
