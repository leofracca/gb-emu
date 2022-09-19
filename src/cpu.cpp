/*
 * See pages 61 to 118 of the documentation (docs/GB.pdf)
 * See https://gbdev.io/pandocs/CPU_Instruction_Set.html
 * See https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
 */

#include "../include/cpu.h" // CPU

#include <stdexcept> // std::runtime_error

namespace gameboy
{
    CPU::CPU(Memory *memory)
        : m_memory(memory)
    {}

    uint8_t CPU::cycle()
    {
        // Check interrupts
        int cycles = handleInterrupts();
        if (cycles > 0)
            return cycles;

        // No operation because the CPU is halted
        if (m_halted)
            return 1;

        // Fetch opcode
        uint8_t instruction = m_memory->read(m_registers.pc++);

        // Decode and execute opcode
        return execute_opcode(instruction);
    }

    uint8_t CPU::handleInterrupts()
    {
        // Interrupts are disabled
        if (!m_ime)
            return 0;

        // If here, interrupts are enabled
        // Get the requested interrupt
        uint8_t interrupt = m_memory->read(INTERRUPT_FLAG_ADDRESS) & m_memory->read(INTERRUPT_ENABLE_ADDRESS);
        if (interrupt == 0)
            return 0;

        m_halted = false;
        push(m_registers.pc);

        // Handle the interrupt
        for (uint8_t interruptBit = 0; interruptBit < 5; interruptBit++)
        {
            bool isInterrupt = handleInterrupt(interruptBit, interruptValues[interruptBit], interrupt);
            if (isInterrupt)
                return 5;
        }

        return 0;
    }

    bool CPU::handleInterrupt(uint8_t interruptBit, uint16_t interruptAddress, uint8_t interruptFlagBit)
    {
        if ((interruptFlagBit & (1 << interruptBit)) != 0)
        {
            m_ime = false;
            m_registers.pc = interruptAddress;
            m_memory->write(INTERRUPT_FLAG_ADDRESS, m_memory->read(INTERRUPT_FLAG_ADDRESS) & ~(1 << interruptBit));
            return true;
        }
        return false;
    }

