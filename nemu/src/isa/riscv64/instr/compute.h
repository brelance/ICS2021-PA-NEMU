def_EHelper(auipc) {
  rtl_li(s, ddest, id_src1->imm + s->pc);
}

def_EHelper(addi) {
  rtl_addi(s, ddest, id_src1->preg, id_src2->imm);
}


def_EHelper(jal) {
  *ddest += s->snpc;
  rtl_j(s, id_src1->imm + s->pc);
}

def_EHelper(jalr) {
  *ddest += s->snpc;
  rtl_j(s, *dsrc1 + id_src2->imm);
}
