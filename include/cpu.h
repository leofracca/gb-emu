/**
 * @file cpu.h
 * @brief This file contains the declaration of the CPU class.
 *        It emulates the behavior of the CPU.
 */

/*
 * See pages 61 to 118 of the documentation (PanDocs/GB.pdf)
 * See https://gbdev.io/pandocs/CPU_Instruction_Set.html
 * See https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
 */

#pragma once

#include "memory.h" // Memory
#include "registers.h" // Registers

namespace gameboy
{
    namespace cpu_cycles
    {
        // clang-format off
        constexpr uint8_t OPCODE_CYCLES[256] =
        {// 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
            1, 3, 2, 2, 1, 1, 2, 1, 5, 2, 2, 2, 1, 1, 2, 1, // 0
            1, 3, 2, 2, 1, 1, 2, 1, 3, 2, 2, 2, 1, 1, 2, 1, // 1
            2, 3, 2, 2, 1, 1, 2, 1, 2, 2, 2, 2, 1, 1, 2, 1, // 2
            2, 3, 2, 2, 3, 3, 3, 1, 2, 2, 2, 2, 1, 1, 2, 1, // 3
            1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 4
            1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 5
            1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 6
            2, 2, 2, 2, 2, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, // 7
            1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 8
            1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 9
            1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // A
            1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // B
            2, 3, 3, 4, 3, 4, 2, 4, 2, 4, 3, 0, 3, 6, 2, 4, // C
            2, 3, 3, 0, 3, 4, 2, 4, 2, 4, 3, 0, 3, 0, 2, 4, // D
            3, 3, 2, 0, 0, 4, 2, 4, 4, 1, 4, 0, 0, 0, 2, 4, // E
            3, 3, 2, 1, 0, 4, 2, 4, 3, 2, 4, 1, 0, 0, 2, 4  // F
        }; ///< Opcodes machine cycles

        constexpr uint8_t OPCODE_CYCLES_BRANCHED[256] =
        {// 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
            1, 3, 2, 2, 1, 1, 2, 1, 5, 2, 2, 2, 1, 1, 2, 1, // 0
            1, 3, 2, 2, 1, 1, 2, 1, 3, 2, 2, 2, 1, 1, 2, 1, // 1
            3, 3, 2, 2, 1, 1, 2, 1, 3, 2, 2, 2, 1, 1, 2, 1, // 2
            3, 3, 2, 2, 3, 3, 3, 1, 3, 2, 2, 2, 1, 1, 2, 1, // 3
            1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 4
            1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 5
            1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 6
            2, 2, 2, 2, 2, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, // 7
            1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 8
            1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 9
            1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // A
            1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // B
            5, 3, 4, 4, 6, 4, 2, 4, 5, 4, 4, 0, 6, 6, 2, 4, // C
            5, 3, 4, 0, 6, 4, 2, 4, 5, 4, 4, 0, 6, 0, 2, 4, // D
            3, 3, 2, 0, 0, 4, 2, 4, 4, 1, 4, 0, 0, 0, 2, 4, // E
            3, 3, 2, 1, 0, 4, 2, 4, 3, 2, 4, 1, 0, 0, 2, 4  // F
        }; ///< Opcodes machine cycles when branched (jump, call, return)

        constexpr uint8_t OPCODE_CB_CYCLES[256] =
        {// 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
            2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // 0
            2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // 1
            2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // 2
            2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // 3
            2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2, // 4
            2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2, // 5
            2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2, // 6
            2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2, // 7
            2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // 8
            2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // 9
            2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // A
            2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // B
            2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // C
            2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // D
            2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // E
            2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2  // F
        }; ///< Opcodes cb-prefixed machine cycles
        // clang-format on
    } // namespace cpu_cycles

    /**
     * @brief CPU class that emulates the behavior of the CPU (logic and arithmetic).
     */
    class CPU
    {
    public:
        /**
         * @brief Initialize the CPU.
         *
         * @param memory The memory
         */
        explicit CPU(Memory &memory);

        /**
         * @brief Get the opcode of the next instruction, increment the program counter and execute the instruction.
         *
         * @return The number of cycles used by the instruction, 0 if the instruction does not exist.
         */
        uint8_t cycle();

