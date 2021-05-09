#define MINE(Name) &Emit##Name::Emit
#define emitDst(Name) TPL({UnimplementedOpcodes::emitLabel1Unimplemented, UnimplementedOpcodes::emitLabel2Unimplemented, MINE(Name), UnimplementedOpcodes::emitDst2Unimplemented, UnimplementedOpcodes::emitImmUnimplemented, UnimplementedOpcodes::emitImm20Unimplemented})
#define emitDst2(Name) TPL({UnimplementedOpcodes::emitLabel1Unimplemented, UnimplementedOpcodes::emitLabel2Unimplemented, UnimplementedOpcodes::emitDstUnimplemented, MINE(Name), UnimplementedOpcodes::emitImmUnimplemented, UnimplementedOpcodes::emitImm20Unimplemented})
#define emitImm(Name) TPL({UnimplementedOpcodes::emitLabel1Unimplemented, UnimplementedOpcodes::emitLabel2Unimplemented, UnimplementedOpcodes::emitDstUnimplemented, UnimplementedOpcodes::emitDst2Unimplemented, MINE(Name), UnimplementedOpcodes::emitImm20Unimplemented})
#define emitImm20(Name) TPL({UnimplementedOpcodes::emitLabel1Unimplemented, UnimplementedOpcodes::emitLabel2Unimplemented, UnimplementedOpcodes::emitDstUnimplemented, UnimplementedOpcodes::emitDst2Unimplemented, UnimplementedOpcodes::emitImmUnimplemented, MINE(Name)})
#define emitLabel1(Name) TPL({MINE(Name), UnimplementedOpcodes::emitLabel2Unimplemented, UnimplementedOpcodes::emitDstUnimplemented, UnimplementedOpcodes::emitDst2Unimplemented, UnimplementedOpcodes::emitImmUnimplemented, UnimplementedOpcodes::emitImm20Unimplemented})
#define emitLabel2(Name) TPL({UnimplementedOpcodes::emitLabel1Unimplemented, MINE(Name), UnimplementedOpcodes::emitDstUnimplemented, UnimplementedOpcodes::emitDst2Unimplemented, UnimplementedOpcodes::emitImmUnimplemented, UnimplementedOpcodes::emitImm20Unimplemented})
#define CONTEXT const EncoderContext* const context

ENCODER_SPEC(R, emitDst(R),
    TPL(<uint32_t funct7, uint32_t funct3, uint32_t opcode>),
    TPL(<funct7, funct3, opcode>),
    (const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2),
    (CONTEXT, const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2),
    (context, rd, rs1, rs2),
    ((funct7 << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct3 << 12) | (rd.code << 7) | (opcode << 0))
)

ENCODER_SPEC(R4, emitDst2(R4),
    TPL(<uint32_t funct3, uint32_t opcode>),
    TPL(<funct3, opcode>),
    (const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2, const RegisterGpr rs3, const uint32_t rm),
    (CONTEXT, const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2, const RegisterGpr rs3, const uint32_t rm),
    (context, rd, rs1, rs2,rs3, rm),
    ((rs3.code << 27) | (rm << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct3 << 12) | (rd.code << 7) | (opcode << 0))
)

ENCODER_SPEC(RF, emitDst2(RF),
    TPL(<uint32_t funct7, uint32_t opcode>),
    TPL(<funct7, opcode>),
    (const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2, const RegisterGpr rs3, const uint32_t rm),
    (CONTEXT, const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2, const RegisterGpr rs3, const uint32_t rm),
    (context, rd, rs1, rs2, rs3, rm),
    ((rs3.code << 27) | (rm << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct7 << 12) | (rd.code << 7) | (opcode << 0))
)

ENCODER_SPEC(R2F, emitDst2(R2F),
    TPL(<uint32_t funct7, uint32_t funct5, uint32_t funct3, uint32_t opcode>),
    TPL(<funct7, funct5, funct3, opcode>),
    (const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2, const RegisterGpr rs3, const uint32_t rm),
    (CONTEXT, const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2, const RegisterGpr rs3, const uint32_t rm),
    (context, rd, rs1, rs2, rs3, rm),
    ((rs3.code << 27) | (rm << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct3 << 12) | (rd.code << 7) | (opcode << 0))
)

