/**
 * @file gb.h
 * @brief This file contains the declaration of the GB class.
 *        It includes all the other classes and is responsbile for the emulator to run.
 */

#pragma once

#include "cartridge.h" // Cartridge
#include "cpu.h" // CPU
#include "input.h" // Input
#include "memory.h" // Memory
#include "platform.h" // Platform
#include "ppu.h" // PPU
#include "registers.h" // Registers
#include "timer.h" // Timer

namespace gameboy
{
    /**
     * @brief The GB class is responsible for the emulator to run.
     */
    class GB
    {
    public:
        /**
         * @brief Create and initialize the emulator
         *
         * @param rom The path to the rom file
         */
        GB(std::string rom);

        /**
         * @brief Run the emulator
         * @details Do the steps of the CPU, PPU, Timer and Input
         */
        void run();

    private:
        Cartridge *m_cartridge;
        CPU *m_cpu;
        Input *m_input;
        Memory *m_memory;
        Platform *m_platform;
        PPU *m_ppu;
        Registers *m_registers;
        Timer *m_timer;

        constexpr static int FPS = 60;
        constexpr static int FRAMERATE = 1000 / FPS;

        /**
         * @brief Update the screen
         * @details If the PPU is rendering, update the screen and return the current time
         *
         * @param lastCycleTime The last time the screen was updated
         * @return uint64_t The current time
         */
        uint64_t updateScreen(uint64_t lastCycleTime);
    };
} // namespace gameboy
