# gb-emu

An open source GameBoy emulator written in C++.

## Dependency

The emulator is currently tested only on Fedora. The required software can be installed with:

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

Then the executable will be created in the `/build` folder

## Playing

```shell
./GBEmu path/to/rom [scale]
```

where `scale` represents the scale of the window.

For example, let's suppose that there is a ROM called rom.gb inside the /build folder:

```shell
./GBEmu rom.gb
```

or

```shell
./GBEmu rom.gb 4
```

## Testing

```shell
make test
```

In addition, there is a rom that tests the cpu instructions inside [/tests/roms](tests/roms).
To test them run the following (supposing you are inside the build folder):

```shell
./GBEmu ../tests/roms/cpu_instrs.gb
```

## Documentation

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
