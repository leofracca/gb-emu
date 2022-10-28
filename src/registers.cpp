/*
 * See pages 17-18 and 60 to 64 of the documentation (PanDocs/GB.pdf)
 * See https://gbdev.io/pandocs/CPU_Registers_and_Flags.html
 * See https://gbdev.io/pandocs/Power_Up_Sequence.html
 */

#include "registers.h" // Registers

namespace gameboy
{
    Registers::Registers()
    {
        a = 0x01;
        f = 0xB0;
        b = 0x00;
        c = 0x13;
        d = 0x00;
        e = 0xD8;
        h = 0x01;
        l = 0x4D;
        sp = 0xFFFE;
        pc = 0x0100;

        setFlag(ZERO_FLAG, true);
        setFlag(SUBTRACT_FLAG, false);
        setFlag(HALF_CARRY_FLAG, true);
        setFlag(CARRY_FLAG, true);
    }

    void Registers::setFlag(uint8_t flag, bool value)
    {
        if (value)
            f |= flag;
        else
            f &= ~flag;
    }

    bool Registers::getFlag(uint8_t flag) const
    {
        return f & flag;
    }

    uint16_t Registers::getAF() const
    {
        return getRegisterPair(a, f);
    }

    uint16_t Registers::getBC() const
    {
        return getRegisterPair(b, c);
    }

    uint16_t Registers::getDE() const
    {
        return getRegisterPair(d, e);
    }

    uint16_t Registers::getHL() const
    {
        return getRegisterPair(h, l);
    }

    void Registers::setAF(uint16_t value)
    {
        setRegisterPair(a, f, value);
    }

    void Registers::setBC(uint16_t value)
    {
        setRegisterPair(b, c, value);
    }

    void Registers::setDE(uint16_t value)
    {
        setRegisterPair(d, e, value);
    }

    void Registers::setHL(uint16_t value)
    {
        setRegisterPair(h, l, value);
    }

    uint16_t Registers::getRegisterPair(uint8_t high, uint8_t low)
    {
        return (high << 8) | low;
    }

    void Registers::setRegisterPair(uint8_t &high, uint8_t &low, uint16_t value)
    {
        high = (value >> 8) & 0xFF;
        low = value & 0xFF;
    }
} // namespace gameboy
