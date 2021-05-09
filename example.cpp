#include <cstdio>
#include <cstdlib>

#include "emitfive.h"

using namespace emitfive;
using namespace emitfive::riscv64;

struct MyEmitter: emitfive::riscv64::Assembler {
    uint32_t codeBuffer[64];
    MyEmitter() : emitfive::riscv64::Assembler(codeBuffer) {

    }

    void __attribute__ ((noinline)) FlexbileCodeGenerationTest(bool branch, const Encoder& insn) {
        if (branch) {
            Label dst;
            insn(x0, dst);
            bind(dst);
        } else {
            insn(x0, x1, x2);
        }
    }

    void EncoderGenericTest(const Encoder& insn) {
        insn(x3, x0, x3);
    
        // should generate runtime error
        Label dst;
        try {
            insn(x0, dst);
            abort();
        } catch (InvalidEncoderUsed<void (emitfive::RegisterGpr, emitfive::Label const&)> ex) {
            printf("Expected failure happened <<%s>>\n", ex.message);
        }

        try {
            insn(x0, x1, dst);
            abort();
        } catch (InvalidEncoderUsed<void (emitfive::RegisterGpr, emitfive::RegisterGpr, emitfive::Label const&)> ex) {
            printf("Expected failure happened: <<%s>>\n", ex.message);
        }
    }

    void EncoderRTest(const EncoderR& insn) {
        insn(x1, x2, x3);

        /*
        //these should fail

        Label dst;
        insn(dst);

        insn(r0, r1, dst);
        */
    }

    static int StaticFunction(int a, int b) {
        return a + b;
    }

    int MemberFunction(int a, int b) {
        return a + b;
    }

    void BranchTest() {
        Label fn(&StaticFunction);

        jal(x1, fn);

        Label ptr(0xC0DEC0de);
        
        jal(x1, ptr);

        Label local;

        beq(x1, x2, local);

        bind(local);

        Label memberFn(&MyEmitter::MemberFunction);

        jal(x1, memberFn);
    }

    void EmitSomething() {
        add(r0, a0, a1);
        jalr(x0, ra, 0);

        add(x0, x1, x2);
        sub(x1, x2, x3);
        //mul(x31, x8, x11);
        //div(x22, x12, x7);
        
        EncoderRTest(add);
        EncoderRTest(sub);
        
        BranchTest();
        
        Encoder encoded = add;
        encoded(x0, x1, x2);
        
        EncoderGenericTest(add);
        
        
        FlexbileCodeGenerationTest(false, add); 
        
        FlexbileCodeGenerationTest(true, jal);
    }

    inline void Print() {
        for (uint32_t v : codeBuffer) {
            printf("%08X ", v);
        }
        printf("\n");

        auto f = fopen("dump.bin", "wb");
        fwrite(codeBuffer, 1, sizeof(codeBuffer), f);
        fclose(f);

        // TODO: Flush Cache
        int (*jitAdd)(int, int) = reinterpret_cast<int (*)(int, int)>(codeBuffer);
        //printf("1 + 3 = %d\n", jitAdd(1, 3));
    }
};
/*
struct MyMacroEmitter: emitfive::riscv64::MacroAssembler {
    uint32_t codeBuffer[64];

    MyMacroEmitter() : emitfive::riscv64::MacroAssembler(codeBuffer) {

    }

    void EmitSomething() {
        Add(a0, a0, 3);
        Add(r0, a0, a1);
        Ret();

        Add(a0.w(), a0, 3);
        Add(r0.w(), a0, a1);
        Ret();
    }

    void Print() {
        for (uint32_t v : codeBuffer) {
            printf("%08X ", v);
        }
        printf("\n");

        auto f = fopen("dump.bin", "wb");
        fwrite(codeBuffer, 1, sizeof(codeBuffer), f);
        fclose(f);

        // TODO: Flush Cache
        int (*jitAdd)(int, int) = reinterpret_cast<int (*)(int, int)>(codeBuffer);
        printf("1 + 3 = %d\n", jitAdd(1, 3));
    }
};
*/
int main() {  
    MyEmitter test;

    EncoderR encoderR = test.add;
    encoderR(test.x0, test.x2, test.x3);
    test.EmitSomething();

    test.sub(test.x0, test.x2, test.x3);
    
    test.Print();
/*
    MyMacroEmitter test2;

    test2.EmitSomething();

    test2.Print();
    */
    return 0;
}
