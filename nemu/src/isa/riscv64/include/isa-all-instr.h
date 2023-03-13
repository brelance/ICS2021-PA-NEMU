#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) \
f(auipc) f(ld) f(sd) f(inv) f(nemu_trap) \
f(addi) f(jal) f(jalr) f(lw) f(add) \
f(sub) f(bne) f(sltiu) f(beq) f(addw) \
f(addiw) f(slli) f(srli) f(bge) f(sw) \
f(lui) f(srai) f(mulw) f(divw) f(blt) \
f(andi) f(slt) f(lbu) f(sb) f(or) f(bltu) \
f(remw) f(slliw) f(srliw) f(sraiw) f(sraw) \
f(srlw) f(mul) f(subw) f(lh) f(lhu) f(sh) \
f(lb) f(sllw) f(xori) f(and) f(sltu) f(bgeu) \
f(divu) \

def_all_EXEC_ID();