    private:
        Memory &m_memory; ///< The memory
        Registers m_registers; ///< The registers

        bool m_halted = false; ///< True if the cpu is halted
        bool m_ime = true; ///< True if the cpu is in the interrupt master enable mode

        bool m_branched = false; // Used to check if a branch was taken (for conditional opcodes (jump, call, return))

        static constexpr uint16_t LD_START_ADDRESS = 0xFF00; ///< Start address of instructions with opcode 0xE0, 0xE2, 0xF0, 0xF2

        /**
         * @brief 0x40: VBLANK interrupt start address
         *        0x48: LCD STAT interrupt start address
         *        0x50: TIMER interrupt start address
         *        0x58: SERIAL interrupt start address
         *        0x60: JOYPAD interrupt start address
         */
        static constexpr uint16_t INTERRUPT_ADDRESS[5] = {0x40, 0x48, 0x50, 0x58, 0x60};

        /**
         * @brief Check if interrupts are enabled and if an interrupt has been requested.
         *        Then execute the interrupt if it is the case.
         *
         * @return 0 if no interrupt has been executed, 5 if an interrupt has been executed.
         * @see handleInterrupt
         */
        uint8_t handleInterrupts();

        /**
         * @brief Execute the interrupt.
         *
         * @param interruptBit The bit of the interrupt to execute.
         * @param interruptAddress The address of the interrupt to execute.
         * @param interruptFlagBit The bit of the interrupt flag to reset.
         * @return True if the interrupt has been executed, false otherwise.
         */
        bool handleInterrupt(uint8_t interruptBit, uint16_t interruptAddress, uint8_t interruptFlagBit);

        /**
         * @brief Executes the next instruction.
         * @details Call the appropriate function depending on the opcode and execute the next instruction.
         *
         * @param opcode The opcode of the instruction.
         * @return The number of cycles used by the instruction or 0 if the opcode does not exist.
         */
        uint8_t executeOpcode(uint8_t opcode);

        /**
         * @brief Executes the next instruction.
         * @details Call the appropriate function depending on the cb opcode and execute the next instruction.
         *
         * @param opcode The cb opcode of the instruction.
         * @return The number of cycles used by the instruction or 0 if the opcode does not exist.
         */
        uint8_t executeOpcodeCB(uint8_t opcode);

        /**
         * @brief Log an invalid opcode
         *
         * @param opcode The invalid opcode
         */
        static void logUnexpectedOpcode(uint8_t opcode);

        // Push and pop
        /**
         * @brief Push a value onto the stack.
         *
         * @param value The value to push onto the stack.
         */
        void push(uint16_t value);

        /**
         * @brief Pop a value from the stack.
         *
         * @return The value popped from the stack.
         */
        uint16_t pop();

        // CPU instructions
        /**
         * @brief Add the value n to A.
         * @details Use with:
         *            n = A, B, C, D, E, H, L, (HL), #
         *
         *          Flags affected:
         *            Z - Set if result is zero.
         *            N - Reset.
         *            H - Set if carry from bit 3.
         *            C - Set if carry from bit 7.
         *
         * @param n The value to add to A.
         */
        void add(uint8_t n);

        /**
         * @brief Add n + Carry flag to A.
         * @details Use with:
         *            n = A, B, C, D, E, H, L, (HL), #
         *
         *          Flags affected:
         *            Z - Set if result is zero.
         *            N - Reset.
         *            H - Set if carry from bit 3.
         *            C - Set if carry from bit 7.
         *
         * @param n The value to add to A.
         */
        void adc(uint8_t n);

        /**
         * @brief Subtract n from A.
         * @details Use with:
         *            n = A, B, C, D, E, H, L, (HL), #
         *
         *          Flags affected:
         *            Z - Set if result is zero.
         *            N - Set.
         *            H - Set if borrow from bit 4.
         *            C - Set if borrow.
         *
         * @param n The value to subtract from A.
         */
        void sub(uint8_t n);

        /**
         * @brief Subtract n + Carry flag from A.
         * @details Use with:
         *            n = A, B, C, D, E, H, L, (HL), #
         *
         *          Flags affected:
         *            Z - Set if result is zero.
         *            N - Set.
         *            H - Set if borrow from bit 4.
         *            C - Set if borrow.
         *
         * @param n The value to subtract from A.
         */
        void sbc(uint8_t n);

