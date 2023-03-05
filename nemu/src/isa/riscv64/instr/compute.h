def_EHelper(auipc) {
  rtl_li(s, ddest, id_src1->imm + s->pc);
}

def_EHelper(addi) {
  rtl_addi(s, ddest, id_src1->preg, id_src2->imm);
}

def_EHelper(jal) {
  rtl_addi(s, ddest, 0, s->snpc);
  rtl_j(s, s->pc + (id_src1->simm));
}