// compute
def_EHelper(auipc)
{
  rtl_li(s, ddest, id_src1->imm + s->pc);
}

def_EHelper(addi)
{
  rtl_addi(s, ddest, id_src1->preg, id_src2->imm);
}

def_EHelper(sltiu)
{
    *ddest = (sword_t)(*dsrc1 - id_src2->imm) < 0 ? 1 : 0;
}

def_EHelper(add)
{
  rtl_add(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sub)
{
  rtl_sub(s, ddest, dsrc1, dsrc2);
}
// src/isa/riscv64/instr/compute.h:25

// branch
def_EHelper(jal)
{
  // *ddest = s->snpc;
  rtl_li(s, ddest, s->snpc);
  rtl_j(s, s->pc + (id_src1->simm));
}

def_EHelper(jalr)
{
  // *ddest = s->snpc;
  rtl_li(s, ddest, s->snpc);
  rtl_j(s, *dsrc1 + id_src2->imm);
}

def_EHelper(beq)
{
  rtlreg_t temp;
  rtl_xor(s, &temp, dsrc1, dsrc2);
  if (temp == 0)
  {
    rtl_li(s, &s->dnpc, s->pc + *s0);
  }
}

def_EHelper(bne)
{
  rtlreg_t temp;
  rtl_xor(s, &temp, dsrc1, dsrc2);
  if (temp != 0)
  {
    rtl_li(s, &s->dnpc, s->pc + *s0);
  }
}

// riscv64_reg
def_EHelper(addw) {
  rtl_addw(s, ddest, dsrc1, dsrc2);
}


// riscv64_imm
def_EHelper(addiw) {
  rtl_addiw(s, ddest, dsrc1, id_src2->imm);
}