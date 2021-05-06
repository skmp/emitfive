// TPL({UnimplementedOpcodes::emitLabel1Unimplemented, UnimplementedOpcodes::emitLabel2Unimplemented, UnimplementedOpcodes::emitDstUnimplemented, UnimplementedOpcodes::emitDst2Unimplemented, UnimplementedOpcodes::emitImmUnimplemented, UnimplementedOpcodes::emitImm20Unimplemented}),
ENCODER_SPEC(R, 
    TPL({UnimplementedOpcodes::emitLabel1Unimplemented, UnimplementedOpcodes::emitLabel2Unimplemented, encoder, UnimplementedOpcodes::emitDst2Unimplemented, UnimplementedOpcodes::emitImmUnimplemented, UnimplementedOpcodes::emitImm20Unimplemented}),
    TPL(<uint32_t funct7, uint32_t funct3, uint32_t opcode>),
    TPL(<funct7, funct3, opcode>),
    (const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2),
    (rd, rs1, rs2),
    ((funct7 << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct3 << 12) | (rd.code << 7) | (opcode << 0))
)

ENCODER_SPEC(R4, 
    TPL({UnimplementedOpcodes::emitLabel1Unimplemented, UnimplementedOpcodes::emitLabel2Unimplemented, UnimplementedOpcodes::emitDstUnimplemented, encoder, UnimplementedOpcodes::emitImmUnimplemented, UnimplementedOpcodes::emitImm20Unimplemented}),
    TPL(<uint32_t funct3, uint32_t opcode>),
    TPL(<funct3, opcode>),
    (const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2, const RegisterGpr rs3, const uint32_t rm),
    (rd, rs1, rs2,rs3, rm),
    ((rs3.code << 27) | (rm << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct3 << 12) | (rd.code << 7) | (opcode << 0))
)

ENCODER_SPEC(RF,
    TPL({UnimplementedOpcodes::emitLabel1Unimplemented, UnimplementedOpcodes::emitLabel2Unimplemented, UnimplementedOpcodes::emitDstUnimplemented, encoder, UnimplementedOpcodes::emitImmUnimplemented, UnimplementedOpcodes::emitImm20Unimplemented}),
    TPL(<uint32_t funct7, uint32_t opcode>),
    TPL(<funct7, opcode>),
    (const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2, const RegisterGpr rs3, const uint32_t rm),
    (rd, rs1, rs2, rs3, rm),
    ((rs3.code << 27) | (rm << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct7 << 12) | (rd.code << 7) | (opcode << 0))
)

ENCODER_SPEC(R2F,
    TPL({UnimplementedOpcodes::emitLabel1Unimplemented, UnimplementedOpcodes::emitLabel2Unimplemented, UnimplementedOpcodes::emitDstUnimplemented, encoder, UnimplementedOpcodes::emitImmUnimplemented, UnimplementedOpcodes::emitImm20Unimplemented}),
    TPL(<uint32_t funct7, uint32_t funct5, uint32_t funct3, uint32_t opcode>),
    TPL(<funct7, funct5, funct3, opcode>),
    (const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2, const RegisterGpr rs3, const uint32_t rm),
    (rd, rs1, rs2, rs3, rm),
    ((rs3.code << 27) | (rm << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct3 << 12) | (rd.code << 7) | (opcode << 0))
)

ENCODER_SPEC(R3F,
    TPL({UnimplementedOpcodes::emitLabel1Unimplemented, UnimplementedOpcodes::emitLabel2Unimplemented, UnimplementedOpcodes::emitDstUnimplemented, encoder, UnimplementedOpcodes::emitImmUnimplemented, UnimplementedOpcodes::emitImm20Unimplemented}),
    TPL(<uint32_t funct7, uint32_t funct5, uint32_t opcode>),
    TPL(<funct7, funct5, opcode>),
    (const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2, const RegisterGpr rs3, const uint32_t rm),
    (rd, rs1, rs2, rs3, rm),
    ((rs3.code << 27) | (rm << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct5 << 12) | (rd.code << 7) | (opcode << 0))
)

ENCODER_SPEC(I,
    TPL({UnimplementedOpcodes::emitLabel1Unimplemented, UnimplementedOpcodes::emitLabel2Unimplemented, UnimplementedOpcodes::emitDstUnimplemented, UnimplementedOpcodes::emitDst2Unimplemented, encoder, UnimplementedOpcodes::emitImm20Unimplemented}),
    TPL(<uint32_t funct3, uint32_t opcode>),
    TPL(<funct3, opcode>),
    (const RegisterGpr rd, const RegisterGpr rs1, const uint32_t imm12),
    (rd, rs1, imm12),
    ((imm12 << 20) | (rs1.code << 15) | (funct3 << 12) | (rd.code << 7) | (opcode << 0))
)

