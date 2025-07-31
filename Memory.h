//
// Created by ckjsuperhh6602 on 25-7-22.
//

#ifndef MEMORY_H
#define MEMORY_H
#include <bitset>
#include <format>
#include <iostream>
#include <map>
#include <bits/types.h>
#include "Decoder.h"
inline __uint32_t Binary_Little_Endian(char ins[]){
    unsigned char bytes[4];
    for (int i = 0; i < 4; ++i) {
        const char tmp[3] = { ins[i*2], ins[i*2+1], 0 };
        bytes[i] = static_cast<unsigned char>(strtol(tmp, nullptr, 16));
    }
    return static_cast<__uint32_t>(bytes[3] << 24 | bytes[2] << 16 | bytes[1] << 8 | bytes[0]);
}

class Memory {
public:
    static std::map<__uint32_t,__uint8_t> mem;

    static __uint32_t read4(const __uint32_t from) {
        if (!mem.contains(from)||!mem.contains(from+1)||!mem.contains(from+2)||!mem.contains(from+3)) {
            return 0;
        }
        return mem[from+3]<<24|mem[from+2]<<16|mem[from+1]<<8|mem[from];
    }

    static __uint16_t read2(const __uint32_t from) {
        if (!mem.contains(from)||!mem.contains(from+1)) {
            return 0;
        }
        return mem[from+1]<<8|mem[from];
    }

    static __uint8_t read1(const __uint32_t from) {
        if (!mem.contains(from)) {
            return 0;
        }
        return mem[from];
    }

    static void write4(const __uint32_t to,char ins[]) {
        unsigned char bytes[4];
        for (int i = 0; i < 4; ++i) {
            const char tmp[3] = { ins[i*2], ins[i*2+1], 0 };
            bytes[i] = static_cast<unsigned char>(strtol(tmp, nullptr, 16));
        if (!mem.contains(to+i)) {
                mem.insert({to+i,bytes[i]});
            }else{mem[to+i]=bytes[i];}
        }
    }//比如0x00000008,得到00 00 00 08就顺序写进去

    static void write2(const __uint32_t to,char ins[]) {
        unsigned char bytes[2];
        for (int i = 0; i < 2; ++i) {
            // 每次取2个字符，组成一个十六进制数
            const char tmp[3] = { ins[i*2], ins[i*2+1], 0 };
            // 转换为字节并存储
            bytes[i] = static_cast<unsigned char>(strtol(tmp, nullptr, 16));
            if (!mem.contains(to+i)) {
                mem.insert({to+i,bytes[i]});
            }else{mem[to+i]=bytes[i];}
        }
    }

    static void write1(const __uint32_t to,const char ins[]) {
        if (!mem.contains(to)) {
            mem.insert({to, std::stoi(ins)});
        }else{mem[to]=std::stoi(ins);}
    }

    static void store_ins() {
        char first;
        __uint32_t pc=0;
        while (std::cin>>first) {
            if (first=='@') {
                char addr[9];
                for (int i=0;i<8;i++) {
                    std::cin>>addr[i];
                }
                addr[8]='\0';
                pc=strtol(addr, nullptr, 16);
            }else {
                char ins[9];
                ins[0]=first;
                for (int i=1;i<8;i++) {
                    std::cin>>ins[i];
                    while (ins[i]==' ') {
                        std::cin>>ins[i];
                    }
                }
                ins[8]='\0';
                // const __uint32_t instruction=Binary_Little_Endian(ins);
                write4(pc,ins);
                pc+=4;
            }
        }
    }

    static void show_mem() {
        for(auto [key,val]:mem){
            std::cout<<std::hex<<std::setw(4)<<key<<std::dec<<":"<<std::bitset<8>(val)<<std::endl;
        }
    }
};

class ticker_mem {
public:
    friend class Memory;
    int ticker{};
    __uint32_t pc{},val{};

    ticker_mem()=default;
    explicit ticker_mem(const __uint32_t pc, const __int32_t val=0):pc(pc){
        this->val=val;
        ticker=0;
    }

    bool read4() {
        if (++ticker!=3) {
            return false;
        }
        this->val=Memory::read4(pc);
        return true;
    }

    bool read2() {
        if (++ticker!=3) {
            return false;
        }
        this->val=Memory::read2(pc);
        return true;
    }

    bool read1() {
        if (++ticker!=3) {
            return false;
        }
        this->val=Memory::read1(pc);
        return true;
    }

    bool write4() {
        if (++ticker!=3) {
            return false;
        }
        Memory::write4(pc,std::format("{:08x}", val).data());
        return true;
    }

    bool write2() {
        if (++ticker!=3) {
            return false;
        }
        Memory::write2(pc,std::format("{:04x}", val).data());
        return true;
    }

    bool write1() {
        if (++ticker!=3) {
            return false;
        }
        Memory::write1(pc,std::format("{:02x}", val).data());
        return true;
    }

};



#endif //MEMORY_H
