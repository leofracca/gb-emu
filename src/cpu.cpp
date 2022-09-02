/*
 * See pages 61 to 118 of the documentation (docs/GB.pdf)
 * See https://gbdev.io/pandocs/CPU_Instruction_Set.html
 * See https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
 */

#include "../include/cpu.h" // CPU

namespace gameboy
{
    CPU::CPU(Memory *memory, Registers *registers)
        : m_memory(memory),
          m_registers(registers)
    {}

    void CPU::add(uint8_t n)
    {
        // Set the zero flag if the result is 0
        m_registers->setFlag(ZERO_FLAG, m_registers->a + n == 0);
        // Set the subtract flag to 0
        m_registers->setFlag(SUBTRACT_FLAG, false);
        // Set the half carry flag if there is a carry from bit 3
        m_registers->setFlag(HALF_CARRY_FLAG, (m_registers->a & 0x0F) + (n & 0x0F) > 0x0F);
        // Set the carry flag if there is a carry from bit 7
        m_registers->setFlag(CARRY_FLAG, m_registers->a + n > 0xFF);

        // Add n to the value of the register A
        m_registers->a += n;
    }

    void CPU::adc(uint8_t n)
    {
        uint8_t carry = m_registers->getFlag(CARRY_FLAG) ? 1 : 0;
        unsigned int resultFull = m_registers->a + n + carry; // Save the result in a temporary variable to check for carry from bit 7

        uint8_t result = static_cast<uint8_t>(resultFull); // Get only the lower 8 bits of the result

        // Set the zero flag if the result is 0
        m_registers->setFlag(ZERO_FLAG, result == 0);
        // Set the subtract flag to 0
        m_registers->setFlag(SUBTRACT_FLAG, false);
        // Set the half carry flag if there is a carry from bit 3
        m_registers->setFlag(HALF_CARRY_FLAG, (m_registers->a & 0x0F) + (n & 0x0F) + carry > 0x0F);
        // Set the carry flag if there is a carry from bit 7
        m_registers->setFlag(CARRY_FLAG, resultFull > 0xFF);

        // Set the value of the register A to the result
        m_registers->a = result;
    }

    void CPU::sub(uint8_t n)
    {
        // Set the zero flag if the result is 0
        m_registers->setFlag(ZERO_FLAG, m_registers->a == n);
        // Set the subtract flag to 1
        m_registers->setFlag(SUBTRACT_FLAG, true);
        // Set the half-carry flag if there is no borrow from bit 4
        m_registers->setFlag(HALF_CARRY_FLAG, (m_registers->a & 0x0F) < (n & 0x0F));
        // Set the carry flag if there is no borrow
        m_registers->setFlag(CARRY_FLAG, m_registers->a < n);

        // Subtract n from the value of the register A
        m_registers->a -= n;
    }

    void CPU::sbc(uint8_t n)
    {
        uint8_t carry = m_registers->getFlag(CARRY_FLAG) ? 1 : 0;
        int resultFull = m_registers->a - n - carry; // Save the result in a temporary variable to check for borrow from bit 7

        uint8_t result = static_cast<uint8_t>(resultFull); // Get only the lower 8 bits of the result

        // Set the zero flag if the result is 0
        m_registers->setFlag(ZERO_FLAG, result == 0);
        // Set the subtract flag to 1
        m_registers->setFlag(SUBTRACT_FLAG, true);
        // Set the half carry flag if there is no borrow from bit 4
        m_registers->setFlag(HALF_CARRY_FLAG, (m_registers->a & 0x0F) < (n & 0x0F) + carry);
        // Set the carry flag if there is no borrow
        m_registers->setFlag(CARRY_FLAG, resultFull < 0);

        // Set the value of the register A to the result
        m_registers->a = result;
    }

    void CPU::and_(uint8_t n)
    {
        // Set the zero flag if the result is 0
        m_registers->setFlag(ZERO_FLAG, (m_registers->a & n) == 0);
        // Set the subtract flag to 0
        m_registers->setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 1
        m_registers->setFlag(HALF_CARRY_FLAG, true);
        // Set the carry flag to 0
        m_registers->setFlag(CARRY_FLAG, false);

        // And the value of the register A with n
        m_registers->a &= n;
    }

    void CPU::or_(uint8_t n)
    {
        // Set the zero flag if the result is 0
        m_registers->setFlag(ZERO_FLAG, (m_registers->a | n) == 0);
        // Set the subtract flag to 0
        m_registers->setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 0
        m_registers->setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag to 0
        m_registers->setFlag(CARRY_FLAG, false);

        // Or the value of the register A with n
        m_registers->a |= n;
    }

    void CPU::xor_(uint8_t n)
    {
        // Set the zero flag if the result is 0
        m_registers->setFlag(ZERO_FLAG, (m_registers->a ^ n) == 0);
        // Set the subtract flag to 0
        m_registers->setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 0
        m_registers->setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag to 0
        m_registers->setFlag(CARRY_FLAG, false);

        // Xor the value of the register A with n
        m_registers->a ^= n;
    }

    void CPU::cp(uint8_t n)
    {
        // Set the zero flag if the result is 0
        m_registers->setFlag(ZERO_FLAG, m_registers->a == n);
        // Set the subtract flag to 1
        m_registers->setFlag(SUBTRACT_FLAG, true);
        // Set the half-carry flag if there is no borrow from bit 4
        m_registers->setFlag(HALF_CARRY_FLAG, (m_registers->a & 0x0F) < (n & 0x0F));
        // Set the carry flag if there is no borrow
        m_registers->setFlag(CARRY_FLAG, m_registers->a < n);
    }

    void CPU::inc(uint8_t &n)
    {
        // Increment n
        n++;

        // Set the zero flag if the result is 0
        m_registers->setFlag(ZERO_FLAG, n == 0);
        // Set the subtract flag to 0
        m_registers->setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag if there is a carry from bit 3
        m_registers->setFlag(HALF_CARRY_FLAG, (n & 0x0F) == 0);
        // Carry flag not affected
    }

    void CPU::dec(uint8_t &n)
    {
        // Decrement n
        n--;

        // Set the zero flag if the result is 0
        m_registers->setFlag(ZERO_FLAG, n == 0);
        // Set the subtract flag to 1
        m_registers->setFlag(SUBTRACT_FLAG, true);
        // Set the half-carry flag if there is a borrow from bit 4
        m_registers->setFlag(HALF_CARRY_FLAG, (n & 0x0F) == 0x0F);
        // Carry flag not affected
    }
}