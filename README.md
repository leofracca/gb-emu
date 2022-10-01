# gb-emu

An open source GameBoy emulator written in C++.

## Images

<p align='center'>
  <img src='https://github.com/leofracca/gb-emu/blob/main/images/poke_homescreen.png' width='30%'/>
  <img src='https://github.com/leofracca/gb-emu/blob/main/images/poke_battle.png' width='30%'/>
</p>

## Dependency

At the moment, the emulator has been tested only on Fedora. The required software can be installed with:

```shell
sudo dnf install make cmake g++ git SDL2 SDL2-devel
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

Then the executable will be created in the `build` folder.

## Playing

To run the emulator:

```shell
./GBEmu path/to/rom [scale]
```

where `scale` represents the scale of the window.

For example, supposing that there is a ROM called rom.gb inside the `build` folder, the command would be:

```shell
./GBEmu rom.gb
```

or

```shell
./GBEmu rom.gb 4
```

if you want a bigger window (in this case the window will be 4 times bigger than the normal).

## Testing

To run the tests:

```shell
make test
```

Some tests are handwritten, but there is also a test that opens a window that runs a test ROM.

Thanks to [Blargg's tests roms.](https://github.com/retrio/gb-test-roms) for the roms.

## Coverage

To generate the code coverage you need to pass the flag `-DCOVERAGE=ON` when building the project with cmake. Then the target `coverage` will be available.

The command

```shell
make coverage
```

will generate the coverage.html file with all the statistics about the coverage.

## Documentation

To create the documentation:

```shell
make doc
```

## TODO

- [ ] GameBoy Color support

- [ ] Audio

## References

- [Pan Docs](https://gbdev.io/pandocs/)

- [The Ultimate Game Boy Talk (33c3) - YouTube](https://www.youtube.com/watch?v=HyzD8pNlpwI)

- [GameBoy CPU opcodes](https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html)

- [codeslinger.co.uk](http://www.codeslinger.co.uk/pages/projects/gameboy.html)

Also, see the [docs](docs) folder.
