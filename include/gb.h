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
        Cartridge *cartridge;
        CPU *cpu;
        Input *input;
        Memory *mmu;
        Platform *platform;
        PPU *ppu;
        Registers *registers;
        Timer *timer;

        constexpr static int FPS = 60;
        constexpr static int FRAMERATE = 1000 / FPS;
    };
} // namespace gameboy
