/**
 * @file platform.h
 * @brief This file contains the declaration of the Platform class.
 *        It is responsible for creating the window, updating the screen and handling the inputs.
 */

#pragma once

#include "input.h" // Input

#include <SDL2/SDL.h> // SDL_Window, SDL_Renderer, SDL_Texture

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
         *
         * @param scale The scale of the window
         * @param maximize True if the window should be maximized, false otherwise
         */
        explicit Platform(int scale, bool maximize);

        /**
         * @brief Destroy the window
         * @details Call the SDL functions to destroy the window, renderer and texture
         */
        ~Platform();

        /// Platform cannot be copied
        Platform(const Platform &) = delete;

        /// Platform cannot be assigned
        Platform &operator=(const Platform &) = delete;

        /**
         * @brief Update the window with the new frame buffer
         *
         * @param buffer The new frame buffer
         */
        void update(const void *buffer);

        /**
         * @brief Get the input from the user
         *
         * @param input The input object
         * @return False if the user wants to quit, true otherwise
         */
        static bool processInput(Input &input);

    private:
        SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_Texture *texture;
    };
} // namespace gameboy