ENCODER_SPEC(IS32,
    TPL({UnimplementedOpcodes::emitLabel1Unimplemented, UnimplementedOpcodes::emitLabel2Unimplemented, UnimplementedOpcodes::emitDstUnimplemented, UnimplementedOpcodes::emitDst2Unimplemented, encoder, UnimplementedOpcodes::emitImm20Unimplemented}),
    TPL(<uint32_t funct7, uint32_t funct3, uint32_t opcode>),
    TPL(<funct7, funct3, opcode>),
    (const RegisterGpr rd, const RegisterGpr rs1, const uint32_t imm5),
    (rd, rs1, imm5),
    ((funct7 << 25) | (imm5 << 20) | (rs1.code << 15) | (funct3 << 12) | (rd.code << 7) | (opcode << 0))
)

ENCODER_SPEC(IS64,
    TPL({UnimplementedOpcodes::emitLabel1Unimplemented, UnimplementedOpcodes::emitLabel2Unimplemented, UnimplementedOpcodes::emitDstUnimplemented, UnimplementedOpcodes::emitDst2Unimplemented, encoder, UnimplementedOpcodes::emitImm20Unimplemented}),
    TPL(<uint32_t funct6, uint32_t funct3, uint32_t opcode>),
    TPL(<funct6, funct3, opcode>),
    (const RegisterGpr rd, const RegisterGpr rs1, const uint32_t imm6),
    (rd, rs1, imm6),
    ((funct6 << 26) | (imm6 << 20) | (rs1.code << 15) | (funct3 << 12) | (rd.code << 7) | (opcode << 0))
)

ENCODER_SPEC(S,
    TPL({UnimplementedOpcodes::emitLabel1Unimplemented, UnimplementedOpcodes::emitLabel2Unimplemented, UnimplementedOpcodes::emitDstUnimplemented, UnimplementedOpcodes::emitDst2Unimplemented, encoder, UnimplementedOpcodes::emitImm20Unimplemented}),
    TPL(<uint32_t funct3, uint32_t opcode>),
    TPL(<funct3, opcode>),
    (const RegisterGpr rs1, const RegisterGpr rs2, const uint32_t imm7),
    (rs1, rs2, imm7),
    ((imm7 << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct3 << 12) | (imm7 << 7) | (opcode << 0))
)

ENCODER_SPEC(B,
    TPL({UnimplementedOpcodes::emitLabel1Unimplemented, encoder, UnimplementedOpcodes::emitDstUnimplemented, UnimplementedOpcodes::emitDst2Unimplemented, UnimplementedOpcodes::emitImmUnimplemented, UnimplementedOpcodes::emitImm20Unimplemented}),
    TPL(<uint32_t funct3, uint32_t opcode>),
    TPL(<funct3, opcode>),
    (const RegisterGpr rs1, const RegisterGpr rs2, const Label& destination),
    (rs1, rs2, destination),
    ((0 /*imm7*/ << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct3 << 12) | (0 /*imm7*/ << 7) | (opcode << 0))
)

ENCODER_SPEC(U,
    TPL({UnimplementedOpcodes::emitLabel1Unimplemented, UnimplementedOpcodes::emitLabel2Unimplemented, UnimplementedOpcodes::emitDstUnimplemented, UnimplementedOpcodes::emitDst2Unimplemented, UnimplementedOpcodes::emitImmUnimplemented, encoder}),
    TPL(<uint32_t opcode>),
    TPL(<opcode>),
    (const RegisterGpr rd, const uint32_t imm20),
    (rd, imm20),
    ((imm20 << 12) | (rd.code << 7) | (opcode << 0))
)

ENCODER_SPEC(J,
    TPL({encoder, UnimplementedOpcodes::emitLabel2Unimplemented, UnimplementedOpcodes::emitDstUnimplemented, UnimplementedOpcodes::emitDst2Unimplemented, UnimplementedOpcodes::emitImmUnimplemented, UnimplementedOpcodes::emitImm20Unimplemented}),
    TPL(<uint32_t opcode>),
    TPL(<opcode>),
    (const RegisterGpr rd, const Label& destination),
    (rd, destination),
    ((0 /*imm20*/ << 12) | (rd.code << 7) | (opcode << 0))
)