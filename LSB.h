//
// Created by ckjsuperhh6602 on 25-7-22.
//

#ifndef LSB_H
#define LSB_H
#include <map>

#include "Memory.h"
#include "Rob.h"
struct inst;
enum Op {
    nothing,read1,read2,read4,write1,write2,write4
};
class LSB {
//这不是一个static的类，需要生成一个又一个的对象，然后我要做的就是把这些对象按照先进先出的队列方式运行
public:
    bool ready{};//那么这个时候就涉及到怎么修改队列中的对象，所以更好的方法应该是一个有序的map而不是队列，这样就可以根据counter给出的索引进行查询，而需要运行就把begin拿出来，观察是否可以运行
    ticker_mem m{};
    std::string op{};

    Op rw{};
    bool execute() ;//我觉得在内部封装可能是一个更可行的事情
    //还需要考虑的就是LSB怎么和ROB_Table中的指令进行互动(基本完成)
};
    class LSB_seq {
        /*
         * 重点就是数据在计算出来之后，我应该维护内存读写的一个顺序性
         * 有个初步的想法，就是写一个并非static的类，每个都是一个对象，然后再由一个统一的标准进行排序和赋值
         * 无论如何，指令肯定是先读入比后读入要早，那么就可以在加入ROB的时候进行赋值
         */
    public:
        static int counter;
        static std::map<int,LSB> cnt_LSB;//一个编号对应一个LSB
        static bool execute(__int32_t &val) ;
        static void modify(int i, __uint32_t pc,  __int32_t val=0) ;//当我需要对其进行修改的时候(需要明确修改什么)，准备好修改pc,ready,val(如果需要)
        static int add(const inst& ins) ;//ROB加入的时候，可以确定的是op,rw,后续只需要修改或添加ready和ticker_mem(先让他用默认构造吧)
    };

inline int counter=0;
#endif //LSB_H
