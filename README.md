# emitfive
A runtime emitter/assembler/disassembler for RISC-V. Similar to vixl and xbyak.

## Design Goals
- Be fast. Simple code should result into direct memory writes.
- Be header only
- Allow generic code generation
- Have nice syntax
- Avoid pitfalls of xbyak and vixl
- Have a full suite of tests
- Have a MacroAssembler that places constant pools, branch gates and so on automatically

## Current status
- [x] Interface Ideation
- [ ] Complete interface
- [ ] Actually emits code, without labels
- [ ] Supports labels
- [ ] Supports relocations
- [ ] Is fully tested
- [ ] Has MacroAssembler
- [ ] Supports risc-v extensions
- [ ] Supports riscv32

## Developed with
- VSCode + cSpell
- clang 11