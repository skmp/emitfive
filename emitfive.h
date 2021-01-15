#include <cstdint>
#include <cassert>

namespace emitfive {
    enum RegisterClass {
        RC_INVALID = 0,
        RC_GPR,
        RC_FPR,
        RC_COUNT
    };

    enum RegisterSize {
        RS_INVALID = 0,
        RS_32,
        RS_64,
    };

    struct Register {
        const uint8_t code;
        const RegisterClass type: 3;
        const uint32_t size: 5;
    };

    struct RegisterGpr: Register {
        constexpr RegisterGpr(uint8_t code, uint32_t size): Register { code, RC_GPR, size } { }
        constexpr RegisterGpr(const RegisterGpr& other): Register { other.code, other.type, other.size } { }

        constexpr RegisterGpr w() const { return RegisterGpr { code, RS_32 }; }
        constexpr RegisterGpr d() const { return RegisterGpr { code, RS_64 }; }
    };

    struct RegisterFpr: Register {
        constexpr RegisterFpr(uint8_t code, uint32_t size): Register { code, RC_FPR, size } { }
        constexpr RegisterFpr(const RegisterFpr& other): Register { other.code, other.type, other.size } { }
    };

    struct Label {
        uintptr_t codePointer;
        bool bound;
        bool used;
        bool external;

        // Local Label
        Label() : bound(false), used(false), external(false) { }

        ~Label() { if (used && !bound) { assert("Bad Label Usage"); } }

        // External Ptr Label
        Label(uintptr_t externalCode, bool external = true) : bound(true), used(false), external(external), codePointer(externalCode) { }

        // External Function Helper
        template<typename R, typename ...Args>
        Label(R(*externalCode)(Args...), bool external = true) : Label(reinterpret_cast<uintptr_t>(externalCode), external) { }

        // External Member Function Helper
        template<typename R, typename C, typename ...Args>
        Label(R(C::*externalCode)(Args...), bool external = true) : Label(reinterpret_cast<uintptr_t&>(externalCode), external) { }
    };

    struct CodeBuffer {
        CodeBuffer(uint32_t* buffer): buffer(buffer) { }
        uint32_t* buffer;
        void Emit32(uint32_t opCode) {
            *buffer++ = opCode;
        }
    };

    namespace riscv64 {
        struct Encoder {
            CodeBuffer* const buffer;
            Encoder(CodeBuffer* buffer) : buffer(buffer) { }

            virtual void operator()(const Label& destination) const { assert("Unsupported encoding"); }
            virtual void operator()(const RegisterGpr src1, const RegisterGpr src2, const Label& destination) const { assert("Unsupported encoding"); }
            virtual void operator()(const RegisterGpr dst, const RegisterGpr src1, const RegisterGpr src2) const { assert("Unsupported encoding"); }

            virtual bool CanEncodeR() { return false; }
            virtual bool CanEncodeR4() { return false; }
            virtual bool CanEncodeI() { return false; }
            virtual bool CanEncodeS() { return false; }
            virtual bool CanEncodeB() { return false; }
            virtual bool CanEncodeU() { return false; }
            virtual bool CanEncodeJ() { return false; }

        };

#define ENCODER_SPEC(Type, Template, Arguments, Encode) \
        struct Encoder##Type: public Encoder { \
            using Encoder::Encoder; \
            virtual bool CanEncode##Type() { return true; } \
        };\
        template Template \
        struct Encoder##Type##Emit: public Encoder##Type { \
            using Encoder##Type::Encoder##Type; \
            virtual void operator() Arguments const { \
                buffer->Emit32 Encode; \
            } \
        };

