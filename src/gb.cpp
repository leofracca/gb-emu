#include "../include/gb.h" // GB

#include <SDL2/SDL_timer.h> // SDL_GetTicks

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

        auto lastCycleTime = SDL_GetTicks64();

        while (!quit)
        {
            cycles = 0;

            cycles = cpu->tick();
            timer->addCycles(cycles);
            ppu->cycle(cycles);

            if (ppu->isRenderingEnabled())
            {
                if (SDL_GetTicks64() - lastCycleTime < FRAMERATE)
                    SDL_Delay(FRAMERATE - SDL_GetTicks64() + lastCycleTime);

                platform->update(ppu->getFrameBuffer());
                ppu->setRenderingEnabled(false);

                lastCycleTime = SDL_GetTicks64();
            }

            quit = platform->processInput(input, cycles);
        }
    }
} // namespace gameboy
