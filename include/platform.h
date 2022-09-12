/**
 * @file platform.h
 * @brief This file contains the declaration of the Platform class.
 *        It is responsible for creating the window and updating the screen.
 */

#pragma once

#include "input.h" // Input

#include <SDL2/SDL.h>

namespace gameboy
{
    /**
     * @brief The Platform class is responsible for creating the window and updating the screen.
     */
    class Platform
    {
    public:
        /**
         * @brief Create and initialize the window
         */
        Platform();

        /**
         * @brief Update the window with the new frame buffer
         *
         * @param buffer The new frame buffer
         */
        void update(const void *buffer);

        /**
         * @brief Get the input from the user
         *
         * @return True if the user wants to quit, false otherwise
         */
        bool processInput(Input *input);

    private:
        SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_Texture *texture;
    };
} // namespace gameboy
