#include "gb.h" // GB

#include <SDL2/SDL_timer.h> // SDL_GetTicks64, SDL_Delay

namespace gameboy
{
    GB::GB(const int scale)
        : m_platform(scale)
    {}

    int GB::run(const std::string &filename)
    {
        Cartridge cartridge;
        bool cartridgeLoaded = cartridge.loadROM(filename);
        if (!cartridgeLoaded)
            return 1;

        Memory m_memory(cartridge);
        CPU cpu(m_memory);
        PPU ppu(m_memory);
        Timer timer(m_memory);
        Input input(m_memory);

        auto lastCycleTime = SDL_GetTicks();

        do
        {
            uint8_t cycles = cpu.cycle() * 4;
            if (cycles == 0) // An unexpected opcode was encountered
                return 1;
            timer.cycle(cycles);
            ppu.cycle(cycles);
        } while (updatePlatform(lastCycleTime, ppu, input));

        saveRAMData(cartridge);
        return 0;
    }

    bool GB::updatePlatform(uint32_t &lastCycleTime, PPU &ppu, Input &input)
    {
        if (!ppu.isRenderingEnabled())
            return true;

        if (SDL_GetTicks() - lastCycleTime < FRAMERATE)
            SDL_Delay(FRAMERATE - SDL_GetTicks() + lastCycleTime);

        m_platform.update(ppu.getFrameBuffer());
        ppu.setRenderingEnabled(false);

        lastCycleTime = SDL_GetTicks();

        return Platform::processInput(input);
    }

    void GB::saveRAMData(const Cartridge &cartridge)
    {
        cartridge.saveRAMData();
    }
} // namespace gameboy
