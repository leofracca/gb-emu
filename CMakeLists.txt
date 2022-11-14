cmake_minimum_required(VERSION 3.21)
project(GBEmu
    DESCRIPTION "Gameboy Emulator"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

#########
# Flags #
#########
# Sanitizers
option(USAN     OFF)
# Coverage
option(COVERAGE OFF)

set(COMPILER_FLAGS
    -O3
    -Wall -Wextra -Wpedantic -Werror
    $<$<BOOL:${USAN}>:-fsanitize=undefined -fno-omit-frame-pointer>
)

set(LINKER_FLAGS
    $<$<BOOL:${USAN}>:-fsanitize=undefined -fno-omit-frame-pointer>
    $<$<BOOL:${COVERAGE}>:--coverage>
)

################
# Source files #
################
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
list(REMOVE_ITEM SOURCES ${PROJECT_SOURCE_DIR}/src/main.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS include/*.h)
file(GLOB_RECURSE TESTS   CONFIGURE_DEPENDS tests/*.cpp)

#####################
# Create executable #
#####################
find_package(SDL2 REQUIRED)
add_library(GBEmu_lib)
add_executable(GBEmu
    ${PROJECT_SOURCE_DIR}/src/main.cpp
)

target_sources(GBEmu_lib
    PUBLIC  ${HEADERS}
    PRIVATE ${SOURCES}
)

target_include_directories(GBEmu_lib
    PUBLIC  ${PROJECT_SOURCE_DIR}/include
    PRIVATE ${PROJECT_SOURCE_DIR}/src
)
target_include_directories(GBEmu
    PUBLIC  ${PROJECT_SOURCE_DIR}/include
    PRIVATE ${PROJECT_SOURCE_DIR}/src
)

target_compile_options(GBEmu_lib
    PRIVATE ${COMPILER_FLAGS}
    PRIVATE $<$<BOOL:${COVERAGE}>:--coverage>
)
target_compile_options(GBEmu
    PRIVATE ${COMPILER_FLAGS}
    PRIVATE $<$<BOOL:${COVERAGE}>:--coverage>
)

target_link_options(GBEmu_lib
    PRIVATE ${LINKER_FLAGS}
)
target_link_options(GBEmu
    PRIVATE ${LINKER_FLAGS}
)

target_link_libraries(GBEmu_lib
    PRIVATE SDL2::SDL2
)
target_link_libraries(GBEmu
    PRIVATE GBEmu_lib
)

# Install
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
install(
    FILES ${CMAKE_BINARY_DIR}/GBEmu
    DESTINATION ${PROJECT_SOURCE_DIR}/bin
)

#################
# Code Coverage #
#################
# TODO: Check if there are cleaner ways to exclude tests and catch from coverage
if (COVERAGE)
    add_custom_target(coverage
        COMMAND gcovr --root ${PROJECT_SOURCE_DIR}
                      --exclude ${PROJECT_SOURCE_DIR}/tests
                      --exclude ${PROJECT_SOURCE_DIR}/third_party/catch2
                      --html
                      --html-details
                      --output coverage.html
        COMMENT "Generating code coverage"
    )
endif()

#########################
# Doxygen documentation #
#########################
find_package(Doxygen)
# Check if Doxygen is installed
if (DOXYGEN_FOUND)
    file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/doc)

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
    )
else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

#############
# Test code #
#############
add_executable(GBEmu_test)

target_sources(GBEmu_test
    PUBLIC ${HEADERS}
    PRIVATE ${TESTS}
    PRIVATE ${PROJECT_SOURCE_DIR}/third_party/catch2/catch.hpp
)
target_include_directories(GBEmu_test
    PUBLIC  ${PROJECT_SOURCE_DIR}/include
)
target_include_directories(GBEmu_test SYSTEM
    PUBLIC ${PROJECT_SOURCE_DIR}/third_party/catch2
)
target_link_libraries(GBEmu_test
    PUBLIC GBEmu_lib
)
target_compile_options(GBEmu_test
    PRIVATE ${COMPILER_FLAGS}
)
target_link_options(GBEmu_test
    PRIVATE ${LINKER_FLAGS}
)

add_custom_command(TARGET GBEmu_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/data/roms
        ${CMAKE_CURRENT_BINARY_DIR}/test_roms
)

# Install
install(
    FILES ${CMAKE_BINARY_DIR}/GBEmu_test
    DESTINATION ${PROJECT_SOURCE_DIR}/bin
)

#########
# Tests #
#########
enable_testing()

add_test(
    NAME BasicTests
    COMMAND ${CMAKE_BINARY_DIR}/GBEmu_test
)
add_test(
    NAME valgrindTests
    COMMAND valgrind
        --leak-check=full
        --show-leak-kinds=all
        --track-fds=yes
        --track-origins=yes
        --error-exitcode=1
        ${CMAKE_BINARY_DIR}/GBEmu_test
)
# Blargg tests
add_custom_target(blargg
    COMMAND ${CMAKE_BINARY_DIR}/GBEmu ${PROJECT_SOURCE_DIR}/data/roms/cpu_instrs.gb 2
    COMMENT "Running Blargg tests"
)
