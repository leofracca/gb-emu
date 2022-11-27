#include "gb.h" // GB

#include <SDL2/SDL_timer.h> // SDL_GetTicks64, SDL_Delay

namespace gameboy
{
    GB::GB(const std::string &filename, const int scale)
        : m_platform(scale),
          m_memory(filename)
    {}

    void GB::run()
    {
        CPU cpu(&m_memory);
        PPU ppu(&m_memory);
        Timer timer(&m_memory);
        Input input(&m_memory);

        auto lastCycleTime = SDL_GetTicks64();

        do
        {
            int cycles = cpu.cycle() * 4;
            timer.cycle(cycles);
            ppu.cycle(cycles);
        } while (!updatePlatform(lastCycleTime, ppu, input));

        saveRAMData();
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

            return Platform::processInput(input);
        }

        return false;
    }

    void GB::saveRAMData() const
    {
        m_memory.getCartridge().saveRAMData();
    }
} // namespace gameboy