        /**
         * @brief AND n with A, result in A.
         * @details Use with:
         *            n = A, B, C, D, E, H, L, (HL), #
         *
         *          Flags affected:
         *            Z - Set if result is zero.
         *            N - Reset.
         *            H - Set.
         *            C - Reset.
         *
         * @param n The value to AND with A.
         */
        void and_(uint8_t n);

        /**
         * @brief OR n with A, result in A.
         * @details Use with:
         *            n = A, B, C, D, E, H, L, (HL), #
         *
         *          Flags affected:
         *            Z - Set if result is zero.
         *            N - Reset.
         *            H - Reset.
         *            C - Reset.
         *
         * @param n The value to OR with A.
         */
        void or_(uint8_t n);

        /**
         * @brief XOR n with A, result in A.
         * @details Use with:
         *            n = A, B, C, D, E, H, L, (HL), #
         *
         *          Flags affected:
         *            Z - Set if result is zero.
         *            N - Reset.
         *            H - Reset.
         *            C - Reset.
         *
         * @param n The value to XOR with A.
         */
        void xor_(uint8_t n);

        /**
         * @brief Compare A with n. This is basically an A - n
         *        subtraction instruction but the results are thrown
         *        away.
         * @details Use with:
         *            n = A, B, C, D, E, H, L, (HL), #
         *
         *          Flags affected:
         *            Z - Set if result is zero. (Set if A = n.)
         *            N - Set.
         *            H - Set if borrow from bit 4.
         *            C - Set if borrow. (Set if A < n.)
         *
         * @param n The value to compare with A.
         */
        void cp(uint8_t n);

        /**
         * @brief Increment register n.
         * @details Use with:
         *            n = A, B, C, D, E, H, L, (HL)
         *
         *          Flags affected:
         *            Z - Set if result is zero.
         *            N - Reset.
         *            H - Set if carry from bit 3.
         *            C - Not affected.
         *
         * @param n The register to increment.
         */
        void inc(uint8_t &n);

        /**
         * @brief Decrement register n.
         * @details Use with:
         *            n = A, B, C, D, E, H, L, (HL)
         *
         *          Flags affected:
         *            Z - Set if result is zero.
         *            N - Set.
         *            H - Set if borrow from bit 4.
         *            C - Not affected.
         *
         * @param n The register to decrement.
         */
        void dec(uint8_t &n);

        /**
         * @brief Add n to HL.
         * @details Use with:
         *            n = BC, DE, HL, SP
         *
         *          Flags affected:
         *            Z - Not affected.
         *            N - Reset.
         *            H - Set if carry from bit 11.
         *            C - Set if carry from bit 15.
         *
         * @param nn The value to add to HL.
         */
        void add_hl(uint16_t nn);

        /**
         * @brief Add n to SP (Stack Pointer).
         * @details Use with:
         *            n = one byte signed immediate value (#)
         *
         *          Flags affected:
         *            Z - Reset.
         *            N - Reset.
         *            H - Set or reset according to operation.
         *            C - Set or reset according to operation.
         *
         * @param n The value to add to SP.
         */
        void add_sp(int8_t n);

        /**
         * @brief Put SP + n effective address into HL.
         * @details Use with:
         *            n = one byte signed immediate value (#)
         *
         *          Flags affected:
         *            Z - Reset.
         *            N - Reset.
         *            H - Set or reset according to operation.
         *            C - Set or reset according to operation.
         *
         * @param n The value to add to SP.
         */
        void ldhl(int8_t n);

        /**
         * @brief Swap upper & lower nibbles of n.
         * @details Use with:
         *            n = A, B, C, D, E, H, L, (HL)
         *
         *          Flags affected:
         *            Z - Set if result is zero.
         *            N - Reset.
         *            H - Reset.
         *            C - Reset.
         *
         * @param n The value to load into the register.
         */
        void swap(uint8_t &n);

        /**
         * @brief Decimal adjust register A. This instruction adjusts register A so that the
         *        correct representation of Binary Coded Decimal (BCD)
         *        is obtained.
         * @details Flags affected:
         *            Z - Set if register A is zero.
         *            N - Not affected.
         *            H - Reset.
         *            C - Set or reset according to operation.
         */
        void daa();

