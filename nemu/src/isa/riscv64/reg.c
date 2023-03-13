#include <isa.h>
#include <macro.h>
#include "local-include/reg.h"

const char *regs[] = {
    "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

#define REG_NUMS ARRLEN(regs)

void isa_reg_display()
{
  printf(ASNI_FG_RED);
  printf("----NEMU register states start----\n");
  for (int idx = 0; idx < REG_NUMS; idx++)
  {
    printf("%s: %lx\n", regs[idx], gpr(idx));
  }
  printf("----NEME rigister states end----\n");
}

void spike_reg_display(CPU_state *refs)
{
  printf(ASNI_FG_GREEN);
  printf("----Spike register states start----\n");
  for (int idx = 0; idx < REG_NUMS; idx++)
  {
    printf("%s: %lx \n", regs[idx], refs->gpr[idx]._64);
  }
  printf("----Spike rigister states end----\n");
}


uint64_t isa_reg_str2val(const char *s, bool *success)
{
  int idx;
  for (idx = 0; idx < REG_NUMS; idx++)
  {
    if (s == regs[idx])
    {
      break;
    }
  }
  return gpr(idx);
}
