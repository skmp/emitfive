#include <cstdint>
#include <cassert>
#include <functional>
#include <memory.h>
#include <type_traits>

template <typename R, typename... Args>
struct Caller;

template <typename T>
struct CallerM { };


template <typename R, typename... Args>
struct FuncD;

template <typename T, typename... Args>
struct Func;

template <typename T>
struct FuncS;

template <typename... Args>
struct DecomposeArgs;

template <typename R, typename... Args>
struct DecomposeFn;

template <typename Rt, typename... Argst>
struct DecomposeFn<Rt(Argst...)> {
    typedef Rt R;
    typedef DecomposeArgs<Argst...> Args;
};

template <typename T>
struct DecomposeMFn;

template <typename Ct, typename Rt, typename... Argst>
struct DecomposeMFn<Rt(Ct::*)(Argst...) const> {
    typedef Ct C;
    typedef Rt R;
    typedef DecomposeArgs<Argst...> Args;
};

template <typename R, typename... Args>
struct Caller<R(DecomposeArgs<Args...>)> {
    virtual R operator ()(Args... args) const { assert(false); };
};

struct BaseClass;

template <typename C, typename R, typename... Args>
struct CallerM<R(C::*)(DecomposeArgs<Args...>) const> {
    static const R Objectify(const void* that, Args... args) {
        return reinterpret_cast<const C*>(that)->operator()(args...);
    }
};

template <typename C, typename R, typename... Args>
struct CallerM<R(C::*)(Args...) const> : CallerM<R(C::*)(DecomposeArgs<Args...>) const> {

};

template <typename R, typename... Args>
struct FuncD<R(Args...)> {
    union {
        uint8_t Storage[32];
    };

    const R(*Objectifier)(const void* that, Args... args);

    template<typename T>
    inline FuncD(const FuncS<T>& Fn) : Objectifier(&FuncS<T>::VTableType::Objectify) { 
        static_assert( sizeof(Fn.Fn) < sizeof(Storage));
        memcpy(Storage, &Fn.Fn, sizeof(Fn.Fn)); 
    }

    inline R operator ()(Args... args) const {
        return Objectifier(&Storage[0], args...);
    }
};

template <typename T, typename... Argst>
struct Func<T, DecomposeArgs<Argst...>>: CallerM<decltype(&T::operator())>  {
    typedef CallerM<decltype(&T::operator())> VTableType;
    typedef typename DecomposeMFn<decltype(&T::operator())>::R R;
    typedef typename DecomposeMFn<decltype(&T::operator())>::Args Args;

    inline static constexpr const size_t Size = sizeof(T);

    const T Fn;

    Func(T Fn) : Fn(Fn) { }

    inline virtual R operator ()(Argst... args) {
        return Fn(args...);
    }

    inline operator const FuncD<R, Argst...>&() {
        return FuncD<R, Argst...>();
    }
};

template <typename T>
struct FuncS: Func<T, typename DecomposeMFn<decltype(&T::operator())>::Args>{
    FuncS(T Fn) : Func<T, typename DecomposeMFn<decltype(&T::operator())>::Args>(Fn) { }
    typedef typename Func<T, typename DecomposeMFn<decltype(&T::operator())>::Args>::VTableType VTableType;
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
        inline constexpr RegisterGpr(uint8_t code, uint32_t size): Register { code, RC_GPR, size } { }
        inline constexpr RegisterGpr(const RegisterGpr& other): Register { other.code, other.type, other.size } { }

        inline constexpr RegisterGpr w() const { return RegisterGpr { code, RS_32 }; }
        inline constexpr RegisterGpr d() const { return RegisterGpr { code, RS_64 }; }
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

    struct EncoderContext {
        CodeBuffer* buffer;

        EncoderContext(CodeBuffer* buffer): buffer(buffer) { }
    };

    namespace riscv64 {

       
        struct Encoder {
            const FuncD<void(const RegisterGpr dst, const Label& destination)> emitLabel1;
            const FuncD<void(const RegisterGpr src1, const RegisterGpr src2, const Label& destination)> emitLabel2;
            const FuncD<void(const RegisterGpr dst, const RegisterGpr src1, const RegisterGpr src2)> emitDst;
            const FuncD<void(const RegisterGpr dst, const RegisterGpr src1, const RegisterGpr src2, const RegisterGpr src3, const uint32_t rm)> emitDst2;
            const FuncD<void(const RegisterGpr r1, const RegisterGpr r2, const uint32_t imm)> emitImm;
            const FuncD<void(const RegisterGpr rd, const uint32_t imm20)> emitImm20;
            const FuncD<bool()> CanEncodeR, CanEncodeR4, CanEncodeRF, CanEncodeR2F, CanEncodeR3F, CanEncodeI, CanEncodeIS32, CanEncodeIS64, CanEncodeS, CanEncodeB, CanEncodeU, CanEncodeJ;

            #define Template(Name, Arguments) \
                typename T##Name

            #define PARAM(Name, Arguments) \
                const FuncS<T##Name>* Name

