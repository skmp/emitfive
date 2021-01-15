#include <cstdio>
#include <cstdlib>

#include "emitfive.h"

struct MyEmitter: emitfive::riscv64::Assembler {
    uint32_t codeBuffer[64];
    MyEmitter() : emitfive::riscv64::Assembler(codeBuffer) {

    }

    void FlexbileCodeGenerationTest(bool branch, const EncoderGeneric& insn) {
        if (branch) {
            Label dst;
            insn(dst);
            bind(dst);
        } else {
            insn(x0, x1, x2);
        }
    }

    void EncoderGenericTest(const EncoderGeneric& insn) {
        insn(x3, x0, x3);

        // should generate runtime error
        Label dst;
        insn(dst);

        insn(x0, x1, dst);
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
        EncoderGenericTest(add);
        FlexbileCodeGenerationTest(false, add);
        FlexbileCodeGenerationTest(true, jal);
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

int main() {
    printf("Test\n");
    MyEmitter test;

    test.EmitSomething();

    test.sub(test.x0, test.x2, test.x3);
    
    test.Print();
    
    return 0;
}
