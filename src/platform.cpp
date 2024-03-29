#include "platform.h" // Platform
#include "ppu.h" // SCREEN_WIDTH, SCREEN_HEIGHT
#include <iostream>

namespace gameboy
{
    Platform::Platform(const int scale, const bool maximize)
    {
        SDL_Init(SDL_INIT_VIDEO);

        window = SDL_CreateWindow("GBEmu",
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  screen_size::SCREEN_WIDTH * scale,
                                  screen_size::SCREEN_HEIGHT * scale,
                                  SDL_WINDOW_RESIZABLE);
        if (maximize)
            SDL_MaximizeWindow(window);

        renderer = SDL_CreateRenderer(window, -1, 0);
        SDL_RenderSetLogicalSize(renderer, screen_size::SCREEN_WIDTH, screen_size::SCREEN_HEIGHT);

        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, screen_size::SCREEN_WIDTH, screen_size::SCREEN_HEIGHT);
    }

    Platform::~Platform()
    {
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void Platform::update(const void *buffer)
    {
        SDL_RenderClear(renderer);
        SDL_UpdateTexture(texture, nullptr, buffer, screen_size::SCREEN_WIDTH * 4);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    bool Platform::processInput(Input &input)
    {
        SDL_Event event;
        SDL_PollEvent(&event);
        switch (event.type)
        {
            case SDL_KEYDOWN: // Key pressed
                switch (event.key.keysym.sym)
                {
                    case SDLK_a: input.setButton(JoypadButton::BUTTON_A, true); break;
                    case SDLK_s: input.setButton(JoypadButton::BUTTON_B, true); break;
                    case SDLK_RETURN: input.setButton(JoypadButton::BUTTON_SELECT, true); break;
                    case SDLK_SPACE: input.setButton(JoypadButton::BUTTON_START, true); break;
                    case SDLK_RIGHT: input.setButton(JoypadButton::DIRECTION_RIGHT, true); break;
                    case SDLK_LEFT: input.setButton(JoypadButton::DIRECTION_LEFT, true); break;
                    case SDLK_UP: input.setButton(JoypadButton::DIRECTION_UP, true); break;
                    case SDLK_DOWN: input.setButton(JoypadButton::DIRECTION_DOWN, true); break;
                }
                break;
            case SDL_KEYUP: // Key released
                switch (event.key.keysym.sym)
                {
                    case SDLK_a: input.setButton(JoypadButton::BUTTON_A, false); break;
                    case SDLK_s: input.setButton(JoypadButton::BUTTON_B, false); break;
                    case SDLK_RETURN: input.setButton(JoypadButton::BUTTON_SELECT, false); break;
                    case SDLK_SPACE: input.setButton(JoypadButton::BUTTON_START, false); break;
                    case SDLK_RIGHT: input.setButton(JoypadButton::DIRECTION_RIGHT, false); break;
                    case SDLK_LEFT: input.setButton(JoypadButton::DIRECTION_LEFT, false); break;
                    case SDLK_UP: input.setButton(JoypadButton::DIRECTION_UP, false); break;
                    case SDLK_DOWN: input.setButton(JoypadButton::DIRECTION_DOWN, false); break;
                }
                break;
            case SDL_QUIT:
                return false;
        }

        input.sendInterrupt();
        return true;
    }
} // namespace gameboy