    uint8_t CPU::execute_opcode(uint8_t opcode)
    {
        int cycles = OPCODE_CYCLES[opcode];

        uint8_t value = 0; // Temp variable used for some opcodes

        switch (opcode)
        {
            case 0x00: // NOP
                break;
            case 0x01: // LD BC, nn
                m_registers.setBC(m_memory->readWord(m_registers.pc));
                m_registers.pc += 2;
                break;
            case 0x02: // LD (BC), A
                m_memory->write(m_registers.getBC(), m_registers.a);
                break;
            case 0x03: // INC BC
                m_registers.setBC(m_registers.getBC() + 1);
                break;
            case 0x04: // INC B
                inc(m_registers.b);
                break;
            case 0x05: // DEC B
                dec(m_registers.b);
                break;
            case 0x06: // LD B, n
                m_registers.b = m_memory->read(m_registers.pc++);
                break;
            case 0x07: // RLCA
                rlca();
                break;
            case 0x08: // LD (nn), SP
                m_memory->writeWord(m_memory->readWord(m_registers.pc), m_registers.sp);
                m_registers.pc += 2;
                break;
            case 0x09: // ADD HL, BC
                add_hl(m_registers.getBC());
                break;
            case 0x0A: // LD A, (BC)
                m_registers.a = m_memory->read(m_registers.getBC());
                break;
            case 0x0B: // DEC BC
                m_registers.setBC(m_registers.getBC() - 1);
                break;
            case 0x0C: // INC C
                inc(m_registers.c);
                break;
            case 0x0D: // DEC C
                dec(m_registers.c);
                break;
            case 0x0E: // LD C, n
                m_registers.c = m_memory->read(m_registers.pc++);
                break;
            case 0x0F: // RRCA
                rrca();
                break;
            case 0x10: // STOP
                break;
            case 0x11: // LD DE, nn
                m_registers.setDE(m_memory->readWord(m_registers.pc));
                m_registers.pc += 2;
                break;
            case 0x12: // LD (DE), A
                m_memory->write(m_registers.getDE(), m_registers.a);
                break;
            case 0x13: // INC DE
                m_registers.setDE(m_registers.getDE() + 1);
                break;
            case 0x14: // INC D
                inc(m_registers.d);
                break;
            case 0x15: // DEC D
                dec(m_registers.d);
                break;
            case 0x16: // LD D, n
                m_registers.d = m_memory->read(m_registers.pc++);
                break;
            case 0x17: // RLA
                rla();
                break;
            case 0x18: // JR n
                jr();
                break;
            case 0x19: // ADD HL, DE
                add_hl(m_registers.getDE());
                break;
            case 0x1A: // LD A, (DE)
                m_registers.a = m_memory->read(m_registers.getDE());
                break;
            case 0x1B: // DEC DE
                m_registers.setDE(m_registers.getDE() - 1);
                break;
            case 0x1C: // INC E
                inc(m_registers.e);
                break;
            case 0x1D: // DEC E
                dec(m_registers.e);
                break;
            case 0x1E: // LD E, n
                m_registers.e = m_memory->read(m_registers.pc++);
                break;
            case 0x1F: // RRA
                rra();
                break;
            case 0x20: // JR NZ, n
                jr(!m_registers.getFlag(ZERO_FLAG));
                break;
            case 0x21: // LD HL, nn
                m_registers.setHL(m_memory->readWord(m_registers.pc));
                m_registers.pc += 2;
                break;
            case 0x22: // LD (HL+), A
                m_memory->write(m_registers.getHL(), m_registers.a);
                m_registers.setHL(m_registers.getHL() + 1);
                break;
            case 0x23: // INC HL
                m_registers.setHL(m_registers.getHL() + 1);
                break;
            case 0x24: // INC H
                inc(m_registers.h);
                break;
            case 0x25: // DEC H
                dec(m_registers.h);
                break;
            case 0x26: // LD H, n
                m_registers.h = m_memory->read(m_registers.pc++);
                break;
            case 0x27: // DAA
                daa();
                break;
            case 0x28: // JR Z, n
                jr(m_registers.getFlag(ZERO_FLAG));
                break;
            case 0x29: // ADD HL, HL
                add_hl(m_registers.getHL());
                break;
            case 0x2A: // LD A, (HL+)
                m_registers.a = m_memory->read(m_registers.getHL());
                m_registers.setHL(m_registers.getHL() + 1);
                break;
            case 0x2B: // DEC HL
                m_registers.setHL(m_registers.getHL() - 1);
                break;
            case 0x2C: // INC L
                inc(m_registers.l);
                break;
            case 0x2D: // DEC L
                dec(m_registers.l);
                break;
            case 0x2E: // LD L, n
                m_registers.l = m_memory->read(m_registers.pc++);
                break;
            case 0x2F: // CPL
                cpl();
                break;
            case 0x30: // JR NC, n
                jr(!m_registers.getFlag(CARRY_FLAG));
                break;
            case 0x31: // LD SP, nn
                m_registers.sp = m_memory->readWord(m_registers.pc);
                m_registers.pc += 2;
                break;
            case 0x32: // LD (HL-), A
                m_memory->write(m_registers.getHL(), m_registers.a);
                m_registers.setHL(m_registers.getHL() - 1);
                break;
            case 0x33: // INC SP
                m_registers.sp++;
                break;
            case 0x34: // INC (HL)
                value = m_memory->read(m_registers.getHL());
                inc(value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0x35: // DEC (HL)
                value = m_memory->read(m_registers.getHL());
                dec(value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0x36: // LD (HL), n
                m_memory->write(m_registers.getHL(), m_memory->read(m_registers.pc++));
                break;
            case 0x37: // SCF
                scf();
                break;
            case 0x38: // JR C, n
                jr(m_registers.getFlag(CARRY_FLAG));
                break;
            case 0x39: // ADD HL, SP
                add_hl(m_registers.sp);
                break;
            case 0x3A: // LD A, (HL-)
                m_registers.a = m_memory->read(m_registers.getHL());
                m_registers.setHL(m_registers.getHL() - 1);
                break;
            case 0x3B: // DEC SP
                m_registers.sp--;
                break;
            case 0x3C: // INC A
                inc(m_registers.a);
                break;
            case 0x3D: // DEC A
                dec(m_registers.a);
                break;
            case 0x3E: // LD A, n
                m_registers.a = m_memory->read(m_registers.pc++);
                break;
            case 0x3F: // CCF
                ccf();
                break;
            case 0x40: // LD B, B
                m_registers.b = m_registers.b;
                break;
            case 0x41: // LD B, C
                m_registers.b = m_registers.c;
                break;
            case 0x42: // LD B, D
                m_registers.b = m_registers.d;
                break;
            case 0x43: // LD B, E
                m_registers.b = m_registers.e;
                break;
            case 0x44: // LD B, H
                m_registers.b = m_registers.h;
                break;
            case 0x45: // LD B, L
                m_registers.b = m_registers.l;
                break;
            case 0x46: // LD B, (HL)
                m_registers.b = m_memory->read(m_registers.getHL());
                break;
            case 0x47: // LD B, A
                m_registers.b = m_registers.a;
                break;
            case 0x48: // LD C, B
                m_registers.c = m_registers.b;
                break;
            case 0x49: // LD C, C
                m_registers.c = m_registers.c;
                break;
            case 0x4A: // LD C, D
                m_registers.c = m_registers.d;
                break;
            case 0x4B: // LD C, E
                m_registers.c = m_registers.e;
                break;
            case 0x4C: // LD C, H
                m_registers.c = m_registers.h;
                break;
            case 0x4D: // LD C, L
                m_registers.c = m_registers.l;
                break;
            case 0x4E: // LD C, (HL)
                m_registers.c = m_memory->read(m_registers.getHL());
                break;
            case 0x4F: // LD C, A
                m_registers.c = m_registers.a;
                break;
            case 0x50: // LD D, B
                m_registers.d = m_registers.b;
                break;
            case 0x51: // LD D, C
                m_registers.d = m_registers.c;
                break;
            case 0x52: // LD D, D
                m_registers.d = m_registers.d;
                break;
            case 0x53: // LD D, E
                m_registers.d = m_registers.e;
                break;
            case 0x54: // LD D, H
                m_registers.d = m_registers.h;
                break;
            case 0x55: // LD D, L
                m_registers.d = m_registers.l;
                break;
            case 0x56: // LD D, (HL)
                m_registers.d = m_memory->read(m_registers.getHL());
                break;
            case 0x57: // LD D, A
                m_registers.d = m_registers.a;
                break;
            case 0x58: // LD E, B
                m_registers.e = m_registers.b;
                break;
            case 0x59: // LD E, C
                m_registers.e = m_registers.c;
                break;
            case 0x5A: // LD E, D
                m_registers.e = m_registers.d;
                break;
            case 0x5B: // LD E, E
                m_registers.e = m_registers.e;
                break;
            case 0x5C: // LD E, H
                m_registers.e = m_registers.h;
                break;
            case 0x5D: // LD E, L
                m_registers.e = m_registers.l;
                break;
            case 0x5E: // LD E, (HL)
                m_registers.e = m_memory->read(m_registers.getHL());
                break;
            case 0x5F: // LD E, A
                m_registers.e = m_registers.a;
                break;
            case 0x60: // LD H, B
                m_registers.h = m_registers.b;
                break;
            case 0x61: // LD H, C
                m_registers.h = m_registers.c;
                break;
            case 0x62: // LD H, D
                m_registers.h = m_registers.d;
                break;
            case 0x63: // LD H, E
                m_registers.h = m_registers.e;
                break;
            case 0x64: // LD H, H
                m_registers.h = m_registers.h;
                break;
            case 0x65: // LD H, L
                m_registers.h = m_registers.l;
                break;
            case 0x66: // LD H, (HL)
                m_registers.h = m_memory->read(m_registers.getHL());
                break;
            case 0x67: // LD H, A
                m_registers.h = m_registers.a;
                break;
            case 0x68: // LD L, B
                m_registers.l = m_registers.b;
                break;
            case 0x69: // LD L, C
                m_registers.l = m_registers.c;
                break;
            case 0x6A: // LD L, D
                m_registers.l = m_registers.d;
                break;
            case 0x6B: // LD L, E
                m_registers.l = m_registers.e;
                break;
            case 0x6C: // LD L, H
                m_registers.l = m_registers.h;
                break;
            case 0x6D: // LD L, L
                m_registers.l = m_registers.l;
                break;
            case 0x6E: // LD L, (HL)
                m_registers.l = m_memory->read(m_registers.getHL());
                break;
            case 0x6F: // LD L, A
                m_registers.l = m_registers.a;
                break;
            case 0x70: // LD (HL), B
                m_memory->write(m_registers.getHL(), m_registers.b);
                break;
            case 0x71: // LD (HL), C
                m_memory->write(m_registers.getHL(), m_registers.c);
                break;
            case 0x72: // LD (HL), D
                m_memory->write(m_registers.getHL(), m_registers.d);
                break;
            case 0x73: // LD (HL), E
                m_memory->write(m_registers.getHL(), m_registers.e);
                break;
            case 0x74: // LD (HL), H
                m_memory->write(m_registers.getHL(), m_registers.h);
                break;
            case 0x75: // LD (HL), L
                m_memory->write(m_registers.getHL(), m_registers.l);
                break;
            case 0x76: // HALT
                halt();
                break;
            case 0x77: // LD (HL), A
                m_memory->write(m_registers.getHL(), m_registers.a);
                break;
            case 0x78: // LD A, B
                m_registers.a = m_registers.b;
                break;
            case 0x79: // LD A, C
                m_registers.a = m_registers.c;
                break;
            case 0x7A: // LD A, D
                m_registers.a = m_registers.d;
                break;
            case 0x7B: // LD A, E
                m_registers.a = m_registers.e;
                break;
            case 0x7C: // LD A, H
                m_registers.a = m_registers.h;
                break;
            case 0x7D: // LD A, L
                m_registers.a = m_registers.l;
                break;
            case 0x7E: // LD A, (HL)
                m_registers.a = m_memory->read(m_registers.getHL());
                break;
            case 0x7F: // LD A, A
                m_registers.a = m_registers.a;
                break;
            case 0x80: // ADD A, B
                add(m_registers.b);
                break;
            case 0x81: // ADD A, C
                add(m_registers.c);
                break;
            case 0x82: // ADD A, D
                add(m_registers.d);
                break;
            case 0x83: // ADD A, E
                add(m_registers.e);
                break;
            case 0x84: // ADD A, H
                add(m_registers.h);
                break;
            case 0x85: // ADD A, L
                add(m_registers.l);
                break;
            case 0x86: // ADD A, (HL)
                add(m_memory->read(m_registers.getHL()));
                break;
            case 0x87: // ADD A, A
                add(m_registers.a);
                break;
            case 0x88: // ADC A, B
                adc(m_registers.b);
                break;
            case 0x89: // ADC A, C
                adc(m_registers.c);
                break;
            case 0x8A: // ADC A, D
                adc(m_registers.d);
                break;
            case 0x8B: // ADC A, E
                adc(m_registers.e);
                break;
            case 0x8C: // ADC A, H
                adc(m_registers.h);
                break;
            case 0x8D: // ADC A, L
                adc(m_registers.l);
                break;
            case 0x8E: // ADC A, (HL)
                adc(m_memory->read(m_registers.getHL()));
                break;
            case 0x8F: // ADC A, A
                adc(m_registers.a);
                break;
            case 0x90: // SUB B
                sub(m_registers.b);
                break;
            case 0x91: // SUB C
                sub(m_registers.c);
                break;
            case 0x92: // SUB D
                sub(m_registers.d);
                break;
            case 0x93: // SUB E
                sub(m_registers.e);
                break;
            case 0x94: // SUB H
                sub(m_registers.h);
                break;
            case 0x95: // SUB L
                sub(m_registers.l);
                break;
            case 0x96: // SUB (HL)
                sub(m_memory->read(m_registers.getHL()));
                break;
            case 0x97: // SUB A
                sub(m_registers.a);
                break;
            case 0x98: // SBC A, B
                sbc(m_registers.b);
                break;
            case 0x99: // SBC A, C
                sbc(m_registers.c);
                break;
            case 0x9A: // SBC A, D
                sbc(m_registers.d);
                break;
            case 0x9B: // SBC A, E
                sbc(m_registers.e);
                break;
            case 0x9C: // SBC A, H
                sbc(m_registers.h);
                break;
            case 0x9D: // SBC A, L
                sbc(m_registers.l);
                break;
            case 0x9E: // SBC A, (HL)
                sbc(m_memory->read(m_registers.getHL()));
                break;
            case 0x9F: // SBC A, A
                sbc(m_registers.a);
                break;
            case 0xA0: // AND B
                and_(m_registers.b);
                break;
            case 0xA1: // AND C
                and_(m_registers.c);
                break;
            case 0xA2: // AND D
                and_(m_registers.d);
                break;
            case 0xA3: // AND E
                and_(m_registers.e);
                break;
            case 0xA4: // AND H
                and_(m_registers.h);
                break;
            case 0xA5: // AND L
                and_(m_registers.l);
                break;
            case 0xA6: // AND (HL)
                and_(m_memory->read(m_registers.getHL()));
                break;
            case 0xA7: // AND A
                and_(m_registers.a);
                break;
            case 0xA8: // XOR B
                xor_(m_registers.b);
                break;
            case 0xA9: // XOR C
                xor_(m_registers.c);
                break;
            case 0xAA: // XOR D
                xor_(m_registers.d);
                break;
            case 0xAB: // XOR E
                xor_(m_registers.e);
                break;
            case 0xAC: // XOR H
                xor_(m_registers.h);
                break;
            case 0xAD: // XOR L
                xor_(m_registers.l);
                break;
            case 0xAE: // XOR (HL)
                xor_(m_memory->read(m_registers.getHL()));
                break;
            case 0xAF: // XOR A
                xor_(m_registers.a);
                break;
            case 0xB0: // OR B
                or_(m_registers.b);
                break;
            case 0xB1: // OR C
                or_(m_registers.c);
                break;
            case 0xB2: // OR D
                or_(m_registers.d);
                break;
            case 0xB3: // OR E
                or_(m_registers.e);
                break;
            case 0xB4: // OR H
                or_(m_registers.h);
                break;
            case 0xB5: // OR L
                or_(m_registers.l);
                break;
            case 0xB6: // OR (HL)
                or_(m_memory->read(m_registers.getHL()));
                break;
            case 0xB7: // OR A
                or_(m_registers.a);
                break;
            case 0xB8: // CP B
                cp(m_registers.b);
                break;
            case 0xB9: // CP C
                cp(m_registers.c);
                break;
            case 0xBA: // CP D
                cp(m_registers.d);
                break;
            case 0xBB: // CP E
                cp(m_registers.e);
                break;
            case 0xBC: // CP H
                cp(m_registers.h);
                break;
            case 0xBD: // CP L
                cp(m_registers.l);
                break;
            case 0xBE: // CP (HL)
                cp(m_memory->read(m_registers.getHL()));
                break;
            case 0xBF: // CP A
                cp(m_registers.a);
                break;
            case 0xC0: // RET NZ
                ret(!m_registers.getFlag(ZERO_FLAG));
                break;
            case 0xC1: // POP BC
                m_registers.setBC(pop());
                break;
            case 0xC2: // JP NZ, nn
                jp(!m_registers.getFlag(ZERO_FLAG));
                break;
            case 0xC3: // JP nn
                jp();
                break;
            case 0xC4: // CALL NZ, nn
                call(!m_registers.getFlag(ZERO_FLAG));
                break;
            case 0xC5: // PUSH BC
                push(m_registers.getBC());
                break;
            case 0xC6: // ADD A, n
                add(m_memory->read(m_registers.pc++));
                break;
            case 0xC7: // RST 00H
                rst(0x00);
                break;
            case 0xC8: // RET Z
                ret(m_registers.getFlag(ZERO_FLAG));
                break;
            case 0xC9: // RET
                ret();
                break;
            case 0xCA: // JP Z, nn
                jp(m_registers.getFlag(ZERO_FLAG));
                break;
            case 0xCB: // CB prefix
                cycles += execute_CB_opcode(m_memory->read(m_registers.pc++));
                break;
            case 0xCC: // CALL Z, nn
                call(m_registers.getFlag(ZERO_FLAG));
                break;
            case 0xCD: // CALL nn
                call();
                break;
            case 0xCE: // ADC A, n
                adc(m_memory->read(m_registers.pc++));
                break;
            case 0xCF: // RST 08H
                rst(0x08);
                break;
            case 0xD0: // RET NC
                ret(!m_registers.getFlag(CARRY_FLAG));
                break;
            case 0xD1: // POP DE
                m_registers.setDE(pop());
                break;
            case 0xD2: // JP NC, nn
                jp(!m_registers.getFlag(CARRY_FLAG));
                break;
            case 0xD4: // CALL NC, nn
                call(!m_registers.getFlag(CARRY_FLAG));
                break;
            case 0xD5: // PUSH DE
                push(m_registers.getDE());
                break;
            case 0xD6: // SUB n
                sub(m_memory->read(m_registers.pc++));
                break;
            case 0xD7: // RST 10H
                rst(0x10);
                break;
            case 0xD8: // RET C
                ret(m_registers.getFlag(CARRY_FLAG));
                break;
            case 0xD9: // RETI
                reti();
                break;
            case 0xDA: // JP C, nn
                jp(m_registers.getFlag(CARRY_FLAG));
                break;
            case 0xDC: // CALL C, nn
                call(m_registers.getFlag(CARRY_FLAG));
                break;
            case 0xDE: // SBC A, n
                sbc(m_memory->read(m_registers.pc++));
                break;
            case 0xDF: // RST 18H
                rst(0x18);
                break;
            case 0xE0: // LDH (n), A
                m_memory->write(LD_START_ADDRESS + m_memory->read(m_registers.pc++), m_registers.a);
                break;
            case 0xE1: // POP HL
                m_registers.setHL(pop());
                break;
            case 0xE2: // LD (C), A
                m_memory->write(LD_START_ADDRESS + m_registers.c, m_registers.a);
                break;
            case 0xE5: // PUSH HL
                push(m_registers.getHL());
                break;
            case 0xE6: // AND n
                and_(m_memory->read(m_registers.pc++));
                break;
            case 0xE7: // RST 20H
                rst(0x20);
                break;
            case 0xE8: // ADD SP, n
                add_sp(static_cast<int8_t>(m_memory->read(m_registers.pc++)));
                break;
            case 0xE9: // JP (HL)
                m_registers.pc = m_registers.getHL();
                break;
            case 0xEA: // LD (nn), A
                m_memory->write(m_memory->readWord(m_registers.pc), m_registers.a);
                m_registers.pc += 2;
                break;
            case 0xEE: // XOR n
                xor_(m_memory->read(m_registers.pc++));
                break;
            case 0xEF: // RST 28H
                rst(0x28);
                break;
            case 0xF0: // LDH A, (n)
                m_registers.a = m_memory->read(LD_START_ADDRESS + m_memory->read(m_registers.pc++));
                break;
            case 0xF1: // POP AF
                m_registers.setAF(pop());
                m_registers.f &= 0xf0;
                break;
            case 0xF2: // LD A, (C)
                m_registers.a = m_memory->read(LD_START_ADDRESS + m_registers.c);
                break;
            case 0xF3: // DI
                di();
                break;
            case 0xF5: // PUSH AF
                push(m_registers.getAF());
                break;
            case 0xF6: // OR n
                or_(m_memory->read(m_registers.pc++));
                break;
            case 0xF7: // RST 30H
                rst(0x30);
                break;
            case 0xF8: // LD HL, SP+n
                ldhl(static_cast<int8_t>(m_memory->read(m_registers.pc++)));
                break;
            case 0xF9: // LD SP, HL
                m_registers.sp = m_registers.getHL();
                break;
            case 0xFA: // LD A, (nn)
                m_registers.a = m_memory->read(m_memory->readWord(m_registers.pc));
                m_registers.pc += 2;
                break;
            case 0xFB: // EI
                ei();
                break;
            case 0xFE: // CP n
                cp(m_memory->read(m_registers.pc++));
                break;
            case 0xFF: // RST 38H
                rst(0x38);
                break;
            default:
                throw std::runtime_error("Unexpected opcode: " + std::to_string(opcode));
        }

        if (branched)
            cycles = OPCODE_CYCLES_BRANCHED[opcode];

        return cycles;
    }

    uint8_t CPU::execute_CB_opcode(uint8_t opcode)
    {
        uint8_t cycles = OPCODE_CB_CYCLES[opcode];

        uint8_t value = 0; // Temp variable used for some opcodes

        switch (opcode)
        {
            case 0x00: // RLC B
                rlc(m_registers.b);
                break;
            case 0x01: // RLC C
                rlc(m_registers.c);
                break;
            case 0x02: // RLC D
                rlc(m_registers.d);
                break;
            case 0x03: // RLC E
                rlc(m_registers.e);
                break;
            case 0x04: // RLC H
                rlc(m_registers.h);
                break;
            case 0x05: // RLC L
                rlc(m_registers.l);
                break;
            case 0x06: // RLC (HL)
                value = m_memory->read(m_registers.getHL());
                rlc(value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0x07: // RLC A
                rlc(m_registers.a);
                break;
            case 0x08: // RRC B
                rrc(m_registers.b);
                break;
            case 0x09: // RRC C
                rrc(m_registers.c);
                break;
            case 0x0A: // RRC D
                rrc(m_registers.d);
                break;
            case 0x0B: // RRC E
                rrc(m_registers.e);
                break;
            case 0x0C: // RRC H
                rrc(m_registers.h);
                break;
            case 0x0D: // RRC L
                rrc(m_registers.l);
                break;
            case 0x0E: // RRC (HL)
                value = m_memory->read(m_registers.getHL());
                rrc(value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0x0F: // RRC A
                rrc(m_registers.a);
                break;
            case 0x10: // RL B
                rl(m_registers.b);
                break;
            case 0x11: // RL C
                rl(m_registers.c);
                break;
            case 0x12: // RL D
                rl(m_registers.d);
                break;
            case 0x13: // RL E
                rl(m_registers.e);
                break;
            case 0x14: // RL H
                rl(m_registers.h);
                break;
            case 0x15: // RL L
                rl(m_registers.l);
                break;
            case 0x16: // RL (HL)
                value = m_memory->read(m_registers.getHL());
                rl(value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0x17: // RL A
                rl(m_registers.a);
                break;
            case 0x18: // RR B
                rr(m_registers.b);
                break;
            case 0x19: // RR C
                rr(m_registers.c);
                break;
            case 0x1A: // RR D
                rr(m_registers.d);
                break;
            case 0x1B: // RR E
                rr(m_registers.e);
                break;
            case 0x1C: // RR H
                rr(m_registers.h);
                break;
            case 0x1D: // RR L
                rr(m_registers.l);
                break;
            case 0x1E: // RR (HL)
                value = m_memory->read(m_registers.getHL());
                rr(value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0x1F: // RR A
                rr(m_registers.a);
                break;
            case 0x20: // SLA B
                sla(m_registers.b);
                break;
            case 0x21: // SLA C
                sla(m_registers.c);
                break;
            case 0x22: // SLA D
                sla(m_registers.d);
                break;
            case 0x23: // SLA E
                sla(m_registers.e);
                break;
            case 0x24: // SLA H
                sla(m_registers.h);
                break;
            case 0x25: // SLA L
                sla(m_registers.l);
                break;
            case 0x26: // SLA (HL)
                value = m_memory->read(m_registers.getHL());
                sla(value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0x27: // SLA A
                sla(m_registers.a);
                break;
            case 0x28: // SRA B
                sra(m_registers.b);
                break;
            case 0x29: // SRA C
                sra(m_registers.c);
                break;
            case 0x2A: // SRA D
                sra(m_registers.d);
                break;
            case 0x2B: // SRA E
                sra(m_registers.e);
                break;
            case 0x2C: // SRA H
                sra(m_registers.h);
                break;
            case 0x2D: // SRA L
                sra(m_registers.l);
                break;
            case 0x2E: // SRA (HL)
                value = m_memory->read(m_registers.getHL());
                sra(value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0x2F: // SRA A
                sra(m_registers.a);
                break;
            case 0x30: // SWAP B
                swap(m_registers.b);
                break;
            case 0x31: // SWAP C
                swap(m_registers.c);
                break;
            case 0x32: // SWAP D
                swap(m_registers.d);
                break;
            case 0x33: // SWAP E
                swap(m_registers.e);
                break;
            case 0x34: // SWAP H
                swap(m_registers.h);
                break;
            case 0x35: // SWAP L
                swap(m_registers.l);
                break;
            case 0x36: // SWAP (HL)
                value = m_memory->read(m_registers.getHL());
                swap(value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0x37: // SWAP A
                swap(m_registers.a);
                break;
            case 0x38: // SRL B
                srl(m_registers.b);
                break;
            case 0x39: // SRL C
                srl(m_registers.c);
                break;
            case 0x3A: // SRL D
                srl(m_registers.d);
                break;
            case 0x3B: // SRL E
                srl(m_registers.e);
                break;
            case 0x3C: // SRL H
                srl(m_registers.h);
                break;
            case 0x3D: // SRL L
                srl(m_registers.l);
                break;
            case 0x3E: // SRL (HL)
                value = m_memory->read(m_registers.getHL());
                srl(value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0x3F: // SRL A
                srl(m_registers.a);
                break;
            case 0x40: // BIT 0, B
                bit(0, m_registers.b);
                break;
            case 0x41: // BIT 0, C
                bit(0, m_registers.c);
                break;
            case 0x42: // BIT 0, D
                bit(0, m_registers.d);
                break;
            case 0x43: // BIT 0, E
                bit(0, m_registers.e);
                break;
            case 0x44: // BIT 0, H
                bit(0, m_registers.h);
                break;
            case 0x45: // BIT 0, L
                bit(0, m_registers.l);
                break;
            case 0x46: // BIT 0, (HL)
                value = m_memory->read(m_registers.getHL());
                bit(0, value);
                break;
            case 0x47: // BIT 0, A
                bit(0, m_registers.a);
                break;
            case 0x48: // BIT 1, B
                bit(1, m_registers.b);
                break;
            case 0x49: // BIT 1, C
                bit(1, m_registers.c);
                break;
            case 0x4A: // BIT 1, D
                bit(1, m_registers.d);
                break;
            case 0x4B: // BIT 1, E
                bit(1, m_registers.e);
                break;
            case 0x4C: // BIT 1, H
                bit(1, m_registers.h);
                break;
            case 0x4D: // BIT 1, L
                bit(1, m_registers.l);
                break;
            case 0x4E: // BIT 1, (HL)
                value = m_memory->read(m_registers.getHL());
                bit(1, value);
                break;
            case 0x4F: // BIT 1, A
                bit(1, m_registers.a);
                break;
            case 0x50: // BIT 2, B
                bit(2, m_registers.b);
                break;
            case 0x51: // BIT 2, C
                bit(2, m_registers.c);
                break;
            case 0x52: // BIT 2, D
                bit(2, m_registers.d);
                break;
            case 0x53: // BIT 2, E
                bit(2, m_registers.e);
                break;
            case 0x54: // BIT 2, H
                bit(2, m_registers.h);
                break;
            case 0x55: // BIT 2, L
                bit(2, m_registers.l);
                break;
            case 0x56: // BIT 2, (HL)
                value = m_memory->read(m_registers.getHL());
                bit(2, value);
                break;
            case 0x57: // BIT 2, A
                bit(2, m_registers.a);
                break;
            case 0x58: // BIT 3, B
                bit(3, m_registers.b);
                break;
            case 0x59: // BIT 3, C
                bit(3, m_registers.c);
                break;
            case 0x5A: // BIT 3, D
                bit(3, m_registers.d);
                break;
            case 0x5B: // BIT 3, E
                bit(3, m_registers.e);
                break;
            case 0x5C: // BIT 3, H
                bit(3, m_registers.h);
                break;
            case 0x5D: // BIT 3, L
                bit(3, m_registers.l);
                break;
            case 0x5E: // BIT 3, (HL)
                value = m_memory->read(m_registers.getHL());
                bit(3, value);
                break;
            case 0x5F: // BIT 3, A
                bit(3, m_registers.a);
                break;
            case 0x60: // BIT 4, B
                bit(4, m_registers.b);
                break;
            case 0x61: // BIT 4, C
                bit(4, m_registers.c);
                break;
            case 0x62: // BIT 4, D
                bit(4, m_registers.d);
                break;
            case 0x63: // BIT 4, E
                bit(4, m_registers.e);
                break;
            case 0x64: // BIT 4, H
                bit(4, m_registers.h);
                break;
            case 0x65: // BIT 4, L
                bit(4, m_registers.l);
                break;
            case 0x66: // BIT 4, (HL)
                value = m_memory->read(m_registers.getHL());
                bit(4, value);
                break;
            case 0x67: // BIT 4, A
                bit(4, m_registers.a);
                break;
            case 0x68: // BIT 5, B
                bit(5, m_registers.b);
                break;
            case 0x69: // BIT 5, C
                bit(5, m_registers.c);
                break;
            case 0x6A: // BIT 5, D
                bit(5, m_registers.d);
                break;
            case 0x6B: // BIT 5, E
                bit(5, m_registers.e);
                break;
            case 0x6C: // BIT 5, H
                bit(5, m_registers.h);
                break;
            case 0x6D: // BIT 5, L
                bit(5, m_registers.l);
                break;
            case 0x6E: // BIT 5, (HL)
                value = m_memory->read(m_registers.getHL());
                bit(5, value);
                break;
            case 0x6F: // BIT 5, A
                bit(5, m_registers.a);
                break;
            case 0x70: // BIT 6, B
                bit(6, m_registers.b);
                break;
            case 0x71: // BIT 6, C
                bit(6, m_registers.c);
                break;
            case 0x72: // BIT 6, D
                bit(6, m_registers.d);
                break;
            case 0x73: // BIT 6, E
                bit(6, m_registers.e);
                break;
            case 0x74: // BIT 6, H
                bit(6, m_registers.h);
                break;
            case 0x75: // BIT 6, L
                bit(6, m_registers.l);
                break;
            case 0x76: // BIT 6, (HL)
                value = m_memory->read(m_registers.getHL());
                bit(6, value);
                break;
            case 0x77: // BIT 6, A
                bit(6, m_registers.a);
                break;
            case 0x78: // BIT 7, B
                bit(7, m_registers.b);
                break;
            case 0x79: // BIT 7, C
                bit(7, m_registers.c);
                break;
            case 0x7A: // BIT 7, D
                bit(7, m_registers.d);
                break;
            case 0x7B: // BIT 7, E
                bit(7, m_registers.e);
                break;
            case 0x7C: // BIT 7, H
                bit(7, m_registers.h);
                break;
            case 0x7D: // BIT 7, L
                bit(7, m_registers.l);
                break;
            case 0x7E: // BIT 7, (HL)
                value = m_memory->read(m_registers.getHL());
                bit(7, value);
                break;
            case 0x7F: // BIT 7, A
                bit(7, m_registers.a);
                break;
            case 0x80: // RES 0, B
                res(0, m_registers.b);
                break;
            case 0x81: // RES 0, C
                res(0, m_registers.c);
                break;
            case 0x82: // RES 0, D
                res(0, m_registers.d);
                break;
            case 0x83: // RES 0, E
                res(0, m_registers.e);
                break;
            case 0x84: // RES 0, H
                res(0, m_registers.h);
                break;
            case 0x85: // RES 0, L
                res(0, m_registers.l);
                break;
            case 0x86: // RES 0, (HL)
                value = m_memory->read(m_registers.getHL());
                res(0, value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0x87: // RES 0, A
                res(0, m_registers.a);
                break;
            case 0x88: // RES 1, B
                res(1, m_registers.b);
                break;
            case 0x89: // RES 1, C
                res(1, m_registers.c);
                break;
            case 0x8A: // RES 1, D
                res(1, m_registers.d);
                break;
            case 0x8B: // RES 1, E
                res(1, m_registers.e);
                break;
            case 0x8C: // RES 1, H
                res(1, m_registers.h);
                break;
            case 0x8D: // RES 1, L
                res(1, m_registers.l);
                break;
            case 0x8E: // RES 1, (HL)
                value = m_memory->read(m_registers.getHL());
                res(1, value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0x8F: // RES 1, A
                res(1, m_registers.a);
                break;
            case 0x90: // RES 2, B
                res(2, m_registers.b);
                break;
            case 0x91: // RES 2, C
                res(2, m_registers.c);
                break;
            case 0x92: // RES 2, D
                res(2, m_registers.d);
                break;
            case 0x93: // RES 2, E
                res(2, m_registers.e);
                break;
            case 0x94: // RES 2, H
                res(2, m_registers.h);
                break;
            case 0x95: // RES 2, L
                res(2, m_registers.l);
                break;
            case 0x96: // RES 2, (HL)
                value = m_memory->read(m_registers.getHL());
                res(2, value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0x97: // RES 2, A
                res(2, m_registers.a);
                break;
            case 0x98: // RES 3, B
                res(3, m_registers.b);
                break;
            case 0x99: // RES 3, C
                res(3, m_registers.c);
                break;
            case 0x9A: // RES 3, D
                res(3, m_registers.d);
                break;
            case 0x9B: // RES 3, E
                res(3, m_registers.e);
                break;
            case 0x9C: // RES 3, H
                res(3, m_registers.h);
                break;
            case 0x9D: // RES 3, L
                res(3, m_registers.l);
                break;
            case 0x9E: // RES 3, (HL)
                value = m_memory->read(m_registers.getHL());
                res(3, value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0x9F: // RES 3, A
                res(3, m_registers.a);
                break;
            case 0xA0: // RES 4, B
                res(4, m_registers.b);
                break;
            case 0xA1: // RES 4, C
                res(4, m_registers.c);
                break;
            case 0xA2: // RES 4, D
                res(4, m_registers.d);
                break;
            case 0xA3: // RES 4, E
                res(4, m_registers.e);
                break;
            case 0xA4: // RES 4, H
                res(4, m_registers.h);
                break;
            case 0xA5: // RES 4, L
                res(4, m_registers.l);
                break;
            case 0xA6: // RES 4, (HL)
                value = m_memory->read(m_registers.getHL());
                res(4, value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0xA7: // RES 4, A
                res(4, m_registers.a);
                break;
            case 0xA8: // RES 5, B
                res(5, m_registers.b);
                break;
            case 0xA9: // RES 5, C
                res(5, m_registers.c);
                break;
            case 0xAA: // RES 5, D
                res(5, m_registers.d);
                break;
            case 0xAB: // RES 5, E
                res(5, m_registers.e);
                break;
            case 0xAC: // RES 5, H
                res(5, m_registers.h);
                break;
            case 0xAD: // RES 5, L
                res(5, m_registers.l);
                break;
            case 0xAE: // RES 5, (HL)
                value = m_memory->read(m_registers.getHL());
                res(5, value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0xAF: // RES 5, A
                res(5, m_registers.a);
                break;
            case 0xB0: // RES 6, B
                res(6, m_registers.b);
                break;
            case 0xB1: // RES 6, C
                res(6, m_registers.c);
                break;
            case 0xB2: // RES 6, D
                res(6, m_registers.d);
                break;
            case 0xB3: // RES 6, E
                res(6, m_registers.e);
                break;
            case 0xB4: // RES 6, H
                res(6, m_registers.h);
                break;
            case 0xB5: // RES 6, L
                res(6, m_registers.l);
                break;
            case 0xB6: // RES 6, (HL)
                value = m_memory->read(m_registers.getHL());
                res(6, value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0xB7: // RES 6, A
                res(6, m_registers.a);
                break;
            case 0xB8: // RES 7, B
                res(7, m_registers.b);
                break;
            case 0xB9: // RES 7, C
                res(7, m_registers.c);
                break;
            case 0xBA: // RES 7, D
                res(7, m_registers.d);
                break;
            case 0xBB: // RES 7, E
                res(7, m_registers.e);
                break;
            case 0xBC: // RES 7, H
                res(7, m_registers.h);
                break;
            case 0xBD: // RES 7, L
                res(7, m_registers.l);
                break;
            case 0xBE: // RES 7, (HL)
                value = m_memory->read(m_registers.getHL());
                res(7, value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0xBF: // RES 7, A
                res(7, m_registers.a);
                break;
            case 0xC0: // SET 0, B
                set(0, m_registers.b);
                break;
            case 0xC1: // SET 0, C
                set(0, m_registers.c);
                break;
            case 0xC2: // SET 0, D
                set(0, m_registers.d);
                break;
            case 0xC3: // SET 0, E
                set(0, m_registers.e);
                break;
            case 0xC4: // SET 0, H
                set(0, m_registers.h);
                break;
            case 0xC5: // SET 0, L
                set(0, m_registers.l);
                break;
            case 0xC6: // SET 0, (HL)
                value = m_memory->read(m_registers.getHL());
                set(0, value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0xC7: // SET 0, A
                set(0, m_registers.a);
                break;
            case 0xC8: // SET 1, B
                set(1, m_registers.b);
                break;
            case 0xC9: // SET 1, C
                set(1, m_registers.c);
                break;
            case 0xCA: // SET 1, D
                set(1, m_registers.d);
                break;
            case 0xCB: // SET 1, E
                set(1, m_registers.e);
                break;
            case 0xCC: // SET 1, H
                set(1, m_registers.h);
                break;
            case 0xCD: // SET 1, L
                set(1, m_registers.l);
                break;
            case 0xCE: // SET 1, (HL)
                value = m_memory->read(m_registers.getHL());
                set(1, value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0xCF: // SET 1, A
                set(1, m_registers.a);
                break;
            case 0xD0: // SET 2, B
                set(2, m_registers.b);
                break;
            case 0xD1: // SET 2, C
                set(2, m_registers.c);
                break;
            case 0xD2: // SET 2, D
                set(2, m_registers.d);
                break;
            case 0xD3: // SET 2, E
                set(2, m_registers.e);
                break;
            case 0xD4: // SET 2, H
                set(2, m_registers.h);
                break;
            case 0xD5: // SET 2, L
                set(2, m_registers.l);
                break;
            case 0xD6: // SET 2, (HL)
                value = m_memory->read(m_registers.getHL());
                set(2, value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0xD7: // SET 2, A
                set(2, m_registers.a);
                break;
            case 0xD8: // SET 3, B
                set(3, m_registers.b);
                break;
            case 0xD9: // SET 3, C
                set(3, m_registers.c);
                break;
            case 0xDA: // SET 3, D
                set(3, m_registers.d);
                break;
            case 0xDB: // SET 3, E
                set(3, m_registers.e);
                break;
            case 0xDC: // SET 3, H
                set(3, m_registers.h);
                break;
            case 0xDD: // SET 3, L
                set(3, m_registers.l);
                break;
            case 0xDE: // SET 3, (HL)
                value = m_memory->read(m_registers.getHL());
                set(3, value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0xDF: // SET 3, A
                set(3, m_registers.a);
                break;
            case 0xE0: // SET 4, B
                set(4, m_registers.b);
                break;
            case 0xE1: // SET 4, C
                set(4, m_registers.c);
                break;
            case 0xE2: // SET 4, D
                set(4, m_registers.d);
                break;
            case 0xE3: // SET 4, E
                set(4, m_registers.e);
                break;
            case 0xE4: // SET 4, H
                set(4, m_registers.h);
                break;
            case 0xE5: // SET 4, L
                set(4, m_registers.l);
                break;
            case 0xE6: // SET 4, (HL)
                value = m_memory->read(m_registers.getHL());
                set(4, value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0xE7: // SET 4, A
                set(4, m_registers.a);
                break;
            case 0xE8: // SET 5, B
                set(5, m_registers.b);
                break;
            case 0xE9: // SET 5, C
                set(5, m_registers.c);
                break;
            case 0xEA: // SET 5, D
                set(5, m_registers.d);
                break;
            case 0xEB: // SET 5, E
                set(5, m_registers.e);
                break;
            case 0xEC: // SET 5, H
                set(5, m_registers.h);
                break;
            case 0xED: // SET 5, L
                set(5, m_registers.l);
                break;
            case 0xEE: // SET 5, (HL)
                value = m_memory->read(m_registers.getHL());
                set(5, value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0xEF: // SET 5, A
                set(5, m_registers.a);
                break;
            case 0xF0: // SET 6, B
                set(6, m_registers.b);
                break;
            case 0xF1: // SET 6, C
                set(6, m_registers.c);
                break;
            case 0xF2: // SET 6, D
                set(6, m_registers.d);
                break;
            case 0xF3: // SET 6, E
                set(6, m_registers.e);
                break;
            case 0xF4: // SET 6, H
                set(6, m_registers.h);
                break;
            case 0xF5: // SET 6, L
                set(6, m_registers.l);
                break;
            case 0xF6: // SET 6, (HL)
                value = m_memory->read(m_registers.getHL());
                set(6, value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0xF7: // SET 6, A
                set(6, m_registers.a);
                break;
            case 0xF8: // SET 7, B
                set(7, m_registers.b);
                break;
            case 0xF9: // SET 7, C
                set(7, m_registers.c);
                break;
            case 0xFA: // SET 7, D
                set(7, m_registers.d);
                break;
            case 0xFB: // SET 7, E
                set(7, m_registers.e);
                break;
            case 0xFC: // SET 7, H
                set(7, m_registers.h);
                break;
            case 0xFD: // SET 7, L
                set(7, m_registers.l);
                break;
            case 0xFE: // SET 7, (HL)
                value = m_memory->read(m_registers.getHL());
                set(7, value);
                m_memory->write(m_registers.getHL(), value);
                break;
            case 0xFF: // SET 7, A
                set(7, m_registers.a);
                break;
            default:
                throw std::runtime_error("Unexpected CB opcode: " + std::to_string(opcode));
        }

        return cycles;
    }

    void CPU::push(uint16_t value)
    {
        m_registers.sp -= 2;
        m_memory->writeWord(m_registers.sp, value);
    }

    uint16_t CPU::pop()
    {
        uint16_t value = m_memory->readWord(m_registers.sp);
        m_registers.sp += 2;
        return value;
    }

    void CPU::add(uint8_t n)
    {
        uint16_t result = m_registers.a + n;

        // Set the zero flag if the result is 0
        m_registers.setFlag(ZERO_FLAG, static_cast<uint8_t>(result) == 0);
        // Set the subtract flag to 0
        m_registers.setFlag(SUBTRACT_FLAG, false);
        // Set the half carry flag if there is a carry from bit 3
        m_registers.setFlag(HALF_CARRY_FLAG, (m_registers.a & 0x0F) + (n & 0x0F) > 0x0F);
        // Set the carry flag if there is a carry from bit 7
        m_registers.setFlag(CARRY_FLAG, result > 0xFF);

        // Add n to the value of the register A
        m_registers.a = static_cast<uint8_t>(result);
    }

    void CPU::adc(uint8_t n)
    {
        uint8_t carry = m_registers.getFlag(CARRY_FLAG) ? 1 : 0;
        unsigned int resultFull = m_registers.a + n + carry; // Save the result in a temporary variable to check for carry from bit 7

        uint8_t result = static_cast<uint8_t>(resultFull); // Get only the lower 8 bits of the result

        // Set the zero flag if the result is 0
        m_registers.setFlag(ZERO_FLAG, result == 0);
        // Set the subtract flag to 0
        m_registers.setFlag(SUBTRACT_FLAG, false);
        // Set the half carry flag if there is a carry from bit 3
        m_registers.setFlag(HALF_CARRY_FLAG, (m_registers.a & 0x0F) + (n & 0x0F) + carry > 0x0F);
        // Set the carry flag if there is a carry from bit 7
        m_registers.setFlag(CARRY_FLAG, resultFull > 0xFF);

        // Set the value of the register A to the result
        m_registers.a = result;
    }

    void CPU::sub(uint8_t n)
    {
        // Set the zero flag if the result is 0
        m_registers.setFlag(ZERO_FLAG, m_registers.a == n);
        // Set the subtract flag to 1
        m_registers.setFlag(SUBTRACT_FLAG, true);
        // Set the half-carry flag if there is no borrow from bit 4
        m_registers.setFlag(HALF_CARRY_FLAG, (m_registers.a & 0x0F) < (n & 0x0F));
        // Set the carry flag if there is no borrow
        m_registers.setFlag(CARRY_FLAG, m_registers.a < n);

        // Subtract n from the value of the register A
        m_registers.a -= n;
    }

    void CPU::sbc(uint8_t n)
    {
        uint8_t carry = m_registers.getFlag(CARRY_FLAG) ? 1 : 0;
        int resultFull = m_registers.a - n - carry; // Save the result in a temporary variable to check for borrow from bit 7

        uint8_t result = static_cast<uint8_t>(resultFull); // Get only the lower 8 bits of the result

        // Set the zero flag if the result is 0
        m_registers.setFlag(ZERO_FLAG, result == 0);
        // Set the subtract flag to 1
        m_registers.setFlag(SUBTRACT_FLAG, true);
        // Set the half carry flag if there is no borrow from bit 4
        m_registers.setFlag(HALF_CARRY_FLAG, (m_registers.a & 0x0F) < (n & 0x0F) + carry);
        // Set the carry flag if there is no borrow
        m_registers.setFlag(CARRY_FLAG, resultFull < 0);

        // Set the value of the register A to the result
        m_registers.a = result;
    }

    void CPU::and_(uint8_t n)
    {
        // Set the zero flag if the result is 0
        m_registers.setFlag(ZERO_FLAG, (m_registers.a & n) == 0);
        // Set the subtract flag to 0
        m_registers.setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 1
        m_registers.setFlag(HALF_CARRY_FLAG, true);
        // Set the carry flag to 0
        m_registers.setFlag(CARRY_FLAG, false);

        // And the value of the register A with n
        m_registers.a &= n;
    }

    void CPU::or_(uint8_t n)
    {
        // Set the zero flag if the result is 0
        m_registers.setFlag(ZERO_FLAG, (m_registers.a | n) == 0);
        // Set the subtract flag to 0
        m_registers.setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 0
        m_registers.setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag to 0
        m_registers.setFlag(CARRY_FLAG, false);

        // Or the value of the register A with n
        m_registers.a |= n;
    }

    void CPU::xor_(uint8_t n)
    {
        // Set the zero flag if the result is 0
        m_registers.setFlag(ZERO_FLAG, (m_registers.a ^ n) == 0);
        // Set the subtract flag to 0
        m_registers.setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 0
        m_registers.setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag to 0
        m_registers.setFlag(CARRY_FLAG, false);

        // Xor the value of the register A with n
        m_registers.a ^= n;
    }

    void CPU::cp(uint8_t n)
    {
        // Set the zero flag if the result is 0
        m_registers.setFlag(ZERO_FLAG, m_registers.a == n);
        // Set the subtract flag to 1
        m_registers.setFlag(SUBTRACT_FLAG, true);
        // Set the half-carry flag if there is no borrow from bit 4
        m_registers.setFlag(HALF_CARRY_FLAG, (m_registers.a & 0x0F) < (n & 0x0F));
        // Set the carry flag if there is no borrow
        m_registers.setFlag(CARRY_FLAG, m_registers.a < n);
    }

    void CPU::inc(uint8_t &n)
    {
        // Increment n
        n++;

        // Set the zero flag if the result is 0
        m_registers.setFlag(ZERO_FLAG, n == 0);
        // Set the subtract flag to 0
        m_registers.setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag if there is a carry from bit 3
        m_registers.setFlag(HALF_CARRY_FLAG, (n & 0x0F) == 0);
        // Carry flag not affected
    }

    void CPU::dec(uint8_t &n)
    {
        // Decrement n
        n--;

        // Set the zero flag if the result is 0
        m_registers.setFlag(ZERO_FLAG, n == 0);
        // Set the subtract flag to 1
        m_registers.setFlag(SUBTRACT_FLAG, true);
        // Set the half-carry flag if there is a borrow from bit 4
        m_registers.setFlag(HALF_CARRY_FLAG, (n & 0x0F) == 0x0F);
        // Carry flag not affected
    }

    void CPU::add_hl(uint16_t nn)
    {
        unsigned int resultFull = m_registers.getHL() + nn; // Save the result in a temporary variable to check for carry from bit 15

        // Zero flag not affected
        // Set the subtract flag to 0
        m_registers.setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag if there is a carry from bit 11
        m_registers.setFlag(HALF_CARRY_FLAG, (m_registers.getHL() & 0x0FFF) + (nn & 0x0FFF) > 0x0FFF);
        // Set the carry flag if there is a carry from bit 15
        m_registers.setFlag(CARRY_FLAG, resultFull > 0xFFFF);

        // Set the value of the register HL to the result
        m_registers.setHL(static_cast<uint16_t>(resultFull));
    }

    void CPU::add_sp(int8_t n)
    {
        unsigned int resultFull = m_registers.sp + n;

        // Set the zero flag to 0
        m_registers.setFlag(ZERO_FLAG, false);
        // Set the subtract flag to 0
        m_registers.setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag according to operation
        m_registers.setFlag(HALF_CARRY_FLAG, ((m_registers.sp ^ n ^ (resultFull & 0xFFFF)) & 0x10) == 0x10);
        // Set the carry flag according to operation
        m_registers.setFlag(CARRY_FLAG, ((m_registers.sp ^ n ^ (resultFull & 0xFFFF)) & 0x100) == 0x100);

        // Set the value of the register SP to the result
        m_registers.sp = static_cast<uint16_t>(resultFull);
    }

    void CPU::ldhl(int8_t n)
    {
        unsigned int resultFull = m_registers.sp + n;

        // Set the zero flag to 0
        m_registers.setFlag(ZERO_FLAG, false);
        // Set the subtract flag to 0
        m_registers.setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag according to operation
        m_registers.setFlag(HALF_CARRY_FLAG, ((m_registers.sp ^ n ^ (resultFull & 0xFFFF)) & 0x10) == 0x10);
        // Set the carry flag according to operation
        m_registers.setFlag(CARRY_FLAG, ((m_registers.sp ^ n ^ (resultFull & 0xFFFF)) & 0x100) == 0x100);

        // Set the value of the register SP to the result
        m_registers.setHL(static_cast<uint16_t>(resultFull));
    }

    void CPU::swap(uint8_t &n)
    {
        // Swap the upper and lower nibbles of n
        n = (n & 0x0F) << 4 | (n & 0xF0) >> 4;

        // Set the zero flag if the result is 0
        m_registers.setFlag(ZERO_FLAG, n == 0);
        // Set the subtract flag to 0
        m_registers.setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 0
        m_registers.setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag to 0
        m_registers.setFlag(CARRY_FLAG, false);
    }

    void CPU::daa()
    {
        // See https://en.wikipedia.org/wiki/Binary-coded_decimal

        uint8_t a = m_registers.a;
        uint8_t adjust = m_registers.getFlag(CARRY_FLAG) ? 0x60 : 0x00;

        if (m_registers.getFlag(HALF_CARRY_FLAG))
            adjust |= 0x06;

        if (m_registers.getFlag(SUBTRACT_FLAG))
            a -= adjust;
        else
        {
            if ((a & 0x0F) > 0x09)
                adjust |= 0x06;

            if (a > 0x99)
                adjust |= 0x60;

            a += adjust;
        }

        // Set the zero flag if the result is 0
        m_registers.setFlag(ZERO_FLAG, a == 0);
        // Subtract flag not affected
        // Set the half-carry flag to 0
        m_registers.setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag if there is a carry from bit 7
        m_registers.setFlag(CARRY_FLAG, adjust >= 0x60);

        m_registers.a = a;
    }

    void CPU::cpl()
    {
        // Zero flag not affected
        // Set the subtract flag to 1
        m_registers.setFlag(SUBTRACT_FLAG, true);
        // Set the half-carry flag to 1
        m_registers.setFlag(HALF_CARRY_FLAG, true);
        // Carry flag not affected

        // Complement the value of the register A
        m_registers.a = ~m_registers.a;
    }

    void CPU::ccf()
    {
        // Zero flag not affected
        // Set the subtract flag to 0
        m_registers.setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 0
        m_registers.setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag to the value of the carry flag
        m_registers.setFlag(CARRY_FLAG, !m_registers.getFlag(CARRY_FLAG));
    }

    void CPU::scf()
    {
        // Zero flag not affected
        // Set the subtract flag to 0
        m_registers.setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 0
        m_registers.setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag to 1
        m_registers.setFlag(CARRY_FLAG, true);
    }

    void CPU::halt()
    {
        m_halted = true;
    }

    void CPU::di()
    {
        m_ime = false;
    }

    void CPU::ei()
    {
        m_ime = true;
    }

    void CPU::rlca()
    {
        rlc(m_registers.a);
        // Set the zero flag to 0
        m_registers.setFlag(ZERO_FLAG, false);
    }

    void CPU::rla()
    {
        rl(m_registers.a);
        // Set the zero flag to 0
        m_registers.setFlag(ZERO_FLAG, false);
    }

    void CPU::rrca()
    {
        rrc(m_registers.a);
        // Set the zero flag to 0
        m_registers.setFlag(ZERO_FLAG, false);
    }

    void CPU::rra()
    {
        rr(m_registers.a);
        // Set the zero flag to 0
        m_registers.setFlag(ZERO_FLAG, false);
    }

    void CPU::rlc(uint8_t &n)
    {
        uint8_t carry = n & 0x80 ? 1 : 0;

        uint8_t result = (n << 1) | carry;

        // Set the zero flag if the result is 0
        m_registers.setFlag(ZERO_FLAG, result == 0);
        // Set the subtract flag to 0
        m_registers.setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 0
        m_registers.setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag to the value of old bit 7 of n
        m_registers.setFlag(CARRY_FLAG, carry == 1);

        n = result;
    }

    void CPU::rl(uint8_t &n)
    {
        uint8_t carry = m_registers.getFlag(CARRY_FLAG) ? 1 : 0;

        uint8_t result = (n << 1) | carry;

        // Set the zero flag if the result is 0
        m_registers.setFlag(ZERO_FLAG, result == 0);
        // Set the subtract flag to 0
        m_registers.setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 0
        m_registers.setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag to the value of old bit 7 of n
        m_registers.setFlag(CARRY_FLAG, n & 0x80 ? 1 : 0);

        n = result;
    }

    void CPU::rrc(uint8_t &n)
    {
        uint8_t carry = n & 0x01 ? 1 : 0;

        uint8_t result = (n >> 1) | (carry << 7);

        // Set the zero flag if the result is 0
        m_registers.setFlag(ZERO_FLAG, result == 0);
        // Set the subtract flag to 0
        m_registers.setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 0
        m_registers.setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag to the value of old bit 0 of n
        m_registers.setFlag(CARRY_FLAG, carry == 1);

        n = result;
    }

    void CPU::rr(uint8_t &n)
    {
        uint8_t carry = m_registers.getFlag(CARRY_FLAG) ? 1 : 0;

        uint8_t result = (n >> 1) | (carry << 7);

        // Set the zero flag if the result is 0
        m_registers.setFlag(ZERO_FLAG, result == 0);
        // Set the subtract flag to 0
        m_registers.setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 0
        m_registers.setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag to the value of old bit 0 of n
        m_registers.setFlag(CARRY_FLAG, n & 0x01 ? 1 : 0);

        n = result;
    }

    void CPU::sla(uint8_t &n)
    {
        uint8_t carry = n & 0x80 ? 1 : 0;

        uint8_t result = n << 1;

        // Set the zero flag if the result is 0
        m_registers.setFlag(ZERO_FLAG, result == 0);
        // Set the subtract flag to 0
        m_registers.setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 0
        m_registers.setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag to the value of old bit 7 of n
        m_registers.setFlag(CARRY_FLAG, carry == 1);

        n = result;
    }

    void CPU::sra(uint8_t &n)
    {
        uint8_t carry = n & 0x01 ? 1 : 0;

        uint8_t result = (n >> 1) | (n & 0x80);

        // Set the zero flag if the result is 0
        m_registers.setFlag(ZERO_FLAG, result == 0);
        // Set the subtract flag to 0
        m_registers.setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 0
        m_registers.setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag to the value of old bit 0 of n
        m_registers.setFlag(CARRY_FLAG, carry == 1);

        n = result;
    }

    void CPU::srl(uint8_t &n)
    {
        uint8_t carry = n & 0x01 ? 1 : 0;

        uint8_t result = n >> 1;

        // Set the zero flag if the result is 0
        m_registers.setFlag(ZERO_FLAG, result == 0);
        // Set the subtract flag to 0
        m_registers.setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 0
        m_registers.setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag to the value of old bit 0 of n
        m_registers.setFlag(CARRY_FLAG, carry == 1);

        n = result;
    }

    void CPU::bit(uint8_t b, uint8_t r)
    {
        // Set the zero flag if bit b of register r is 0
        m_registers.setFlag(ZERO_FLAG, (r & (1 << b)) == 0);
        // Set the subtract flag to 0
        m_registers.setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 1
        m_registers.setFlag(HALF_CARRY_FLAG, true);
        // Carry flag not affected
    }

    void CPU::set(uint8_t b, uint8_t &r)
    {
        r |= (1 << b);
    }

    void CPU::res(uint8_t b, uint8_t &r)
    {
        r &= ~(1 << b);
    }

    void CPU::jp()
    {
        uint16_t address = m_memory->readWord(m_registers.pc);
        m_registers.pc = address;
    }

    void CPU::jp(bool condition)
    {
        if (condition)
        {
            jp();
            branched = true;
        }
        else
            m_registers.pc += 2;
    }

    void CPU::jr()
    {
        int8_t offset = m_memory->read(m_registers.pc);
        m_registers.pc++;
        m_registers.pc += offset;
    }

    void CPU::jr(bool condition)
    {
        if (condition)
        {
            jr();
            branched = true;
        }
        else
            m_registers.pc++;
    }

    void CPU::call()
    {
        uint16_t address = m_memory->readWord(m_registers.pc);
        push(m_registers.pc + 2);
        m_registers.pc = address;
    }

    void CPU::call(bool condition)
    {
        if (condition)
        {
            call();
            branched = true;
        }
        else
            m_registers.pc += 2;
    }

    void CPU::rst(uint8_t n)
    {
        push(m_registers.pc);
        m_registers.pc = n;
    }

    void CPU::ret()
    {
        m_registers.pc = pop();
    }

    void CPU::ret(bool condition)
    {
        if (condition)
        {
            ret();
            branched = true;
        }
    }

    void CPU::reti()
    {
        ret();
        ei();
    }
} // namespace gameboy
