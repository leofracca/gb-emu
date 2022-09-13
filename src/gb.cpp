#include "../include/gb.h" // GB

#include <iostream>

namespace gameboy
{
    GB::GB(std::string rom)
    {
        cartridge = new Cartridge(rom);
        registers = new Registers();
        mmu = new Memory(cartridge);
        cpu = new CPU(mmu, registers);
        ppu = new PPU(mmu);
        timer = new Timer(mmu);
        input = new Input(mmu);
        platform = new Platform();
    }

    void GB::run()
    {
        int cycles = 0;

        bool quit = false;

        while (!quit)
        {
            cycles = 0;

            cycles = cpu->tick();
            timer->addCycles(4 * cycles);
            ppu->cycle(cycles);

            if (ppu->isRenderingEnabled())
            {
                // std::cout << "rendering enabled\n";
                platform->update(ppu->getFrameBuffer());
                ppu->setRenderingEnabled(false);
            }

            quit = platform->processInput(input, cycles);
            // std::cout << cycles << std::endl;
        }
    }
} // namespace gameboy
