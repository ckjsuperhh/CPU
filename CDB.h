//
// Created by ckjsuperhh6602 on 25-7-24.
//

#ifndef CDB_H
#define CDB_H
#include <map>
#include <unordered_map>
#include <bits/types.h>

#include "RS.h"
#include "Rob.h"  // 确保包含ROB的头文件

enum Pos {
    RS, RF, Mem, Rob
};

class CDB {
public:
    static std::map<int, int> update;  // 声明静态成员变量

    // 成员函数声明
    static void execute();
    static void add(int i, int data);
    static void broadcast(int i, int value);
};

#endif // CDB_H
