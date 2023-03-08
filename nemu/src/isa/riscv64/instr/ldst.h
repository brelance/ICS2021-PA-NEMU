def_EHelper(ld) {
  rtl_lm(s, ddest, dsrc1, id_src2->imm, 8);
}

def_EHelper(lw) {
  rtl_lm(s, ddest, dsrc1, id_src2->imm, 4);
}


def_EHelper(sd) {
  rtl_sm(s, ddest, dsrc1, id_src2->imm, 8);
}

def_EHelper(sw) {
  rtl_sm(s, ddest, dsrc1, id_src2->imm, 4);
}





