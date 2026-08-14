# ticker-fw

### Build and Flash (Firmware)

```bash
cd app
make        # build ELF, HEX, BIN
make flash  # program via ST-Link
make clean  # remove build artifacts
```

### Firmware structure

```
ticker-fw
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

