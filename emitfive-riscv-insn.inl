// RV32I Base Instruction Set
INSTRUCTION(Lui, lui, U, 0b0110111)
INSTRUCTION(Auipc, auipc, U, 0b0010111)
INSTRUCTION(Jal, jal, J, 0b1101111)
INSTRUCTION(Jalr, jalr, S, 0b1100111, 0b000)
INSTRUCTION(Beq, beq, B, 0b1100011, 0b000)
INSTRUCTION(Bne, bne, B, 0b1100011, 0b001)
INSTRUCTION(Blt, blt, B, 0b1100011, 0b100)
INSTRUCTION(Bge, bge, B, 0b1100011, 0b101)
INSTRUCTION(Bltu, bltu, B, 0b1100011, 0b110)
INSTRUCTION(Bgeu, bgeu, B, 0b1100011, 0b111)
INSTRUCTION(Lb, lb, I, 0b0000011, 0b000)
INSTRUCTION(Lh, lh, I, 0b0000011, 0b001)
INSTRUCTION(Lw, lw, I, 0b0000011, 0b010)
INSTRUCTION(Lbu, lbu, I, 0b0000011, 0b100)
INSTRUCTION(Lhu, lhu, I, 0b0000011, 0b101)
INSTRUCTION(Sb, sb, S, 0b0100011, 0b000)
INSTRUCTION(Sh, sh, S, 0b0100011, 0b001)
INSTRUCTION(Sw, sw, S, 0b0100011, 0b010)
INSTRUCTION(Addi, addi, I, 0b0010011, 0b000)
INSTRUCTION(Slti, slti, I, 0b0010011, 0b010)
INSTRUCTION(Sltiu, sltiu, I, 0b0010011, 0b011)
INSTRUCTION(Xori, xori, I, 0b0010011, 0b100)
INSTRUCTION(Ori, ori, I, 0b0010011, 0b110)
INSTRUCTION(Andi, andi, I, 0b0010011, 0b111)
#if 0
//INSTRUCTION(Slli, slli) // these also have 64 bit versions
//INSTRUCTION(Srli, srli) // these also have 64 bit versions
//INSTRUCTION(Srai, srai) // these also have 64 bit versions
#endif
INSTRUCTION(Add, add, R, 0b0110011, 0b000, 0b0000000)
INSTRUCTION(Sub, sub, R, 0b0110011, 0b000, 0b0100000)
INSTRUCTION(Sll, sll, R, 0b0110011, 0b001, 0b0000000)
INSTRUCTION(Slt, slt, R, 0b0110011, 0b010, 0b0000000)
INSTRUCTION(Sltu, sltu, R, 0b0110011, 0b011, 0b0000000)
INSTRUCTION(Xor, xor_, R, 0b0110011, 0b100, 0b0000000)
INSTRUCTION(Srl, srl, R, 0b0110011, 0b101, 0b0000000)
INSTRUCTION(Sra, sra, R, 0b0110011, 0b101, 0b0100000)
INSTRUCTION(Or, or_, R, 0b0110011, 0b110, 0b0000000)
INSTRUCTION(And, and_, R, 0b0110011, 0b111, 0b0000000)
#if 0
INSTRUCTION(Fence, fence)
INSTRUCTION(Ecall, ecall)
INSTRUCTION(Ebreak, ebreak)


// RV64I Base Instruction Set (in addition to RV32I)
INSTRUCTION(Lwu, lwu)
INSTRUCTION(Ld, ld)
INSTRUCTION(Sd, sd)
INSTRUCTION(Slli, slli)
INSTRUCTION(Srli, srli)
INSTRUCTION(Srai, srai)
INSTRUCTION(Addiw, addiw)
INSTRUCTION(Slliw, slliw)
INSTRUCTION(Srliw, srliw)
INSTRUCTION(Sraiw, sraiw)
INSTRUCTION(Addw, addw)
INSTRUCTION(Subw, subw)
INSTRUCTION(Sllw, sllw)
INSTRUCTION(Srlw, srlw)
INSTRUCTION(Sraw, sraw)

// RV32/RV64 Zifencei Standard Extension
INSTRUCTION(Fence_i, fence_i)

