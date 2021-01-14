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
            insn(r0, r1, r2);
        }
    }

    void EncoderGenericTest(const EncoderGeneric& insn) {
        insn(r3, r0, r3);

        // should generate runtime error
        Label dst;
        insn(dst);

        insn(r0, r1, dst);
    }

    void EncoderJTest(const EncoderJ& insn) {
        insn(r1, r2, r3);

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

        jal(fn);

        Label ptr(0xC0DEC0de);
        
        jal(ptr);

        Label local;

        beq(r1, r2, local);

        bind(local);

        Label memberFn(&MyEmitter::MemberFunction);

        jal(memberFn);
    }

    void EmitSomething() {
        add(r0, r1, r2);
        sub(r1, r2, r3);
        mul(r31, r8, r11);
        div(r22, r12, r7);

        EncoderJTest(add);
        EncoderJTest(div);
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
    }
};

int main() {
    printf("Test\n");
    MyEmitter test;

    test.EmitSomething();

    test.div(test.r0, test.r2, test.r3);
    
    test.Print();
    
    return 0;
}
