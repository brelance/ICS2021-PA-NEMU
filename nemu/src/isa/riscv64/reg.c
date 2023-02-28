#include <isa.h>
#include <macro.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

#define REG_NUMS ARRLEN(regs)

void isa_reg_display() {
  for(int idx = 0; idx < REG_NUMS; idx++) {
    printf("%s: %lu \n", regs[idx], gpr(idx));
  }
}

uint64_t isa_reg_read(char *reg_name) {
  int idx;
  for (idx = 0; idx < REG_NUMS; idx++) {
    if (reg_name == regs[idx]) {
      break;
    }
  }
  return gpr(idx);
}

word_t isa_reg_str2val(const char *s, bool *success) {
  return 0;
}