        /**
         * @brief Complement A register. (Flip all bits.)
         * @details Flags affected:
         *            Z - Not affected.
         *            N - Set.
         *            H - Set.
         *            C - Not affected.
         */
        void cpl();

        /**
         * @brief Complement Carry flag.
         * @details Flags affected:
         *            Z - Not affected.
         *            N - Reset.
         *            H - Reset.
         *            C - Complemented.
         */
        void ccf();

        /**
         * @brief Set Carry flag.
         * @details Flags affected:
         *            Z - Not affected.
         *            N - Reset.
         *            H - Reset.
         *            C - Set.
         */
        void scf();

        /**
         * @brief Power down CPU until an interrupt occurs.
         *        Use this whenever possible to reduce power consumption.
         */
        void halt();

        /**
         * @brief This instruction disables interrupts but not
         *        immediately. Interrupts are disabled after
         *        instruction after DI is executed.
         */
        void di();

        /**
         * @brief Enable interrupts. This instruction enables interrupts
         *        but not immediately. Interrupts are enabled after
         *        instruction after EI is executed.
         */
        void ei();

        /**
         * @brief Rotate A left. Old bit 7 to Carry flag.
         * @details Flags affected:
         *            Z - Reset.
         *            N - Reset.
         *            H - Reset.
         *            C - Contains old bit 7 data.
         */
        void rlca();

        /**
         * @brief Rotate A left through Carry flag.
         * @details Flags affected:
         *            Z - Reset.
         *            N - Reset.
         *            H - Reset.
         *            C - Contains old bit 0 data.
         */
        void rla();

        /**
         * @brief Rotate A right. Old bit 0 to Carry flag.
         * @details Flags affected:
         *            Z - Reset.
         *            N - Reset.
         *            H - Reset.
         *            C - Contains old bit 0 data.
         */
        void rrca();

        /**
         * @brief Rotate A right through Carry flag.
         * @details Flags affected:
         *            Z - Reset.
         *            N - Reset.
         *            H - Reset.
         *            C - Contains old bit 0 data.
         */
        void rra();

        /**
         * @brief Rotate n left. Old bit 7 to Carry flag.
         * @details Use with:
         *            n = A, B, C, D, E, H, L, (HL)
         *
         *          Flags affected:
         *            Z - Set if result is zero.
         *            N - Reset.
         *            H - Reset.
         *            C - Contains old bit 7 data.
         *
         * @param n The value to rotate.
         */
        void rlc(uint8_t &n);

        /**
         * @brief Rotate n left through Carry flag.
         * @details Use with:
         *            n = A, B, C, D, E, H, L, (HL)
         *
         *          Flags affected:
         *            Z - Set if result is zero.
         *            N - Reset.
         *            H - Reset.
         *            C - Contains old bit 7 data.
         *
         * @param n The value to rotate.
         */
        void rl(uint8_t &n);

        /**
         * @brief Rotate n right. Old bit 0 to Carry flag.
         * @details Use with:
         *            n = A, B, C, D, E, H, L, (HL)
         *
         *          Flags affected:
         *            Z - Set if result is zero.
         *            N - Reset.
         *            H - Reset.
         *            C - Contains old bit 0 data.
         *
         * @param n The value to rotate.
         */
        void rrc(uint8_t &n);

        /**
         * @brief Rotate n right through Carry flag.
         * @details Use with:
         *            n = A, B, C, D, E, H, L, (HL)
         *
         *          Flags affected:
         *            Z - Set if result is zero.
         *            N - Reset.
         *            H - Reset.
         *            C - Contains old bit 0 data.
         *
         * @param n The value to rotate.
         */
        void rr(uint8_t &n);

        /**
         * @brief Shift n left into Carry. LSB of n set to 0.
         * @details Use with:
         *            n = A, B, C, D, E, H, L, (HL)
         *
         *          Flags affected:
         *            Z - Set if result is zero.
         *            N - Reset.
         *            H - Reset.
         *            C - Contains old bit 7 data.
         *
         * @param n The value to shift.
         */
        void sla(uint8_t &n);

