#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(auipc) f(ld) f(sd) f(inv) f(nemu_trap) f(addi) f(jal) f(jalr) f(lw) f(add) f(sub) f(bne) f(sltiu) f(beq) \
    f(addw)\

def_all_EXEC_ID();
