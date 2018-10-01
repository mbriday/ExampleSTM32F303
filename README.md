# Basic baremetal examples on STM32F303KT8

This project aims to give some basic examples for the ST Nucleo32 board STM32F303K8, using internal peripherals. 

## required tools

Examples do not use any IDE, it is just based on Makefile, with arm-none-eabi-gcc. 
The debugging is done using directly arm-none-eabi-gdb (or ddd for a GUI).

On Linux (Debian/Ubuntu flavor), you should install:
 * `build-essential`
 * `gcc-arm-none-eabi`
 * `gdb-arm-none-eabi` (to debug)
 * [st-link](https://github.com/texane/stlink)

For the first 3 tools:
```sh
sudo apt-get install build-essential gcc-arm-none-eabi gdb-arm-none-eabi
```

## Compile/Run

To compile, just use the makefile:

```sh
make
```

To flash the application (st-link should be installed first):

```sh
make flash
```

to debug the application (using gdb), first run `st-util` in another terminal, and run:

```sh
arm-none-eabi-gdb -tui test.elf
```

a gdb script is provided in examples to initiate communication with target, load the application and run until main, just use the gdb init script inside gdb: 

```gdb
source init.gdb
```


