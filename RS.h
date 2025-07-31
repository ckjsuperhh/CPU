//
// Created by ckjsuperhh6602 on 25-7-22.
//

#ifndef RS_H
#define RS_H
#include <string>

#include "Rob.h"
enum class Posi {
    none,rs1,rs2,imm,rd
};
struct inst;

class RS {
public:
    static bool busy_a[2];
    static bool busy_l[2];
    static bool busy_j[2];
    static std::string op[6];
    static int Vj[6];
    static int Vk[6];
    static int Qj[6];
    static int Qk[6];
    static int A[6];
    static int Dest[6];
    static Posi pj[6];
    static Posi pk[6];
    static int launch(inst &ins, int i);

    static void clear(int i);

    static void show(int i);
    static void show_everything();

    static std::string pos_show(Posi p);
};
#endif //RS_H
