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

    void CPU::push(uint16_t value)
    {
        m_registers->sp -= 2;
        m_memory->writeWord(m_registers->sp, value);
    }

    uint16_t CPU::pop()
    {
        uint16_t value = m_memory->readWord(m_registers->sp);
        m_registers->sp += 2;
        return value;
    }

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

    void CPU::add_hl(uint16_t nn)
    {
        unsigned int resultFull = m_registers->hl + nn; // Save the result in a temporary variable to check for carry from bit 15

        // Zero flag not affected
        // Set the subtract flag to 0
        m_registers->setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag if there is a carry from bit 11
        m_registers->setFlag(HALF_CARRY_FLAG, (m_registers->hl & 0x0FFF) + (nn & 0x0FFF) > 0x0FFF);
        // Set the carry flag if there is a carry from bit 15
        m_registers->setFlag(CARRY_FLAG, resultFull > 0xFFFF);

        // Set the value of the register HL to the result
        m_registers->hl = static_cast<uint16_t>(resultFull);
    }

    void CPU::add_sp(int8_t n)
    {
        unsigned int resultFull = m_registers->sp + n;

        // Set the zero flag to 0
        m_registers->setFlag(ZERO_FLAG, false);
        // Set the subtract flag to 0
        m_registers->setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag according to operation
        m_registers->setFlag(HALF_CARRY_FLAG, ((m_registers->sp ^ n ^ (resultFull & 0xFFFF)) & 0x10) == 0x10);
        // Set the carry flag according to operation
        m_registers->setFlag(HALF_CARRY_FLAG, ((m_registers->sp ^ n ^ (resultFull & 0xFFFF)) & 0x100) == 0x100);

        // Set the value of the register SP to the result
        m_registers->sp = static_cast<uint16_t>(resultFull);
    }

    void CPU::ldhl(int8_t n)
    {
        unsigned int resultFull = m_registers->sp + n;

        // Set the zero flag to 0
        m_registers->setFlag(ZERO_FLAG, false);
        // Set the subtract flag to 0
        m_registers->setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag according to operation
        m_registers->setFlag(HALF_CARRY_FLAG, ((m_registers->sp ^ n ^ (resultFull & 0xFFFF)) & 0x10) == 0x10);
        // Set the carry flag according to operation
        m_registers->setFlag(HALF_CARRY_FLAG, ((m_registers->sp ^ n ^ (resultFull & 0xFFFF)) & 0x100) == 0x100);

        // Set the value of the register SP to the result
        m_registers->hl = static_cast<uint16_t>(resultFull);
    }

    void CPU::swap(uint8_t &n)
    {
        // Swap the upper and lower nibbles of n
        n = (n & 0x0F) << 4 | (n & 0xF0) >> 4;

        // Set the zero flag if the result is 0
        m_registers->setFlag(ZERO_FLAG, n == 0);
        // Set the subtract flag to 0
        m_registers->setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 0
        m_registers->setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag to 0
        m_registers->setFlag(CARRY_FLAG, false);
    }

    void CPU::daa()
    {
        // TODO
        // See https://en.wikipedia.org/wiki/Binary-coded_decimal
    }

    void CPU::cpl()
    {
        // Zero flag not affected
        // Set the subtract flag to 1
        m_registers->setFlag(SUBTRACT_FLAG, true);
        // Set the half-carry flag to 1
        m_registers->setFlag(HALF_CARRY_FLAG, true);
        // Carry flag not affected

        // Complement the value of the register A
        m_registers->a = ~m_registers->a;
    }

    void CPU::ccf()
    {
        // Zero flag not affected
        // Set the subtract flag to 0
        m_registers->setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 0
        m_registers->setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag to the value of the carry flag
        m_registers->setFlag(CARRY_FLAG, !m_registers->getFlag(CARRY_FLAG));
    }

    void CPU::scf()
    {
        // Zero flag not affected
        // Set the subtract flag to 0
        m_registers->setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 0
        m_registers->setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag to 1
        m_registers->setFlag(CARRY_FLAG, true);
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
        rlc(m_registers->a);
        // Set the zero flag to 0
        m_registers->setFlag(ZERO_FLAG, false);
    }

    void CPU::rla()
    {
        rl(m_registers->a);
        // Set the zero flag to 0
        m_registers->setFlag(ZERO_FLAG, false);
    }

    void CPU::rrca()
    {
        rrc(m_registers->a);
        // Set the zero flag to 0
        m_registers->setFlag(ZERO_FLAG, false);
    }

    void CPU::rra()
    {
        rr(m_registers->a);
        // Set the zero flag to 0
        m_registers->setFlag(ZERO_FLAG, false);
    }

    void CPU::rlc(uint8_t &n)
    {
        uint8_t carry = n & 0x80 ? 1 : 0;

        uint8_t result = (n << 1) | carry;

        // Set the zero flag if the result is 0
        m_registers->setFlag(ZERO_FLAG, result == 0);
        // Set the subtract flag to 0
        m_registers->setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 0
        m_registers->setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag to the value of old bit 7 of n
        m_registers->setFlag(CARRY_FLAG, carry == 1);

        n = result;
    }

    void CPU::rl(uint8_t &n)
    {
        uint8_t carry = m_registers->getFlag(CARRY_FLAG) ? 1 : 0;

        uint8_t result = (n << 1) | carry;

        // Set the zero flag if the result is 0
        m_registers->setFlag(ZERO_FLAG, result == 0);
        // Set the subtract flag to 0
        m_registers->setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 0
        m_registers->setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag to the value of old bit 7 of n
        m_registers->setFlag(CARRY_FLAG, n & 0x80 ? 1 : 0);

        n = result;
    }

    void CPU::rrc(uint8_t &n)
    {
        uint8_t carry = n & 0x01 ? 1 : 0;

        uint8_t result = (n >> 1) | (carry << 7);

        // Set the zero flag if the result is 0
        m_registers->setFlag(ZERO_FLAG, result == 0);
        // Set the subtract flag to 0
        m_registers->setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 0
        m_registers->setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag to the value of old bit 0 of n
        m_registers->setFlag(CARRY_FLAG, carry == 1);

        n = result;
    }

    void CPU::rr(uint8_t &n)
    {
        uint8_t carry = m_registers->getFlag(CARRY_FLAG) ? 1 : 0;

        uint8_t result = (n >> 1) | (carry << 7);

        // Set the zero flag if the result is 0
        m_registers->setFlag(ZERO_FLAG, result == 0);
        // Set the subtract flag to 0
        m_registers->setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 0
        m_registers->setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag to the value of old bit 0 of n
        m_registers->setFlag(CARRY_FLAG, n & 0x01 ? 1 : 0);

        n = result;
    }

    void CPU::sla(uint8_t &n)
    {
        uint8_t carry = n & 0x80 ? 1 : 0;

        uint8_t result = n << 1;

        // Set the zero flag if the result is 0
        m_registers->setFlag(ZERO_FLAG, result == 0);
        // Set the subtract flag to 0
        m_registers->setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 0
        m_registers->setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag to the value of old bit 7 of n
        m_registers->setFlag(CARRY_FLAG, carry == 1);

        n = result;
    }

    void CPU::sra(uint8_t &n)
    {
        uint8_t carry = n & 0x01 ? 1 : 0;

        uint8_t result = (n >> 1) | (n & 0x80);

        // Set the zero flag if the result is 0
        m_registers->setFlag(ZERO_FLAG, result == 0);
        // Set the subtract flag to 0
        m_registers->setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 0
        m_registers->setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag to the value of old bit 0 of n
        m_registers->setFlag(CARRY_FLAG, carry == 1);

        n = result;
    }

    void CPU::srl(uint8_t &n)
    {
        uint8_t carry = n & 0x01 ? 1 : 0;

        uint8_t result = n >> 1;

        // Set the zero flag if the result is 0
        m_registers->setFlag(ZERO_FLAG, result == 0);
        // Set the subtract flag to 0
        m_registers->setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 0
        m_registers->setFlag(HALF_CARRY_FLAG, false);
        // Set the carry flag to the value of old bit 0 of n
        m_registers->setFlag(CARRY_FLAG, carry == 1);

        n = result;
    }

    void CPU::bit(uint8_t b, uint8_t r)
    {
        // Set the zero flag if bit b of register r is 0
        m_registers->setFlag(ZERO_FLAG, (r & (1 << b)) == 0);
        // Set the subtract flag to 0
        m_registers->setFlag(SUBTRACT_FLAG, false);
        // Set the half-carry flag to 1
        m_registers->setFlag(HALF_CARRY_FLAG, true);
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
        uint16_t address = m_memory->readWord(m_registers->pc);
        m_registers->pc = address;
    }

    void CPU::jp(bool condition)
    {
        if (condition)
            jp();
        else
            m_registers->pc += 2;

        // TODO: Handle cycles based on condition
    }

    void CPU::jr()
    {
        int8_t offset = m_memory->read(m_registers->pc);
        m_registers->pc++;
        m_registers->pc += offset;
    }

    void CPU::jr(bool condition)
    {
        if (condition)
            jr();
        else
            m_registers->pc++;

        // TODO: Handle cycles based on condition
    }

    void CPU::call()
    {
        uint16_t address = m_memory->readWord(m_registers->pc);
        push(m_registers->pc + 2);
        m_registers->pc = address;
    }

    void CPU::call(bool condition)
    {
        if (condition)
            call();
        else
            m_registers->pc += 2;

        // TODO: Handle cycles based on condition
    }

    void CPU::rst(uint8_t n)
    {
        push(m_registers->pc);
        m_registers->pc = n;
    }

    void CPU::ret()
    {
        m_registers->pc = pop();
    }

    void CPU::ret(bool condition)
    {
        if (condition)
            ret();

        // TODO: Handle cycles based on condition
    }

    void CPU::reti()
    {
        ret();
        ei();
    }
} // namespace gameboy
