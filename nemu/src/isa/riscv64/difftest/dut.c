#include <isa.h>
#include <cpu/difftest.h>
#include "../local-include/reg.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc)
{
  if (ref_r->pc != pc)
  {
    printf("\33[1;31m");
    printf("Difftest failure: Spike pc = %lx  NEMU pc = %lx\n", ref_r->pc, pc);
  }
  for (int i = 0; i < 32; i++)
  {
    if (ref_r->gpr[i]._64 != gpr(i))
    {
      printf("\33[1;31m");
      printf("Difftest failure: regs[%d] Spike = %lx  NEMU = %lx\n", i, ref_r->gpr[i]._64, gpr(i));
      return false;
    }
  }
  return true;
}

void isa_difftest_attach()
{
}