ENCODER_SPEC(R3F, emitDst2(R3F),
    TPL(<uint32_t funct7, uint32_t funct5, uint32_t opcode>),
    TPL(<funct7, funct5, opcode>),
    (const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2, const RegisterGpr rs3, const uint32_t rm),
    (CONTEXT, const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2, const RegisterGpr rs3, const uint32_t rm),
    (context, rd, rs1, rs2, rs3, rm),
    ((rs3.code << 27) | (rm << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct5 << 12) | (rd.code << 7) | (opcode << 0))
)

ENCODER_SPEC(I, emitImm(I),
    TPL(<uint32_t funct3, uint32_t opcode>),
    TPL(<funct3, opcode>),
    (const RegisterGpr rd, const RegisterGpr rs1, const uint32_t imm12),
    (CONTEXT, const RegisterGpr rd, const RegisterGpr rs1, const uint32_t imm12),
    (context, rd, rs1, imm12),
    ((imm12 << 20) | (rs1.code << 15) | (funct3 << 12) | (rd.code << 7) | (opcode << 0))
)

ENCODER_SPEC(IS32, emitImm(IS32),
    TPL(<uint32_t funct7, uint32_t funct3, uint32_t opcode>),
    TPL(<funct7, funct3, opcode>),
    (const RegisterGpr rd, const RegisterGpr rs1, const uint32_t imm5),
    (CONTEXT, const RegisterGpr rd, const RegisterGpr rs1, const uint32_t imm5),
    (context, rd, rs1, imm5),
    ((funct7 << 25) | (imm5 << 20) | (rs1.code << 15) | (funct3 << 12) | (rd.code << 7) | (opcode << 0))
)

ENCODER_SPEC(IS64, emitImm(IS64),
    TPL(<uint32_t funct6, uint32_t funct3, uint32_t opcode>),
    TPL(<funct6, funct3, opcode>),
    (const RegisterGpr rd, const RegisterGpr rs1, const uint32_t imm6),
    (CONTEXT, const RegisterGpr rd, const RegisterGpr rs1, const uint32_t imm6),
    (context, rd, rs1, imm6),
    ((funct6 << 26) | (imm6 << 20) | (rs1.code << 15) | (funct3 << 12) | (rd.code << 7) | (opcode << 0))
)

ENCODER_SPEC(S, emitImm(S),
    TPL(<uint32_t funct3, uint32_t opcode>),
    TPL(<funct3, opcode>),
    (const RegisterGpr rs1, const RegisterGpr rs2, const uint32_t imm7),
    (CONTEXT, const RegisterGpr rs1, const RegisterGpr rs2, const uint32_t imm7),
    (context, rs1, rs2, imm7),
    ((imm7 << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct3 << 12) | (imm7 << 7) | (opcode << 0))
)

ENCODER_SPEC(B, emitLabel2(B),
    TPL(<uint32_t funct3, uint32_t opcode>),
    TPL(<funct3, opcode>),
    (const RegisterGpr rs1, const RegisterGpr rs2, const Label& destination),
    (CONTEXT, const RegisterGpr rs1, const RegisterGpr rs2, const Label& destination),
    (context, rs1, rs2, destination),
    ((0 /*imm7*/ << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct3 << 12) | (0 /*imm7*/ << 7) | (opcode << 0))
)

ENCODER_SPEC(U, emitImm20(U),
    TPL(<uint32_t opcode>),
    TPL(<opcode>),
    (const RegisterGpr rd, const uint32_t imm20),
    (CONTEXT, const RegisterGpr rd, const uint32_t imm20),
    (context, rd, imm20),
    ((imm20 << 12) | (rd.code << 7) | (opcode << 0))
)

ENCODER_SPEC(J, emitLabel1(J),
    TPL(<uint32_t opcode>),
    TPL(<opcode>),
    (const RegisterGpr rd, const Label& destination),
    (CONTEXT, const RegisterGpr rd, const Label& destination),
    (context, rd, destination),
    ((0 /*imm20*/ << 12) | (rd.code << 7) | (opcode << 0))
)

#undef CONTEXT
#undef emitDst
#undef emitDst2
#undef emitImm
#undef emitImm20
#undef emitLabel1
#undef emitLabel2
#undef MINE