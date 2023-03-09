// compute imm
def_EHelper(auipc)
{
  rtl_li(s, ddest, id_src1->imm + s->pc);
}

def_EHelper(lui)
{
  rtl_li(s, ddest, id_src1->imm);
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

def_EHelper(slli)
{
  rtl_slli(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(srli)
{
  rtl_srli(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(srai)
{
  rtl_srli(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(andi)
{
  rtl_andi(s, ddest, dsrc1, id_src2->imm);
}

// compute regs
def_EHelper(sub)
{
  rtl_sub(s, ddest, dsrc1, dsrc2);
}

def_EHelper(slt)
{
  *ddest = (sword_t)(*dsrc1 - *dsrc2) < 0 ? 1 : 0;
}

def_EHelper(or)
{
  rtl_or(s, ddest, dsrc1, dsrc2);
}

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

def_EHelper(blt)
{
  if ((sword_t)*dsrc1 < (sword_t)*dsrc2)
  {
    rtl_li(s, &s->dnpc, s->pc + *s0);
  }
}

def_EHelper(bge)
{
  if ((sword_t)*dsrc1 >= (sword_t)*dsrc2)
  {
    rtl_li(s, &s->dnpc, s->pc + *s0);
  }
}

// TODO: bug to-lower-case
def_EHelper(bltu)
{
  if(*dsrc1 < *dsrc2) {
    rtl_li(s, &s->dnpc, s->pc + *s0);
  }
}
// riscv64_reg
def_EHelper(addw)
{
  rtl_addw(s, ddest, dsrc1, dsrc2);
}

// riscv64_imm
def_EHelper(addiw)
{
  rtl_addiw(s, ddest, dsrc1, id_src2->imm);
}

// riscv64_m
def_EHelper(mulw)
{
  rtl_mulw(s, ddest, dsrc1, dsrc2);
}

def_EHelper(divw)
{
  rtl_divw(s, ddest, dsrc1, dsrc2);
}

def_EHelper(remw)
{
  rtl_remw(s, ddest, dsrc1, dsrc2);
}