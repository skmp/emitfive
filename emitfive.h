#include <cstdint>
#include <cassert>

namespace emitfive {
    enum RegisterClass {
        RC_INVALID = 0,
        RC_GPR,
        RC_FPU,
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

        
        struct EncoderGeneric {
            CodeBuffer* const buffer;
            EncoderGeneric(CodeBuffer* buffer) : buffer(buffer) { }

            virtual void operator()(const Label& destination) const { assert("Unsupported encoding"); }
            virtual void operator()(const Register src1, const Register src2, const Label& destination) const { assert("Unsupported encoding"); }
            virtual void operator()(const Register dst, const Register src1, const Register src2) const { assert("Unsupported encoding"); }

            virtual bool CanEncodeJ() { return false; }
            virtual bool CanEncodeBranch() { return false; }
            virtual bool CanEncodeJump() { return false; }
        };

        struct EncoderJ: public EncoderGeneric {
            using EncoderGeneric::EncoderGeneric;

            virtual void operator()(const Register dst, const Register src1, const Register src2) const {
                buffer->Emit32(0xDEADBEEF | dst.code << 26 | src1.code << 20 | src2.code << 8 );
            }
            virtual bool CanEncodeJ() { return true; }
        };

        template<uint32_t BaseOp>
        struct EncoderJEmit: public EncoderJ {
            using EncoderJ::EncoderJ;

            using EncoderJ::operator();
        };

        struct EncoderJump: public EncoderGeneric {
            using EncoderGeneric::EncoderGeneric;

            virtual void operator()(const Label& destination) const {
                buffer->Emit32(0xDEADBEEF | destination.codePointer );
            }
            virtual bool CanEncodeJump() { return true; }
        };

        template<uint32_t BaseOp>
        struct EncoderJumpEmit: public EncoderJump {
            using EncoderJump::EncoderJump;

            using EncoderJump::operator();
        };

        struct EncoderBranch: public EncoderGeneric {
            using EncoderGeneric::EncoderGeneric;

            virtual void operator()(const Register src1, const Register src2, const Label& destination) const {
                buffer->Emit32(0xDEADBEEF | destination.codePointer | src1.code << 20 | src2.code << 8 );
            }
            virtual bool CanEncodeBranch() { return true; }
        };

        template<uint32_t BaseOp>
        struct EncoderBranchEmit: public EncoderBranch {
            using EncoderBranch::EncoderBranch;

            using EncoderBranch::operator();
        };

        struct AddInstruction final: public EncoderJEmit<0> { using EncoderJEmit::operator(); using EncoderJEmit::EncoderJEmit; };
        struct SubInstruction final: public EncoderJEmit<1> { using EncoderJEmit::operator(); using EncoderJEmit::EncoderJEmit; };
        struct MulInstruction final: public EncoderJEmit<2> { using EncoderJEmit::operator(); using EncoderJEmit::EncoderJEmit; };
        struct DivInstruction final: public EncoderJEmit<3> { using EncoderJEmit::operator(); using EncoderJEmit::EncoderJEmit; };

        struct JalInstruction final: public EncoderJumpEmit<0> { using EncoderJumpEmit::operator(); using EncoderJumpEmit::EncoderJumpEmit; };

        struct BeqInstruction final: public EncoderBranchEmit<0> { using EncoderBranchEmit::operator(); using EncoderBranchEmit::EncoderBranchEmit; };
        struct BneInstruction final: public EncoderBranchEmit<0> { using EncoderBranchEmit::operator(); using EncoderBranchEmit::EncoderBranchEmit; };
        struct BgeInstruction final: public EncoderBranchEmit<0> { using EncoderBranchEmit::operator(); using EncoderBranchEmit::EncoderBranchEmit; };
        struct BgtInstruction final: public EncoderBranchEmit<0> { using EncoderBranchEmit::operator(); using EncoderBranchEmit::EncoderBranchEmit; };

        struct Assembler {
            using Label = Label;
            using EncoderGeneric = EncoderGeneric;
            using EncoderJ = EncoderJ;
            using EncoderJump = EncoderJump;
            using EncoderBranch = EncoderBranch;

            Assembler(uint32_t* dataBuffer) : buffer(dataBuffer) { }

            CodeBuffer buffer;
            #define REGISTER_DECL(n) static constexpr const Register r##n {n, RC_GPR, RS_32}

            REGISTER_DECL(0); REGISTER_DECL(1); REGISTER_DECL(2); REGISTER_DECL(3);
            REGISTER_DECL(4); REGISTER_DECL(5); REGISTER_DECL(6); REGISTER_DECL(7);
            REGISTER_DECL(8); REGISTER_DECL(9); REGISTER_DECL(10);REGISTER_DECL(11);
            REGISTER_DECL(12); REGISTER_DECL(13); REGISTER_DECL(14); REGISTER_DECL(15);
            REGISTER_DECL(16); REGISTER_DECL(17); REGISTER_DECL(18); REGISTER_DECL(19);
            REGISTER_DECL(20); REGISTER_DECL(21); REGISTER_DECL(22); REGISTER_DECL(23);
            REGISTER_DECL(24); REGISTER_DECL(25); REGISTER_DECL(26); REGISTER_DECL(27);
            REGISTER_DECL(28); REGISTER_DECL(29); REGISTER_DECL(30); REGISTER_DECL(31);

            #undef REGISTER_DECL

            void bind(Label& label) {
                assert(label.bound == false);
                label.bound = true;
            }

            const AddInstruction add { &buffer };
            const SubInstruction sub { &buffer };
            const MulInstruction mul { &buffer };
            const DivInstruction div { &buffer };

            const JalInstruction jal { &buffer };

            const BeqInstruction beq { &buffer };
            const BneInstruction bne { &buffer };
            const BgeInstruction bge { &buffer };
            const BgtInstruction bgt { &buffer };
        };
    }
}
