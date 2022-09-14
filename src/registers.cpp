/*
 * See pages 17-18 and 60 to 64 of the documentation (docs/GB.pdf)
 * See https://gbdev.io/pandocs/CPU_Registers_and_Flags.html
 * See https://gbdev.io/pandocs/Power_Up_Sequence.html
 */

#include "../include/registers.h" // Registers

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

    void Registers::setFlag(uint8_t flag, bool value) {
        if (value)
            f |= flag;
        else
            f &= ~flag;
    }

    bool Registers::getFlag(uint8_t flag) const {
        return f & flag;
    }
} // namespace gameboy
