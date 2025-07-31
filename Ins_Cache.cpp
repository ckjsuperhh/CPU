//
// Created by ckjsuperhh6602 on 25-7-29.
//
//
// Created by ckjsuperhh6602 on 25-7-25.
//

#include "Ins_Cache.h"
#include "Memory.h"
#include "Rob.h"
#include <vector>
#include <string>

// 初始化静态成员变量
Status Ins_Cache::st = NONE;
int Ins_Cache::pc = 0;//这个值是类内维护的一个值，外部的pc，记录当下跑到哪个位置，对其刚刚decode进来的指令，而这个pc则是记录当下读到哪个位置
std::queue<std::pair<ticker_mem, int>> Ins_Cache::cache_mem;
std::queue<std::pair<__uint32_t, int>> Ins_Cache::cache;

int Ins_Cache::Binary_Little_Endian(__uint32_t ins) {
    __uint32_t a(ins&0xFF),b((ins>>8)&0xff),c((ins>>16)&0xff),d((ins>>24)&0xff);
    return a<<24|b<<16|c<<8|d;
}

// 实现read()函数
std::pair<__uint32_t, int> Ins_Cache::read() {
    auto tmp = cache.front();
    cache.pop();
    return tmp;
}

// 实现empty()函数
bool Ins_Cache::empty() {
    return cache.empty();
}

// 实现clear()函数
void Ins_Cache::clear(const int _pc) {
    // std::cerr<<"Countering jump command:clearing Ins_Cache\n";
    pc = _pc;//传进来pc，是为了调整内部读取指令的位置
    while (!cache.empty()) {
        cache.pop();
    }
    while (!cache_mem.empty()) {
        cache_mem.pop();
    }
    st = NONE;
}

// 实现check()函数
void Ins_Cache::check() {
    // std::cerr<<"Remaining "<<cache.size()<<" instructions in the cahce"<<std::endl;
    // std::cerr<<"State is "<< st<<std::endl;
    if (st == UNABLED|| Reg_status::Busy_pc) {
        return;
    }
    if (st == NONE) {
        for (int i = 0; i < 8; ++i, pc += 4) {
            if (Memory::read4(pc) == 0) {
                st = LAST_READ;
                break;
            }
            cache_mem.emplace(ticker_mem(pc), pc);
        }
        if (st != LAST_READ) {
            st = WAITING;
        }
    } else if (st == WAITING) {
        bool ok = false;
        std::vector<std::pair<ticker_mem, int>> tmp;
        while (!cache_mem.empty()) {
            tmp.push_back(cache_mem.front());
            cache_mem.pop();
            ok = tmp.back().first.read4();
        }
        if (ok) {
            for (auto &[fst, snd] : tmp) {
                cache.emplace(fst.val, snd);
            }
            st = FINISHED;
        } else {
            for (auto &it : tmp) {
                cache_mem.push(it);
            }
        }
    } else if (st == LAST_READ) {
        bool ok = false;
        std::vector<std::pair<ticker_mem, int>> tmp;
        while (!cache_mem.empty()) {
            tmp.push_back(cache_mem.front());
            cache_mem.pop();
            ok = tmp.back().first.read4();
        }
        if (ok) {
            for (auto &[fst, snd] : tmp) {
                cache.emplace(fst.val, snd);
            }
            st = UNABLED;
        } else {
            for (auto &it : tmp) {
                cache_mem.push(it);
            }
        }
    } else if (st == FINISHED) {
        if (cache.size() <= 3) {
            bool hit_last_read = false;
            for (int i = 0; i < 8 - cache.size(); ++i, pc += 4) {
                if (Memory::read4(pc) == 0) {
                    hit_last_read = true;
                    break;
                }
                cache_mem.emplace(ticker_mem(pc), pc);
            }
            if (!hit_last_read) {
                st = WAITING;
            } else {
                st = LAST_READ;
            }
        }
    }
}
void Ins_Cache::modify(const __uint32_t pc) {
    Ins_Cache::pc=pc;
}
