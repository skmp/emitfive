#include <cstdint>
#include <cassert>
#include <functional>
#include <memory.h>
#include <type_traits>
#include <bit>

#define always_inline __attribute__((always_inline)) 

struct InvalidEncodingUsedMessage {
    const char* message;
    constexpr InvalidEncodingUsedMessage(const char message[]) : message(message) { }
};

template<typename... Args>
struct InvalidEncoderUsed;

template<typename... Args>
struct InvalidEncoderUsed<void(Args...)>: InvalidEncodingUsedMessage {
    using InvalidEncodingUsedMessage::InvalidEncodingUsedMessage;
};

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
        always_inline constexpr RegisterGpr(uint8_t code, uint32_t size): Register { code, RC_GPR, size } { }
        always_inline constexpr RegisterGpr(const RegisterGpr& other): Register { other.code, other.type, other.size } { }

        always_inline constexpr RegisterGpr w() const { return RegisterGpr { code, RS_32 }; }
        always_inline constexpr RegisterGpr d() const { return RegisterGpr { code, RS_64 }; }
    };

    struct RegisterFpr: Register {
        always_inline constexpr RegisterFpr(uint8_t code, uint32_t size): Register { code, RC_FPR, size } { }
        always_inline constexpr RegisterFpr(const RegisterFpr& other): Register { other.code, other.type, other.size } { }
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

    struct EncoderContext {
        CodeBuffer* const buffer;

        EncoderContext(CodeBuffer* const buffer): buffer(buffer) { }
    };

    namespace riscv64 {

       
        struct EncoderFunctions {
            void(* const emitLabel1)(const EncoderContext* context, const RegisterGpr dst, const Label& destination);
            void(* const emitLabel2)(const EncoderContext* context, const RegisterGpr src1, const RegisterGpr src2, const Label& destination);
            void(* const emitDst)(const EncoderContext* context, const RegisterGpr dst, const RegisterGpr src1, const RegisterGpr src2);
            void(* const emitDst2)(const EncoderContext* context, const RegisterGpr dst, const RegisterGpr src1, const RegisterGpr src2, const RegisterGpr src3, const uint32_t rm);
            void(* const emitImm)(const EncoderContext* context, const RegisterGpr r1, const RegisterGpr r2, const uint32_t imm);
            void(* const emitImm20)(const EncoderContext* context, const RegisterGpr rd, const uint32_t imm20);

            #define PARAM(Name, Arguments) \
                const decltype(Name) & Name


            #define DEFAULT(Name, Arguments) \
                Name(Name)

            inline constexpr EncoderFunctions(
                PARAM(emitLabel1, (const RegisterGpr dst, const Label& destination)),
                PARAM(emitLabel2, (const RegisterGpr src1, const RegisterGpr src2, const Label& destination)),
                PARAM(emitDst, (const RegisterGpr dst, const RegisterGpr src1, const RegisterGpr src2)),
                PARAM(emitDst2, (const RegisterGpr dst, const RegisterGpr src1, const RegisterGpr src2, const RegisterGpr src3, const uint32_t rm)),
                PARAM(emitImm, (const RegisterGpr r1, const RegisterGpr r2, const uint32_t imm)),
                PARAM(emitImm20, (const RegisterGpr rd, const uint32_t imm20))
            ) : 

                DEFAULT(emitLabel1, (const RegisterGpr dst, const Label& destination)),
                DEFAULT(emitLabel2, (const RegisterGpr src1, const RegisterGpr src2, const Label& destination)),
                DEFAULT(emitDst, (const RegisterGpr dst, const RegisterGpr src1, const RegisterGpr src2)),
                DEFAULT(emitDst2, (const RegisterGpr dst, const RegisterGpr src1, const RegisterGpr src2, const RegisterGpr src3, const uint32_t rm)),
                DEFAULT(emitImm, (const RegisterGpr r1, const RegisterGpr r2, const uint32_t imm)),
                DEFAULT(emitImm20, (const RegisterGpr rd, const uint32_t imm20))
            {
               
            }
        };

        struct Encoder {
            const EncoderFunctions* const encoder;
            const EncoderContext* const context;
            constexpr Encoder(const EncoderFunctions* const encoder, const EncoderContext* const context) : encoder(encoder), context(context) { }

            always_inline constexpr void operator()(const RegisterGpr dst, const Label& destination) const { encoder->emitLabel1(context, dst, destination); }
            always_inline constexpr void operator()(const RegisterGpr src1, const RegisterGpr src2, const Label& destination) const { encoder->emitLabel2(context, src1, src2, destination); }
            always_inline constexpr void operator()(const RegisterGpr dst, const RegisterGpr src1, const RegisterGpr src2) const { encoder->emitDst(context, dst, src1, src2); }
            always_inline constexpr void operator()(const RegisterGpr dst, const RegisterGpr src1, const RegisterGpr src2, const RegisterGpr src3, const uint32_t rm) const { encoder->emitDst2(context, dst, src1, src2, src3, rm); }
            always_inline constexpr void operator()(const RegisterGpr r1, const RegisterGpr r2, const uint32_t imm) const { encoder->emitImm(context, r1, r2, imm); }
            always_inline constexpr void operator()(const RegisterGpr rd, const uint32_t imm20) const { encoder->emitImm20(context, rd, imm20); }
        };

        #define TPL(...) __VA_ARGS__


        struct UnimplementedOpcodes {
            #define DEFAULT_DECL(Name, ContextArguments, Arguments) \
            static constexpr decltype(EncoderFunctions::Name) Name##Unimplemented = [] ContextArguments { \
                throw InvalidEncoderUsed<void Arguments> { "Invalid Encoder: " #Name ", " #Arguments }; \
            };

            
            DEFAULT_DECL(emitLabel1, (const EncoderContext* const context, const RegisterGpr dst, const Label& destination), (const RegisterGpr dst, const Label& destination))
            DEFAULT_DECL(emitLabel2, (const EncoderContext* const context, const RegisterGpr src1, const RegisterGpr src2, const Label& destination), (const RegisterGpr src1, const RegisterGpr src2, const Label& destination))
            DEFAULT_DECL(emitDst, (const EncoderContext* const context, const RegisterGpr dst, const RegisterGpr src1, const RegisterGpr src2), (const RegisterGpr dst, const RegisterGpr src1, const RegisterGpr src2))
            DEFAULT_DECL(emitDst2, (const EncoderContext* const context, const RegisterGpr dst, const RegisterGpr src1, const RegisterGpr src2, const RegisterGpr src3, const uint32_t rm), (const RegisterGpr dst, const RegisterGpr src1, const RegisterGpr src2, const RegisterGpr src3, const uint32_t rm))
            DEFAULT_DECL(emitImm, (const EncoderContext* const context, const RegisterGpr r1, const RegisterGpr r2, const uint32_t imm), (const RegisterGpr r1, const RegisterGpr r2, const uint32_t imm))
            DEFAULT_DECL(emitImm20, (const EncoderContext* const context, const RegisterGpr rd, const uint32_t imm20), (const RegisterGpr rd, const uint32_t imm20))
        };


#define ENCODER_SPEC(Type, GenericEncoderFunction, Template, Instantiation, Arguments, ContextArguments, Values, Encode) \
        struct Encoder##Type { \
            const EncoderContext* const context; \
            const EncoderFunctions * const GenericEncoderFunctions; \
            void(*const encoder) ContextArguments; \
            always_inline constexpr void operator() Arguments const { encoder Values; }\
            always_inline constexpr operator const Encoder() const { return { GenericEncoderFunctions, context }; } \
            always_inline constexpr Encoder##Type(decltype(context) context, decltype(GenericEncoderFunctions) GenericEncoderFunctions, decltype(encoder) encoder) : context(context), GenericEncoderFunctions(GenericEncoderFunctions), encoder(encoder) { } \
        }; \
        template Template \
        struct Emit##Type: EncoderContext { \
            always_inline void operator() Arguments const { buffer->Emit32 Encode; } \
            always_inline static void Emit ContextArguments { context->buffer->Emit32 Encode; } \
            static constexpr EncoderFunctions GenericEncoderFunctions GenericEncoderFunction; \
            always_inline constexpr operator const Encoder##Type() const { return Encoder##Type(this, &GenericEncoderFunctions, &Emit); } \
            always_inline constexpr operator const Encoder() const { return Encoder##Type(this, &GenericEncoderFunctions, &Emit); } \
        };

#include "emitfive-riscv-enc.inl"

        struct Assembler {

            #define INSTRUCTION(Name, name, Type, ...) \
                struct Name##Instruction final: public Emit##Type<__VA_ARGS__> { using Emit##Type::operator();  using Emit##Type::operator const Encoder; using Emit##Type::operator const Encoder##Type; using Emit##Type::Emit##Type; };

            #include "emitfive-riscv-insn.inl"

            #undef INSTRUCTION
            union {
                EncoderContext Context;
#define INSTRUCTION(Name, name, Type, ...) const Name##Instruction name;

#include "emitfive-riscv-insn.inl"

#undef INSTRUCTION
            };


            Assembler(uint32_t* dataBuffer) : Context { new CodeBuffer(dataBuffer) } {  }
            ~Assembler() { delete Context.buffer; }

            #define REGISTER_DECL(n) \
                inline static constexpr const RegisterGpr x##n {n, RS_64}

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
                inline static constexpr const RegisterGpr name = x##n

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
                inline static constexpr const RegisterFpr f##n {n, RS_32}

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
                inline static constexpr const RegisterFpr name = f##n

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
        };

        static_assert(offsetof(Assembler, Context) == 0);


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

            inline static constexpr MemSpecifier ptr{};
            inline static constexpr MemSpecifier b{};
            inline static constexpr MemSpecifier hw{};
            inline static constexpr MemSpecifier w{};
            inline static constexpr MemSpecifier qw{};

            const RetEncoder Ret{ (Assembler*)this };
            const AddEncoder Add{ (Assembler*)this };
        };
        }
}