// RV32/RV64 Zicsr Standard Extension
INSTRUCTION(Csrrw, csrrw)
INSTRUCTION(Csrrs, csrrs)
INSTRUCTION(Csrrc, csrrc)
INSTRUCTION(Csrrwi, csrrwi)
INSTRUCTION(Csrrsi, csrrsi)
INSTRUCTION(Csrrci, csrrci)

// RV32M Standard Extension
INSTRUCTION(Mul, mul)
INSTRUCTION(Mulh, mulh)
INSTRUCTION(Mulhsu, mulhsu)
INSTRUCTION(Mulhu, mulhu)
INSTRUCTION(Div, div)
INSTRUCTION(Divu, divu)
INSTRUCTION(Rem, rem)
INSTRUCTION(Remu, remu)

// RV64M Standard Extension (in addition to RV32M)
INSTRUCTION(Mulw, mulw)
INSTRUCTION(Divw, divw)
INSTRUCTION(Divuw, divuw)
INSTRUCTION(Remw, remw)
INSTRUCTION(Remuw, remuw)

// RV32A Standard Extension
INSTRUCTION(Lr_w, lr_w)
INSTRUCTION(Sc_w, sc_w)
INSTRUCTION(Amoswap_w, amoswap_w)
INSTRUCTION(Amoadd_w, amoadd_w)
INSTRUCTION(Amoxor_w, amoxor_w)
INSTRUCTION(Amoand_w, amoand_w)
INSTRUCTION(Amoor_w, amoor_w)
INSTRUCTION(Amomin_w, amomin_w)
INSTRUCTION(Amomax_w, amomax_w)
INSTRUCTION(Amominu_w, amominu_w)
INSTRUCTION(Amomaxu_w, amomaxu_w)

// RV64A Standard Extension (in addition to RV32A)
INSTRUCTION(Lr_d, lr_d)
INSTRUCTION(Sc_d, sc_d)
INSTRUCTION(Amoswap_d, amoswap_d)
INSTRUCTION(Amoadd_d, amoadd_d)
INSTRUCTION(Amoxor_d, amoxor_d)
INSTRUCTION(Amoand_d, amoand_d)
INSTRUCTION(Amoor_d, amoor_d)
INSTRUCTION(Amomin_d, amomin_d)
INSTRUCTION(Amomax_d, amomax_d)
INSTRUCTION(Amominu_d, amominu_d)
INSTRUCTION(Amomaxu_d, amomaxu_d)

// RV32F Standard Extension
INSTRUCTION(Flw, flw)
INSTRUCTION(Fsw, fsw)
INSTRUCTION(Fmadd_s, fmadd_s)
INSTRUCTION(Fmsub_s, fmsub_s)
INSTRUCTION(Fnmsub_s, fnmsub_s)
INSTRUCTION(Fnmadd_s, fnmadd_s)
INSTRUCTION(Fadd_s, fadd_s)
INSTRUCTION(Fsub_s, fsub_s)
INSTRUCTION(Fmul_s, fmul_s)
INSTRUCTION(Fdiv_s, fdiv_s)
INSTRUCTION(Fsqrt_s, fsqrt_s)
INSTRUCTION(Fsgnj_s, fsgnj_s)
INSTRUCTION(Fsgnjn_s, fsgnjn_s)
INSTRUCTION(Fsgnjx_s, fsgnjx_s)
INSTRUCTION(Fmin_s, fmin_s)
INSTRUCTION(Fmax_s, fmax_s)
INSTRUCTION(Fcvt_w_s, fcvt_w_s)
INSTRUCTION(Fcvt_wu_s, fcvt_wu_s)
INSTRUCTION(Fmv_x_w, fmv_x_w)
INSTRUCTION(Feq_s, feq_s)
INSTRUCTION(Flt_s, flt_s)
INSTRUCTION(Fle_s, fle_s)
INSTRUCTION(Fclass_s, fclass_s)
INSTRUCTION(Fcvt_s_w, fcvt_s_w)
INSTRUCTION(Fcvt_s_wu, fcvt_s_wu)
INSTRUCTION(Fmv_w_x, fmv_w_x)

// RV64F Standard Extension (in addition to RV32F)
INSTRUCTION(Fcvt_l_s, fcvt_l_s)
INSTRUCTION(Fcvt_lu_s, fcvt_lu_s)
INSTRUCTION(Fcvt_s_l, fcvt_s_l)
INSTRUCTION(Fcvt_s_lu, fcvt_s_lu)


