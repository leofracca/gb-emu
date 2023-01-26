# gb-emu

An open source GameBoy emulator written in C++.

## Dependency

- [git](https://git-scm.com/)

- [CMake](https://cmake.org/)

- [g++](https://gcc.gnu.org/)

- [SDL2](https://www.libsdl.org/)

- [Boost](https://www.boost.org/)

At the moment, the emulator has been tested on Ubuntu and Fedora. On Ubuntu the required software can be installed with:

```shell
sudo apt install git cmake g++ libsdl2-dev libboost-dev
```

On Fedora the required software can be installed with:

```shell
sudo dnf install git make cmake g++ SDL2 SDL2-devel boost-devel
```

## Building

```shell
git clone https://github.com/leofracca/gb-emu
cd gb-emu
mkdir build
cd build
cmake ..
make
```

Then the executable `gbemu` will be created inside the `build` folder.

## Playing

To run the emulator:

```shell
./gbemu path/to/rom [scale]
```

where `scale` represents the scale of the window.

For example, supposing that there is a ROM called rom.gb inside the `build` folder, the command would be:

```shell
./gbemu rom.gb
```

or

```shell
./gbemu rom.gb 4
```

if you want a bigger window (in this case the window will be 4 times bigger than the normal).

## Buttons

| Game Boy | Keyboard |
|:--------:|:--------:|
| A        | A        |
| B        | S        |
| Arrows   | Arrows   |
| Start    | Space    |
| Select   | Enter    |

## Testing

To run the tests:

```shell
make test
```

In addition to check the correctness of the code, the tests also check if there are memory leaks (using `valgrind`).

Some tests are handwritten, but there is also a test that opens a window that runs a test ROM. Use

```shell
make blarrg
```

to run it.

Thanks to [Blargg's tests roms](https://github.com/retrio/gb-test-roms).

## Coverage

To generate the code coverage you need to pass the flag `-DCOVERAGE=ON` when building the project with CMake. Then the target `coverage` will be available. [gcovr](https://gcovr.com/en/stable/) is required.

The command

```shell
make coverage
```

will generate the `coverage/coverage.html` file with all the statistics about the coverage (remember to run `make test` and `make blargg` first).

## Undefined behaviours

To check undefined behaviours you need to pass the `-DUSAN=ON` when building the project with CMake.

## Documentation

To create the documentation:

```shell
make doc
```

The `doc` folder will be created inside the root folder of the project.

To use this command `Doxygen` is required. OPTIONAL: you will also need the `texlive-font-utils` package to have a LaTeX documentation.

## TODO

- [ ] GameBoy Color support

- [ ] Audio

## References

- Pan Docs ([web version](https://gbdev.io/pandocs/) and [pdf](PanDocs/GB.pdf))

- [The Ultimate Game Boy Talk (33c3) - YouTube](https://www.youtube.com/watch?v=HyzD8pNlpwI)

- [GameBoy CPU opcodes](https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html)

- [codeslinger.co.uk](http://www.codeslinger.co.uk/pages/projects/gameboy.html)

## Images

| Homescreen                                                                                          | Gameplay                                                                                      |
|:---------------------------------------------------------------------------------------------------:|:---------------------------------------------------------------------------------------------:|
| <img src='https://github.com/leofracca/gb-emu/blob/main/images/poke_homescreen.png' width='70%'/>   | <img src='https://github.com/leofracca/gb-emu/blob/main/images/poke_play.png' width='70%'/>   |
| <img src='https://github.com/leofracca/gb-emu/blob/main/images/zelda_homescreen.png' width='70%'/>  | <img src='https://github.com/leofracca/gb-emu/blob/main/images/zelda_play.png' width='70%'/>  |
| <img src='https://github.com/leofracca/gb-emu/blob/main/images/tetris_homescreen.png' width='70%'/> | <img src='https://github.com/leofracca/gb-emu/blob/main/images/tetris_play.png' width='70%'/> |
| <img src='https://github.com/leofracca/gb-emu/blob/main/images/pacman_homescreen.png' width='70%'/> | <img src='https://github.com/leofracca/gb-emu/blob/main/images/pacman_play.png' width='70%'/> |
