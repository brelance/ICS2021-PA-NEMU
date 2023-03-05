def_EHelper(auipc) {
  rtl_li(s, ddest, id_src1->imm + s->pc);
}

def_EHelper(addi) {
  rtl_addi(s, ddest, id_src1->preg, id_src2->imm);
}


// def_EHelper(jal) {
//   *ddest = s->snpc;
//   // printf("jal: %lx\n", s->snpc);
//   // printf("s->pc: %lx \t id_src1->imm: %lx\n", s->pc, id_src1->simm);
//   rtl_j(s, s->pc + (id_src1->imm >> 12));
// }

// def_EHelper(jalr) {
//   *ddest += s->snpc;
//   rtl_j(s, *dsrc1 + id_src2->imm);
// }

def_EHelper(jal) {
  rtl_addi(s, ddest, &s->pc, 4);
  rtl_addi(s, &s->dnpc, &s->pc, id_src1->imm);
}

def_EHelper(jalr) {
  rtl_addi(s, s0, &s->pc, 4);
  rtl_addi(s, &s->dnpc, dsrc1, id_src2->imm);
  rtl_andi(s, &s->dnpc, &s->dnpc, ~1);
  rtl_addi(s, ddest, s0, 0);
}