// RV32D Standard Extension
INSTRUCTION(Fld, fld)
INSTRUCTION(Fsd, fsd)
INSTRUCTION(Fmadd_d, fmadd_d)
INSTRUCTION(Fmsub_d, fmsub_d)
INSTRUCTION(Fnmsub_d, fnmsub_d)
INSTRUCTION(Fnmadd_d, fnmadd_d)
INSTRUCTION(Fadd_d, fadd_d)
INSTRUCTION(Fsub_d, fsub_d)
INSTRUCTION(Fmul_d, fmul_d)
INSTRUCTION(Fdiv_d, fdiv_d)
INSTRUCTION(Fsqrt_d, fsqrt_d)
INSTRUCTION(Fsgnj_d, fsgnj_d)
INSTRUCTION(Fsgnjn_d, fsgnjn_d)
INSTRUCTION(Fsgnjx_d, fsgnjx_d)
INSTRUCTION(Fmin_d, fmin_d)
INSTRUCTION(Fmax_d, fmax_d)
INSTRUCTION(Fcvt_s_d, fcvt_s_d)
INSTRUCTION(Fcvt_d_s, fcvt_d_s)
INSTRUCTION(Feq_d, feq_d)
INSTRUCTION(Flt_d, flt_d)
INSTRUCTION(Fle_d, fle_d)
INSTRUCTION(Fclass_d, fclass_d)
INSTRUCTION(Fcvt_w_d, fcvt_w_d)
INSTRUCTION(Fcvt_wu_d, fcvt_wu_d)
INSTRUCTION(Fcvt_d_w, fcvt_d_w)
INSTRUCTION(Fcvt_d_wu, fcvt_d_wu)

// RV64D Standard Extension (in addition to RV32D)
INSTRUCTION(Fcvt_l_d, fcvt_l_d)
INSTRUCTION(Fcvt_lu_d, fcvt_lu_d)
INSTRUCTION(Fmv_x_d, fmv_x_d)
INSTRUCTION(Fcvt_d_l, fcvt_d_l)
INSTRUCTION(Fcvt_d_lu, fcvt_d_lu)
INSTRUCTION(Fmv_d_x, fmv_d_x)

// RV32Q Standard Extension
INSTRUCTION(Flq, flq)
INSTRUCTION(Fsq, fsq)
INSTRUCTION(Fmadd_q, fmadd_q)
INSTRUCTION(Fmsub_q, fmsub_q)
INSTRUCTION(Fnmsub_q, fnmsub_q)
INSTRUCTION(Fnmadd_q, fnmadd_q)
INSTRUCTION(Fadd_q, fadd_q)
INSTRUCTION(Fsub_q, fsub_q)
INSTRUCTION(Fmul_q, fmul_q)
INSTRUCTION(Fdiv_q, fdiv_q)
INSTRUCTION(Fsqrt_q, fsqrt_q)
INSTRUCTION(Fsgnj_q, fsgnj_q)
INSTRUCTION(Fsgnjn_q, fsgnjn_q)
INSTRUCTION(Fsgnjx_q, fsgnjx_q)
INSTRUCTION(Fmin_q, fmin_q)
INSTRUCTION(Fmax_q, fmax_q)
INSTRUCTION(Fcvt_s_q, fcvt_s_q)
INSTRUCTION(Fcvt_q_s, fcvt_q_s)
INSTRUCTION(Fcvt_d_q, fcvt_d_q)
INSTRUCTION(Fcvt_q_d, fcvt_q_d)
INSTRUCTION(Feq_q, feq_q)
INSTRUCTION(Flt_q, flt_q)
INSTRUCTION(Fle_q, fle_q)
INSTRUCTION(Fclass_q, fclass_q)
INSTRUCTION(Fcvt_w_q, fcvt_w_q)
INSTRUCTION(Fcvt_wu_q, fcvt_wu_q)
INSTRUCTION(Fcvt_q_w, fcvt_q_w)
INSTRUCTION(Fcvt_q_wu, fcvt_q_wu)

// RV64Q Standard Extension (in addition to RV32Q)
INSTRUCTION(Fcvt_l_q, fcvt_l_q)
INSTRUCTION(Fcvt_lu_q, fcvt_lu_q)
INSTRUCTION(Fcvt_q_l, fcvt_q_l)
INSTRUCTION(Fcvt_q_lu, fcvt_q_lu)
#endif