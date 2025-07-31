//
// Created by ckjsuperhh6602 on 25-7-29.
//

#include <map>
#include "LSB.h"

#include "Ins_Cache.h"


bool LSB::execute() {//我觉得在内部封装可能是一个更可行的事情
        if (!ready) {
            return false;
        }
        bool ok=false;
        if (rw==read1) {//关于0 and signed我好像还需要再做区分，后续再写
            ok=m.read1();
        }else if (rw==read2) {
            ok=m.read2();
        }else if (rw==read4) {
            ok=m.read4();
        }else if (rw==write1) {
            ok=m.write1();
        }else if (rw==write2) {
            ok=m.write2();
        }else if (rw==write4) {
            ok=m.write4();
        }//如果进行存取成功，那么就把这一个清除掉(传true出去，外部要把begin删掉)，不然就只是ticker往后走了一位
        return ok;
    }//还需要考虑的就是LSB怎么和ROB_Table中的指令进行互动(基本完成)

int LSB_seq::counter=0;
std::map<int,LSB> LSB_seq::cnt_LSB;//一个编号对应一个LSB
bool LSB_seq::execute(__int32_t &val) {
            if (cnt_LSB.empty()) {//空的就不执行
                return false;
            }
            if (cnt_LSB.begin()->second.execute()) {//反正进行一步，如果能进行就进行，否则就不进行，如果是写的话现在已经完成，如果不是写的话需要把数据传出去
                if (cnt_LSB.begin()->second.rw==read1||cnt_LSB.begin()->second.rw==read2||cnt_LSB.begin()->second.rw==read4) {
                    val=cnt_LSB.begin()->second.m.val;
                }
                // std::cerr<<"LSB has executed successfully!:"<<cnt_LSB.begin()->second.m.val<<std::endl;
                cnt_LSB.erase(cnt_LSB.begin()->first);

                return true;
            }
            return false;
        }
void LSB_seq::modify(const int i, const __uint32_t pc, const __int32_t val) {//当我需要对其进行修改的时候(需要明确修改什么)，准备好修改pc,ready,val(如果需要)
            if (cnt_LSB.contains(i)) {
                cnt_LSB[i].ready=true;
                cnt_LSB[i].m=ticker_mem{pc,Ins_Cache::Binary_Little_Endian(val)};
            }else {
                 throw;
            }
        }

int LSB_seq::add(const inst& ins) {
            //当decode完加入ROB的时候，我就应该把对应的LSB加入map中
            LSB tmp{};
            tmp.op=ins.op;
            if (ins.op=="lb") {
                tmp.rw=read1;
            }else if (ins.op=="lbu") {
                tmp.rw=read1;
            }else if (ins.op=="lh") {
                tmp.rw=read2;
            }else if (ins.op=="lhu") {
                tmp.rw=read2;
            }else if (ins.op=="lw") {
                tmp.rw=read4;
            }else if (ins.op=="sb") {
                tmp.rw=write1;
            }else if (ins.op=="sh") {
                tmp.rw=write2;
            }else if (ins.op=="sw") {
                tmp.rw=write4;
            }
            cnt_LSB.insert({counter++,tmp});
            return counter-1;//这样在外边或许可以记录下来
        }//ROB加入的时候，可以确定的是op,rw,后续只需要修改或添加ready和ticker_mem(先让他用默认构造吧)