#define TPL(...) __VA_ARGS__    

        ENCODER_SPEC(R,
            TPL(<uint32_t funct7, uint32_t funct3, uint32_t opcode>),
            (const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2),
            ((funct7 << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct3 << 12) | (rd.code << 7) | (opcode << 0))
        )

        ENCODER_SPEC(R4,
            TPL(<uint32_t funct3, uint32_t opcode>),
            (const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2, const RegisterGpr rs3, const uint32_t rm),
            ((rs3.code << 27) | (rm << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct3 << 12) | (rd.code << 7) | (opcode << 0))
        )

        ENCODER_SPEC(RF,
            TPL(<uint32_t funct7, uint32_t opcode>),
            (const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2, const RegisterGpr rs3, const uint32_t rm),
            ((rs3.code << 27) | (rm << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct7 << 12) | (rd.code << 7) | (opcode << 0))
        )

        ENCODER_SPEC(R2F,
            TPL(<uint32_t funct7, uint32_t funct5, uint32_t funct3, uint32_t opcode>),
            (const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2, const RegisterGpr rs3, const uint32_t rm),
            ((rs3.code << 27) | (rm << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct3 << 12) | (rd.code << 7) | (opcode << 0))
        )

        ENCODER_SPEC(R3F,
            TPL(<uint32_t funct7, uint32_t funct5, uint32_t opcode>),
            (const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2, const RegisterGpr rs3, const uint32_t rm),
            ((rs3.code << 27) | (rm << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct5 << 12) | (rd.code << 7) | (opcode << 0))
        )

        ENCODER_SPEC(I,
            TPL(<uint32_t funct3, uint32_t opcode>),
            (const RegisterGpr rd, const RegisterGpr rs1, const uint32_t imm12),
            ((imm12 << 20) | (rs1.code << 15) | (funct3 << 12) | (rd.code << 7) | (opcode << 0))
        )

        ENCODER_SPEC(IS32,
            TPL(<uint32_t funct7, uint32_t funct3, uint32_t opcode>),
            (const RegisterGpr rd, const RegisterGpr rs1, const uint32_t imm5),
            ((funct7 << 25) | (imm5 << 20) | (rs1.code << 15) | (funct3 << 12) | (rd.code << 7) | (opcode << 0))
        )

        ENCODER_SPEC(IS64,
            TPL(<uint32_t funct6, uint32_t funct3, uint32_t opcode>),
            (const RegisterGpr rd, const RegisterGpr rs1, const uint32_t imm6),
            ((funct6 << 26) | (imm6 << 20) | (rs1.code << 15) | (funct3 << 12) | (rd.code << 7) | (opcode << 0))
        )

        ENCODER_SPEC(S,
            TPL(<uint32_t funct3, uint32_t opcode>),
            (const RegisterGpr rs1, const RegisterGpr rs2, const uint32_t imm7),
            ((imm7 << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct3 << 12) | (imm7 << 7) | (opcode << 0))
        )

        ENCODER_SPEC(B,
            TPL(<uint32_t funct3, uint32_t opcode>),
            (const RegisterGpr rs1, const RegisterGpr rs2, const Label& destination),
            ((0 /*imm7*/ << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct3 << 12) | (0 /*imm7*/ << 7) | (opcode << 0))
        )

        ENCODER_SPEC(U,
            TPL(<uint32_t opcode>),
            (const RegisterGpr rd, const uint32_t imm20),
            ((imm20 << 12) | (rd.code << 7) | (opcode << 0))
        )

        ENCODER_SPEC(J,
            TPL(<uint32_t opcode>),
            (const RegisterGpr rd, const Label& destination),
            ((0 /*imm20*/ << 12) | (rd.code << 7) | (opcode << 0))
        )

#define INSTRUCTION(Name, name, Type, ...) \
        struct Name##Instruction final: public Encoder##Type##Emit<__VA_ARGS__> { using Encoder##Type##Emit::operator(); using Encoder##Type##Emit::Encoder##Type##Emit; };

#include "emitfive-riscv-insn.inl"

