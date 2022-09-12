#include "../include/gb.h" // GB

int main(int argc, char *argv[])
{
    gameboy::GB gameboy(argv[1]);
    gameboy.run();
    return 0;
}
