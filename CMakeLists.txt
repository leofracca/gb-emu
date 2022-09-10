cmake_minimum_required(VERSION 3.21)
project(GBEmu)

set(CMAKE_CXX_STANDARD 17)

add_executable(GBEmu src/main.cpp
        src/registers.cpp include/registers.h
        src/mbc.cpp include/mbc.h
        src/cartridge.cpp include/cartridge.h
        src/memory.cpp include/memory.h
        src/timer.cpp include/timer.h
        src/input.cpp include/input.h
        src/cpu.cpp include/cpu.h
        src/ppu.cpp include/ppu.h)
