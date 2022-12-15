#include "gb.h" // GB

int main(int argc, char *argv[])
{
    // Parse the arguments
    std::string romFilename = argv[1];
    int scale = (argc == 3) ? std::stoi(argv[2]) : 1;

    gameboy::GB gameboy(romFilename, scale);
    if (gameboy.run() == 1)
        return 1;

    return 0;
}