        /**
         * @brief Shift n right into Carry. MSB doesn't change.
         * @details Use with:
         *            n = A, B, C, D, E, H, L, (HL)
         *
         *          Flags affected:
         *            Z - Set if result is zero.
         *            N - Reset.
         *            H - Reset.
         *            C - Contains old bit 0 data.
         *
         * @param n The value to shift.
         */
        void sra(uint8_t &n);

        /**
         * @brief Shift n right into Carry. MSB of n set to 0.
         * @details Use with:
         *            n = A, B, C, D, E, H, L, (HL)
         *
         *          Flags affected:
         *            Z - Set if result is zero.
         *            N - Reset.
         *            H - Reset.
         *            C - Contains old bit 0 data.
         *
         * @param n The value to shift.
         */
        void srl(uint8_t &n);

        /**
         * @brief Test bit b in register r.
         * @details Use with:
         *            b = 0 - 7, r = A, B, C, D, E, H, L, (HL)
         *
         *          Flags affected:
         *            Z - Set if bit b of register r is 0.
         *            N - Reset.
         *            H - Set.
         *            C - Not affected.
         *
         * @param b The bit to test.
         * @param r The register to test.
         */
        void bit(uint8_t b, uint8_t r);

        /**
         * @brief Set bit b in register r.
         * @details Use with:
         *            b = 0 - 7, r = A, B, C, D, E, H, L, (HL)
         *
         *          Flags affected:
         *            None.
         *
         * @param b The bit to set.
         * @param r The register to set.
         */
        static void set(uint8_t b, uint8_t &r);

        /**
         * @brief Reset bit b in register r.
         * @details Use with:
         *            b = 0 - 7, r = A, B, C, D, E, H, L, (HL)
         *
         *          Flags affected:
         *            None.
         *
         * @param b The bit to reset.
         * @param r The register to reset.
         */
        static void res(uint8_t b, uint8_t &r);

        /**
         * @brief Jump to address nn.
         * @details Use with:
         *            nn = two byte immediate value.
         */
        void jp();

        /**
         * @brief Jump to address nn if the condition is true.
         * @details Use with:
         *            nn = two byte immediate value.
         *
         *          The condition can be:
         *            NZ - Jump if Z flag is reset.
         *            Z - Jump if Z flag is set.
         *            NC - Jump if C flag is reset.
         *            C - Jump if C flag is set.
         *
         * @param condition The condition to test.
         */
        void jp(bool condition);

        /**
         * @brief Add n to current address and jump to it.
         * @details Use with:
         *            n = one byte signed immediate value
         */
        void jr();

        /**
         * @brief Add n to current address and jump to it if the condition is true.
         * @details Use with:
         *            n = one byte signed immediate value
         *
         *          The condition can be:
         *            NZ - Jump if Z flag is reset.
         *            Z - Jump if Z flag is set.
         *            NC - Jump if C flag is reset.
         *            C - Jump if C flag is set.
         *
         * @param condition The condition to test.
         */
        void jr(bool condition);

        /**
         * @brief Push address of next instruction onto stack and then jump to nn.
         * @details Use with:
         *          nn = two byte immediate value.
         */
        void call();

        /**
         * @brief Call address nn if the condition is true.
         * @details Use with:
         *          nn = two byte immediate value.
         *
         *          The condition can be:
         *            NZ - Jump if Z flag is reset.
         *            Z - Jump if Z flag is set.
         *            NC - Jump if C flag is reset.
         *            C - Jump if C flag is set.
         *
         * @param condition The condition to test.
         */
        void call(bool condition);

        /**
         * @brief Push present address onto stack.
         *        Then jump to address $0000 + n.
         * @details Use with:
         *          n = $00,$08,$10,$18,$20,$28,$30,$38
         *
         * @param n The address to jump to.
         */
        void rst(uint8_t n);

        /**
         * @brief Pop two bytes from stack & jump to that address.
         */
        void ret();

        /**
         * @brief Pop two bytes from stack & jump to that address if the condition is true.
         * @details The condition can be:
         *            NZ - Jump if Z flag is reset.
         *            Z - Jump if Z flag is set.
         *            NC - Jump if C flag is reset.
         *            C - Jump if C flag is set.
         *
         * @param condition The condition to test.
         */
        void ret(bool condition);

        /**
         * @brief Pop two bytes from stack & jump to that address. Then enable interrupts.
         */
        void reti();
    };
} // namespace gameboy
