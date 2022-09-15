/*
 * See pages 30 to 40 of the documentation (docs/GB.pdf)
 * See https://gbdev.io/pandocs/Timer_and_Divider_Registers.html
 */

#include "../include/timer.h" // Timer

namespace gameboy
{
    Timer::Timer(Memory *memory) : m_memory(memory)
    {
        getRegisters();
    }

    void Timer::getRegisters()
    {
        m_div = m_memory->read(DIV_REG_ADDRESS);
        m_tima = m_memory->read(TIMA_REG_ADDRESS);
        m_tma = m_memory->read(TMA_REG_ADDRESS);
        m_tac = m_memory->read(TAC_REG_ADDRESS);
    }

    void Timer::setRegisters()
    {
        m_memory->write(DIV_REG_ADDRESS, m_div);
        m_memory->write(TIMA_REG_ADDRESS, m_tima);
        m_memory->write(TMA_REG_ADDRESS, m_tma);
        m_memory->write(TAC_REG_ADDRESS, m_tac);
    }

    void Timer::cycle(uint64_t cycles)
    {
        getRegisters();

        m_divCycles += cycles;
        if (m_divCycles >= 256)
        {
            m_divCycles -= 256;
            m_div++;
        }

        // Start the timer
        if (m_tac & 0x04)
        {
            m_timaCycles += cycles;

            /*
             * TIMA is incremented at the clock frequency specified by the TAC register ($FF07).
             * When the value overflows (exceeds $FF) it is reset to the value specified in TMA (FF06)
             * and an interrupt is requested.
             */
            unsigned clockFrequencyThreshold = -1;
            // The frequency is represented by the bit 0-1 of the TAC register
            switch (m_tac & 0x03)
            {
                case 0:
                    clockFrequencyThreshold = 1024;
                    break;
                case 1:
                    clockFrequencyThreshold = 16;
                    break;
                case 2:
                    clockFrequencyThreshold = 64;
                    break;
                case 3:
                    clockFrequencyThreshold = 256;
                    break;
            }

            while (m_timaCycles >= clockFrequencyThreshold)
            {
                m_timaCycles -= clockFrequencyThreshold;

                if (m_tima == 0xFF)
                {
                    m_tima = m_tma;

                    // Set the interrupt flag
                    uint8_t interruptFlag = m_memory->read(INTERRUPT_FLAG_ADDRESS);
                    interruptFlag |= 0x04;
                    m_memory->write(INTERRUPT_FLAG_ADDRESS, interruptFlag);
                }
                else
                    m_tima++;
            }
        }

        setRegisters();
    }
} // namespace gameboy
