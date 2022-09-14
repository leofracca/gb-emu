#include "../include/gb.h" // GB

#include <SDL2/SDL_timer.h> // SDL_GetTicks

namespace gameboy
{
    GB::GB(std::string rom)
    {
        m_cartridge = new Cartridge(rom);
        m_registers = new Registers();
        m_memory = new Memory(m_cartridge);
        m_cpu = new CPU(m_memory, m_registers);
        m_ppu = new PPU(m_memory);
        m_timer = new Timer(m_memory);
        m_input = new Input(m_memory);
        m_platform = new Platform();
    }

    void GB::run()
    {
        int cycles = 0;
        bool quit = false;

        auto lastCycleTime = SDL_GetTicks64();

        while (!quit)
        {
            cycles = 0;

            cycles = m_cpu->cycle();
            m_timer->cycle(cycles);
            m_ppu->cycle(cycles);

            lastCycleTime = updateScreen(lastCycleTime);

            quit = m_platform->processInput(m_input, cycles);
        }
    }

    uint64_t GB::updateScreen(uint64_t lastCycleTime)
    {
        if (m_ppu->isRenderingEnabled())
        {
            if (SDL_GetTicks64() - lastCycleTime < FRAMERATE)
                SDL_Delay(FRAMERATE - SDL_GetTicks64() + lastCycleTime);

            m_platform->update(m_ppu->getFrameBuffer());
            m_ppu->setRenderingEnabled(false);

            return SDL_GetTicks64();
        }

        return lastCycleTime;
    }
} // namespace gameboy
