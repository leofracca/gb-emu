cmake_minimum_required(VERSION 3.21)
project(GBEmu)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
add_executable(GBEmu src/main.cpp
        src/registers.cpp include/registers.h
        src/mbc.cpp include/mbc.h
        src/cartridge.cpp include/cartridge.h
        src/memory.cpp include/memory.h
        src/timer.cpp include/timer.h
        src/input.cpp include/input.h
        src/cpu.cpp include/cpu.h
        src/ppu.cpp include/ppu.h
        src/gb.cpp include/gb.h
        src/platform.cpp include/platform.h
        )
target_compile_options(GBEmu PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(GBEmu PRIVATE SDL2::SDL2)

# For testing purposes
enable_testing()

# Catch
find_package(Catch2 3 REQUIRED)
add_executable(GBEmu_test
        tests/testRegisters.cpp
        third_party/catch.hpp)
target_link_libraries(GBEmu_test PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)

# Tests to execute
add_test(Tests GBEmu_test)

# Doxygen
find_package(Doxygen)

# Check if Doxygen is installed
if (DOXYGEN_FOUND)
    # Set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # Configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    add_custom_target(doc ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)
