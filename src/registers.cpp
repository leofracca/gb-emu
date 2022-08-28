/*
 * See pages 60 to 64 of the documentation (docs/GB.pdf)
 */

#include "../include/registers.h"// Registers

namespace gameboy
{
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
