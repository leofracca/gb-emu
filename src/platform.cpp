#include "../include/platform.h" // Platform

#include "../include/ppu.h" // SCREEN_WIDTH, SCREEN_HEIGHT

namespace gameboy
{
    Platform::Platform()
    {
        SDL_Init(SDL_INIT_VIDEO);
        window = SDL_CreateWindow("GBEmu", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, 0);
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    void Platform::update(const void *buffer)
    {
        SDL_RenderClear(renderer);
        SDL_UpdateTexture(texture, nullptr, buffer, SCREEN_WIDTH * 4);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    bool Platform::processInput(Input *input, int cycles)
    {
        input->joypadCycles += cycles;
        if(input->joypadCycles < 65536)
            return false;
        input->joypadCycles -= 65536;

        SDL_Event event;
        SDL_PollEvent(&event);
        switch (event.type) {
            case SDL_KEYUP:
                switch(event.key.keysym.sym){
                    case SDLK_RIGHT: 	input->setButton(JoypadButton::DIRECTION_RIGHT,false); break;
                    case SDLK_LEFT:  	input->setButton(JoypadButton::DIRECTION_LEFT,false); break;
                    case SDLK_UP:    	input->setButton(JoypadButton::DIRECTION_UP,false); break;
                    case SDLK_DOWN:  	input->setButton(JoypadButton::DIRECTION_DOWN,false); break;
                    case SDLK_z:     	input->setButton(JoypadButton::BUTTON_A,false); break;
                    case SDLK_x:     	input->setButton(JoypadButton::BUTTON_B,false); break;
                    case SDLK_SPACE: 	input->setButton(JoypadButton::BUTTON_START,false); break;
                    case SDLK_RETURN: 	input->setButton(JoypadButton::BUTTON_SELECT,false); break;
                }
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                    case SDLK_RIGHT: 	input->setButton(JoypadButton::DIRECTION_RIGHT,true); break;
                    case SDLK_LEFT:  	input->setButton(JoypadButton::DIRECTION_LEFT,true); break;
                    case SDLK_UP:    	input->setButton(JoypadButton::DIRECTION_UP,true); break;
                    case SDLK_DOWN:  	input->setButton(JoypadButton::DIRECTION_DOWN,true); break;
                    case SDLK_z:     	input->setButton(JoypadButton::BUTTON_A,true); break;
                    case SDLK_x:     	input->setButton(JoypadButton::BUTTON_B,true); break;
                    case SDLK_SPACE: 	input->setButton(JoypadButton::BUTTON_START,true); break;
                    case SDLK_RETURN: 	input->setButton(JoypadButton::BUTTON_SELECT,true); break;
                }
                break;
            case SDL_QUIT:
                return true;
        }

        input->sendInterrupt();
        return false;
    }
} // namespace gameboy
