//
// Created by ckjsuperhh6602 on 25-7-25.
//

#ifndef INS_CACHE_H
#define INS_CACHE_H
#include <queue>
#include <cstdint>

// 前置声明
struct ticker_mem;
class Memory;
struct inst;

enum Status{
    NONE,WAITING,FINISHED,LAST_READ,UNABLED
};//如果快要用完就重新读取一堆,所以我只设置这五个状态,倒数第二个状态就是承接UNABLED的最后一次读入,最后一个状态表示已经读到最后没办法再往后了


class Ins_Cache {
public:
    static Status st;
    static int pc;
    static std::queue<std::pair<ticker_mem,int>> cache_mem;
    static std::queue<std::pair<__uint32_t,int>> cache;//读出来的时候大不了用这个queue是否为空来判断能否读取
    static int Binary_Little_Endian(__uint32_t ins);

    static std::pair<__uint32_t,int> read();//读取最近的一条指令
    static bool empty();//规范用法，读取之前先要判空
    static void clear(int _pc);//涉及清除指令，应该都是jump要更改pc的时候，那么就传入pc允许修改
    static void check();
    static void modify(__uint32_t pc);
};

#endif //INS_CACHE_H
