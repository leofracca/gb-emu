#include "../include/gb.h" // GB

#include <SDL2/SDL_timer.h> // SDL_GetTicks64, SDL_Delay

namespace gameboy
{
    GB::GB(const std::string &rom, const int scale)
        : m_platform(scale),
          m_memory(rom)
    {}

    void GB::run()
    {
        CPU cpu(&m_memory);
        PPU ppu(&m_memory);
        Timer timer(&m_memory);
        Input input(&m_memory);

        int cycles = 0;
        auto lastCycleTime = SDL_GetTicks64();

        do
        {
            cycles = 0;

            cycles = cpu.cycle();
            timer.cycle(cycles);
            ppu.cycle(cycles);
        }
        while (!updatePlatform(lastCycleTime, ppu, input));
    }

    bool GB::updatePlatform(uint64_t &lastCycleTime, PPU &ppu, Input &input)
    {
        if (ppu.isRenderingEnabled())
        {
            if (SDL_GetTicks64() - lastCycleTime < FRAMERATE)
                SDL_Delay(FRAMERATE - SDL_GetTicks64() + lastCycleTime);

            m_platform.update(ppu.getFrameBuffer());
            ppu.setRenderingEnabled(false);

            lastCycleTime = SDL_GetTicks64();

            return m_platform.processInput(input);
        }

        return false;
    }
} // namespace gameboy