#undef INSTRUCTION

        struct Assembler {
            using Label = Label;
            using Encoder = Encoder;
            using EncoderR = EncoderR;
            using EncoderR4 = EncoderR4;
            using EncoderI = EncoderI;
            using EncoderS = EncoderS;
            using EncoderB = EncoderB;
            using EncoderU = EncoderU;
            using EncoderJ = EncoderJ;

            Assembler(uint32_t* dataBuffer) : buffer(dataBuffer) { }

            CodeBuffer buffer;
            #define REGISTER_DECL(n) \
                static constexpr const RegisterGpr x##n {n, RS_64}

            REGISTER_DECL(0); REGISTER_DECL(1); REGISTER_DECL(2); REGISTER_DECL(3);
            REGISTER_DECL(4); REGISTER_DECL(5); REGISTER_DECL(6); REGISTER_DECL(7);
            REGISTER_DECL(8); REGISTER_DECL(9); REGISTER_DECL(10);REGISTER_DECL(11);
            REGISTER_DECL(12); REGISTER_DECL(13); REGISTER_DECL(14); REGISTER_DECL(15);
            REGISTER_DECL(16); REGISTER_DECL(17); REGISTER_DECL(18); REGISTER_DECL(19);
            REGISTER_DECL(20); REGISTER_DECL(21); REGISTER_DECL(22); REGISTER_DECL(23);
            REGISTER_DECL(24); REGISTER_DECL(25); REGISTER_DECL(26); REGISTER_DECL(27);
            REGISTER_DECL(28); REGISTER_DECL(29); REGISTER_DECL(30); REGISTER_DECL(31);

            #undef REGISTER_DECL

            #define REGISTER_ALIAS(name, n) \
                static constexpr const RegisterGpr name = x##n

            REGISTER_ALIAS(zero, 0);
            REGISTER_ALIAS(ra, 1);
            REGISTER_ALIAS(sp, 2);
            REGISTER_ALIAS(gp, 3);
            REGISTER_ALIAS(tp, 4);
            REGISTER_ALIAS(t0, 5);
            REGISTER_ALIAS(t1, 6);
            REGISTER_ALIAS(t2, 7);
            REGISTER_ALIAS(s0, 8);
            REGISTER_ALIAS(fp, 8);
            REGISTER_ALIAS(s1, 9);
            REGISTER_ALIAS(a0, 10);
            REGISTER_ALIAS(r0, 10);
            REGISTER_ALIAS(a1, 11);
            REGISTER_ALIAS(r1, 11);
            REGISTER_ALIAS(a2, 12);
            REGISTER_ALIAS(a3, 13);
            REGISTER_ALIAS(a4, 14);
            REGISTER_ALIAS(a5, 15);
            REGISTER_ALIAS(a6, 16);
            REGISTER_ALIAS(a7, 17);
            REGISTER_ALIAS(s2, 18);
            REGISTER_ALIAS(s3, 19);
            REGISTER_ALIAS(s4, 20);
            REGISTER_ALIAS(s5, 21);
            REGISTER_ALIAS(s6, 22);
            REGISTER_ALIAS(s7, 23);
            REGISTER_ALIAS(s8, 24);
            REGISTER_ALIAS(s9, 25);
            REGISTER_ALIAS(s10, 26);
            REGISTER_ALIAS(s11, 27);
            REGISTER_ALIAS(t3, 28);
            REGISTER_ALIAS(t4, 29);
            REGISTER_ALIAS(t5, 30);
            REGISTER_ALIAS(t6, 31);

            #undef REGISTER_ALIAS

            #define REGISTER_DECL(n) \
                static constexpr const RegisterFpr f##n {n, RS_32}

            REGISTER_DECL(0); REGISTER_DECL(1); REGISTER_DECL(2); REGISTER_DECL(3);
            REGISTER_DECL(4); REGISTER_DECL(5); REGISTER_DECL(6); REGISTER_DECL(7);
            REGISTER_DECL(8); REGISTER_DECL(9); REGISTER_DECL(10);REGISTER_DECL(11);
            REGISTER_DECL(12); REGISTER_DECL(13); REGISTER_DECL(14); REGISTER_DECL(15);
            REGISTER_DECL(16); REGISTER_DECL(17); REGISTER_DECL(18); REGISTER_DECL(19);
            REGISTER_DECL(20); REGISTER_DECL(21); REGISTER_DECL(22); REGISTER_DECL(23);
            REGISTER_DECL(24); REGISTER_DECL(25); REGISTER_DECL(26); REGISTER_DECL(27);
            REGISTER_DECL(28); REGISTER_DECL(29); REGISTER_DECL(30); REGISTER_DECL(31);

            #undef REGISTER_DECL

            #define REGISTER_ALIAS(name, n) \
                static constexpr const RegisterFpr name = f##n

            REGISTER_ALIAS(ft0, 0);
            REGISTER_ALIAS(ft1, 1);
            REGISTER_ALIAS(ft2, 2);
            REGISTER_ALIAS(ft3, 3);
            REGISTER_ALIAS(ft4, 4);
            REGISTER_ALIAS(ft5, 5);
            REGISTER_ALIAS(ft6, 6);
            REGISTER_ALIAS(ft7, 7);
            REGISTER_ALIAS(fs0, 8);
            REGISTER_ALIAS(fs1, 9);
            REGISTER_ALIAS(fa0, 10);
            REGISTER_ALIAS(fr0, 10);
            REGISTER_ALIAS(fa1, 11);
            REGISTER_ALIAS(fr1, 11);
            REGISTER_ALIAS(fa2, 12);
            REGISTER_ALIAS(fa3, 13);
            REGISTER_ALIAS(fa4, 14);
            REGISTER_ALIAS(fa5, 15);
            REGISTER_ALIAS(fa6, 16);
            REGISTER_ALIAS(fa7, 17);
            REGISTER_ALIAS(fs2, 18);
            REGISTER_ALIAS(fs3, 19);
            REGISTER_ALIAS(fs4, 20);
            REGISTER_ALIAS(fs5, 21);
            REGISTER_ALIAS(fs6, 22);
            REGISTER_ALIAS(fs7, 23);
            REGISTER_ALIAS(fs8, 24);
            REGISTER_ALIAS(fs9, 25);
            REGISTER_ALIAS(fs10, 26);
            REGISTER_ALIAS(fs11, 27);
            REGISTER_ALIAS(ft8, 28);
            REGISTER_ALIAS(ft9, 29);
            REGISTER_ALIAS(ft10, 30);
            REGISTER_ALIAS(ft11, 31);

            #undef REGISTER_ALIAS

            void bind(Label& label) {
                assert(label.bound == false);
                label.bound = true;
            }

#define INSTRUCTION(Name, name, Type, ...) const Name##Instruction name { &buffer };

#include "emitfive-riscv-insn.inl"

#undef INSTRUCTION
        };


        struct RegisterGprExpression {

        };

        RegisterGprExpression operator+(RegisterGpr, int) { return RegisterGprExpression(); }
        RegisterGprExpression operator+(int, RegisterGpr) { return RegisterGprExpression(); }

        struct MemOperand {

        };

        struct MemSpecifier {
            MemOperand operator[](RegisterGpr) { return MemOperand(); }
            MemOperand operator[](RegisterGprExpression) { return MemOperand(); }
        };

        struct Operand {
            Operand(const int) { }
            Operand(const Register) { }
            Operand(const MemOperand) { }
        };

        struct MacroEncoder {
            Assembler* const assembler;
            MacroEncoder(Assembler* assembler) : assembler(assembler) { }

            virtual void operator()() const { assert(false); }
            virtual void operator()(const Operand) const { assert(false); }
            virtual void operator()(const Operand, const Operand) const { assert(false); }
            virtual void operator()(const Operand, const Operand, const Operand) const { assert(false); }
            virtual void operator()(const Operand, const Operand, const Operand, const Operand) const { assert(false); }
            virtual void operator()(const Operand, const Operand, const Operand, const Operand, const Operand) const { assert(false); }

            virtual void operator()(const RegisterGpr) { assert(false); }
            virtual void operator()(const RegisterGpr, int imm) { assert(false); }
            virtual void operator()(const RegisterGpr, const RegisterGpr) { assert(false); }
            virtual void operator()(const RegisterGpr, const RegisterGpr, int imm) { assert(false); }
            virtual void operator()(const RegisterGpr, const RegisterGpr, const RegisterGpr) { assert(false); }
            virtual void operator()(const RegisterGpr, const RegisterGpr, const RegisterGpr, const RegisterGpr) { assert(false); }
        };

        struct RetEncoder final: MacroEncoder {
            using MacroEncoder::MacroEncoder;

            virtual void operator()() const {
                assembler->jalr(assembler->x0, assembler->x1, 0);
            }
        };

        struct AddEncoder final: MacroEncoder {
            using MacroEncoder::MacroEncoder;

            virtual void operator()(const Operand, const Operand, const Operand) const  { assert(false); }

            virtual void operator()(const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2) const {
                if (rd.size == RS_64)
                    assembler->add(rd, rs1, rs2);
                else
                    assembler->addw(rd, rs1, rs2);
            }

            virtual void operator()(const RegisterGpr rd, const RegisterGpr rs1, const int imm) const {
                if (rd.size == RS_64)
                    assembler->addi(rd, rs1, imm);
                else
                    assembler->addiw(rd, rs1, imm);
            }
        };

        struct MacroAssembler: Assembler {
            MacroAssembler(uint32_t* code): Assembler(code) { }

            static constexpr MemSpecifier ptr{};
            static constexpr MemSpecifier b{};
            static constexpr MemSpecifier hw{};
            static constexpr MemSpecifier w{};
            static constexpr MemSpecifier qw{};

            const RetEncoder Ret{ (Assembler*)this };
            const AddEncoder Add{ (Assembler*)this };
        };
    }
}