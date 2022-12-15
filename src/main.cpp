#include "gb.h" // GB

int main(int argc, char *argv[])
{
    // Parse the arguments
    std::string romFilename = argv[1];
    int scale = (argc == 3) ? std::stoi(argv[2]) : 1;

    // Run the emulator
    gameboy::GB gameboy(scale);
    if (gameboy.run(romFilename) == 1)
        return 1; // An error occurred
    return 0;
}
