# stm32f411-template

Template for STM32F411 development. Includes a basic `Makefile`, startup code, CMSIS headers, a linker script, register-level clock configuration, and a USB CDC (virtual COM port) device built on [TinyUSB](https://github.com/hathach/tinyusb), with no ST HAL/Cube stack anywhere. Builds and flashes with `arm-none-eabi-gcc` and OpenOCD.

### Clone

TinyUSB is a git submodule, so clone with:

```bash
git clone --recurse-submodules <repo-url>
```

or, after a plain clone:

```bash
git submodule update --init
```

### Build and Flash (Firmware)

```bash
cd app
make        # build ELF, HEX, BIN
make flash  # program via ST-Link
make clean  # remove build artifacts
```

The USB CDC demo needs the board's own USB port connected (separate from the ST-Link), and enumerates as `0483:5740`: a `/dev/ttyACM*` (or `COMx`) virtual COM port that echoes back whatever it receives.

### Template structure

```
stm32f411-template/
├── app/
│   ├── src/            # source files: startup assembly, USB descriptors, IRQ handlers, main
│   ├── include/        # project-specific headers, e.g. tusb_config.h
│   ├── Makefile        # build system
│   └── linker.ld       # linker script
└── common/
    ├── src/            # shared source files: oscillator config, newlib retargeting stubs
    ├── include/        # shared headers
    ├── cmsis/          # CMSIS headers
    └── tinyusb/        # TinyUSB (git submodule, pinned to a tag)
```