            #define DEFAULT_DECL(Name, Arguments) \
                inline static const auto Name##Unimplemented = FuncS([] Arguments { \
                    printf("Invalid Encoding: " #Name ", " #Arguments "\n"); \
                });

            #define DEFAULT(Name, Arguments) \
                Name(*Name)
            
            DEFAULT_DECL(emitLabel1, (const RegisterGpr dst, const Label& destination))
            DEFAULT_DECL(emitLabel2, (const RegisterGpr src1, const RegisterGpr src2, const Label& destination))
            DEFAULT_DECL(emitDst, (const RegisterGpr dst, const RegisterGpr src1, const RegisterGpr src2))
            DEFAULT_DECL(emitDst2, (const RegisterGpr dst, const RegisterGpr src1, const RegisterGpr src2, const RegisterGpr src3, const uint32_t rm))
            DEFAULT_DECL(emitImm, (const RegisterGpr r1, const RegisterGpr r2, const uint32_t imm))
            DEFAULT_DECL(emitImm20, (const RegisterGpr rd, const uint32_t imm20))

            template<
                typename TemitLabel1,
                typename TemitLabel2,
                typename TemitDst,
                typename TemitDst2,
                typename TemitImm,
                typename TemitImm20
            >
            inline Encoder(
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
                DEFAULT(emitImm20, (const RegisterGpr rd, const uint32_t imm20)),

                CanEncodeR(FuncS([]{ return false; })),
                CanEncodeR4(FuncS([]{ return false; })),
                CanEncodeRF(FuncS([]{ return false; })),
                CanEncodeR2F(FuncS([]{ return false; })),
                CanEncodeR3F(FuncS([]{ return false; })),
                CanEncodeI(FuncS([]{ return false; })),
                CanEncodeIS32(FuncS([]{ return false; })),
                CanEncodeIS64(FuncS([]{ return false; })),
                CanEncodeS(FuncS([]{ return false; })),
                CanEncodeB(FuncS([]{ return false; })),
                CanEncodeU(FuncS([]{ return false; })),
                CanEncodeJ(FuncS([]{ return false; }))
            {
               
            }

            inline ~Encoder() { }

            inline void operator()(const RegisterGpr dst, const Label& destination) const { emitLabel1(dst, destination); }
            inline void operator()(const RegisterGpr src1, const RegisterGpr src2, const Label& destination) const { emitLabel2(src1, src2, destination); }
            inline void operator()(const RegisterGpr dst, const RegisterGpr src1, const RegisterGpr src2) const { emitDst(dst, src1, src2); }
            inline void operator()(const RegisterGpr dst, const RegisterGpr src1, const RegisterGpr src2, const RegisterGpr src3, const uint32_t rm) const { emitDst2(dst, src1, src2, src3, rm); }
            inline void operator()(const RegisterGpr r1, const RegisterGpr r2, const uint32_t imm) const { emitImm(r1, r2, imm); }        
            inline void operator()(const RegisterGpr rd, const uint32_t imm20) const { emitImm20(rd, imm20); }            
        };

#define ENCODER_SPEC(Type, GenericEncoderFunction, Template, Instantiation, Arguments, Values, Encode) \
        struct Encoder##Type { \
            const FuncD<void Arguments> encoder; \
            inline void operator() Arguments const { encoder Values; }\
            inline operator Encoder() const { return std::move(Encoder(&Encoder::emitLabel1Unimplemented, &Encoder::emitLabel2Unimplemented, &Encoder::emitDstUnimplemented, &Encoder::emitDst2Unimplemented, &Encoder::emitImmUnimplemented, &Encoder::emitImm20Unimplemented )); } \
            inline Encoder##Type(FuncD<void Arguments> encoder) : encoder(encoder) { } \
        }; \
        template Template \
        struct Emit##Type: EncoderContext { \
            inline void operator() Arguments const { \
                buffer->Emit32 Encode; \
            } \
            inline operator Encoder##Type() const  { auto Buffer = buffer; return std::move(Encoder##Type(FuncS([Buffer] Arguments { Buffer->Emit32 Encode; }))); } \
            inline operator Encoder() const { auto Buffer = buffer; return std::move(Encoder##Type(FuncS([Buffer] Arguments { Buffer->Emit32 Encode; }))); } \
        };

#define TPL(...) __VA_ARGS__    

        ENCODER_SPEC(R, emitDst,
            TPL(<uint32_t funct7, uint32_t funct3, uint32_t opcode>),
            TPL(<funct7, funct3, opcode>),
            (const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2),
            (rd, rs1, rs2),
            ((funct7 << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct3 << 12) | (rd.code << 7) | (opcode << 0))
        )

        ENCODER_SPEC(R4, emitDst2,
            TPL(<uint32_t funct3, uint32_t opcode>),
            TPL(<funct3, opcode>),
            (const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2, const RegisterGpr rs3, const uint32_t rm),
            (rd, rs1, rs2,rs3, rm),
            ((rs3.code << 27) | (rm << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct3 << 12) | (rd.code << 7) | (opcode << 0))
        )

        ENCODER_SPEC(RF, emitDst2,
            TPL(<uint32_t funct7, uint32_t opcode>),
            TPL(<funct7, opcode>),
            (const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2, const RegisterGpr rs3, const uint32_t rm),
            (rd, rs1, rs2, rs3, rm),
            ((rs3.code << 27) | (rm << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct7 << 12) | (rd.code << 7) | (opcode << 0))
        )

        ENCODER_SPEC(R2F, emitDst2,
            TPL(<uint32_t funct7, uint32_t funct5, uint32_t funct3, uint32_t opcode>),
            TPL(<funct7, funct5, funct3, opcode>),
            (const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2, const RegisterGpr rs3, const uint32_t rm),
            (rd, rs1, rs2, rs3, rm),
            ((rs3.code << 27) | (rm << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct3 << 12) | (rd.code << 7) | (opcode << 0))
        )

        ENCODER_SPEC(R3F, emitDst2,
            TPL(<uint32_t funct7, uint32_t funct5, uint32_t opcode>),
            TPL(<funct7, funct5, opcode>),
            (const RegisterGpr rd, const RegisterGpr rs1, const RegisterGpr rs2, const RegisterGpr rs3, const uint32_t rm),
            (rd, rs1, rs2, rs3, rm),
            ((rs3.code << 27) | (rm << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct5 << 12) | (rd.code << 7) | (opcode << 0))
        )

        ENCODER_SPEC(I, emitImm,
            TPL(<uint32_t funct3, uint32_t opcode>),
            TPL(<funct3, opcode>),
            (const RegisterGpr rd, const RegisterGpr rs1, const uint32_t imm12),
            (rd, rs1, imm12),
            ((imm12 << 20) | (rs1.code << 15) | (funct3 << 12) | (rd.code << 7) | (opcode << 0))
        )

        ENCODER_SPEC(IS32, emitImm,
            TPL(<uint32_t funct7, uint32_t funct3, uint32_t opcode>),
            TPL(<funct7, funct3, opcode>),
            (const RegisterGpr rd, const RegisterGpr rs1, const uint32_t imm5),
            (rd, rs1, imm5),
            ((funct7 << 25) | (imm5 << 20) | (rs1.code << 15) | (funct3 << 12) | (rd.code << 7) | (opcode << 0))
        )

        ENCODER_SPEC(IS64, emitImm,
            TPL(<uint32_t funct6, uint32_t funct3, uint32_t opcode>),
            TPL(<funct6, funct3, opcode>),
            (const RegisterGpr rd, const RegisterGpr rs1, const uint32_t imm6),
            (rd, rs1, imm6),
            ((funct6 << 26) | (imm6 << 20) | (rs1.code << 15) | (funct3 << 12) | (rd.code << 7) | (opcode << 0))
        )

        ENCODER_SPEC(S, emitImm,
            TPL(<uint32_t funct3, uint32_t opcode>),
            TPL(<funct3, opcode>),
            (const RegisterGpr rs1, const RegisterGpr rs2, const uint32_t imm7),
            (rs1, rs2, imm7),
            ((imm7 << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct3 << 12) | (imm7 << 7) | (opcode << 0))
        )

        ENCODER_SPEC(B, emitLabel2,
            TPL(<uint32_t funct3, uint32_t opcode>),
            TPL(<funct3, opcode>),
            (const RegisterGpr rs1, const RegisterGpr rs2, const Label& destination),
            (rs1, rs2, destination),
            ((0 /*imm7*/ << 25) | (rs2.code << 20) | (rs1.code << 15) | (funct3 << 12) | (0 /*imm7*/ << 7) | (opcode << 0))
        )

        ENCODER_SPEC(U, emitImm20,
            TPL(<uint32_t opcode>),
            TPL(<opcode>),
            (const RegisterGpr rd, const uint32_t imm20),
            (rd, imm20),
            ((imm20 << 12) | (rd.code << 7) | (opcode << 0))
        )

        ENCODER_SPEC(J, emitLabel1,
            TPL(<uint32_t opcode>),
            TPL(<opcode>),
            (const RegisterGpr rd, const Label& destination),
            (rd, destination),
            ((0 /*imm20*/ << 12) | (rd.code << 7) | (opcode << 0))
        )

#define INSTRUCTION(Name, name, Type, ...) \
        struct Name##Instruction final: public Emit##Type<__VA_ARGS__> { using Emit##Type::operator();  using Emit##Type::operator Encoder; using Emit##Type::operator Encoder##Type; using Emit##Type::Emit##Type; };

#include "emitfive-riscv-insn.inl"

#undef INSTRUCTION

        struct Assembler {

            Assembler(uint32_t* dataBuffer) { Context.buffer = new CodeBuffer(dataBuffer); }
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

            union {
                EncoderContext Context;
#define INSTRUCTION(Name, name, Type, ...) const Name##Instruction name;

#include "emitfive-riscv-insn.inl"

#undef INSTRUCTION
            };
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