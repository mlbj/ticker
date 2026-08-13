PROJECT  = stm32f411-template

# Directories
SRCDIR   = src
BINDIR   = bin
OBJDIR   = obj
INCDIR   = include
COMDIR   = ../common
TUSBDIR  = $(COMDIR)/tinyusb/src

# CPU
CPU      = cortex-m4

# Sources
SRC      = $(wildcard $(SRCDIR)/*.c) $(wildcard $(COMDIR)/src/*.c)
SRC     += $(wildcard $(TUSBDIR)/*.c) $(wildcard $(TUSBDIR)/common/*.c)
SRC     += $(wildcard $(TUSBDIR)/device/*.c) $(TUSBDIR)/class/cdc/cdc_device.c
SRC     += $(wildcard $(TUSBDIR)/portable/synopsys/dwc2/*.c)
ASM      = $(wildcard $(SRCDIR)/*.s) $(wildcard $(COMDIR)/src/*.s)

# Objects
OBJ      = $(addprefix $(OBJDIR)/,$(notdir $(SRC:.c=.o)))
OBJ     += $(addprefix $(OBJDIR)/,$(notdir $(ASM:.s=.o)))

# Includes
INCLUDE  = -I$(INCDIR) -I$(COMDIR)/include -I$(COMDIR)/cmsis -I$(TUSBDIR)

# Linker script
LSCRIPT  = linker.ld

# Flags
CFLAGS   = -mcpu=$(CPU) -mthumb -std=c99 -Wall -O0 -g3 -DSTM32F411xE $(INCLUDE) \
           -ffunction-sections -fdata-sections -MMD -MP \
           -mfpu=fpv4-sp-d16 -mfloat-abi=hard
ASFLAGS  = -mcpu=$(CPU)
LDFLAGS  = -T$(LSCRIPT) -Wl,--gc-sections -lc --specs=nosys.specs --specs=nano.specs

# Tools
CC       = arm-none-eabi-gcc
AS       = arm-none-eabi-as
OBJCOPY  = arm-none-eabi-objcopy
SIZE     = arm-none-eabi-size --format=SysV
OBJDUMP  = arm-none-eabi-objdump
GDB      = arm-none-eabi-gdb
OCD      = openocd

# Final targets
ELF      = $(BINDIR)/$(PROJECT).elf
BIN      = $(BINDIR)/$(PROJECT).bin
HEX      = $(BINDIR)/$(PROJECT).hex

# Default target
all: $(ELF) $(BIN) $(HEX)

# Rules
$(ELF): $(OBJ) $(LSCRIPT)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $@
	$(OBJDUMP) -D $@ > $(BINDIR)/$(PROJECT).lst
	$(SIZE) $@

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

vpath %.c $(SRCDIR) $(COMDIR)/src $(TUSBDIR) $(TUSBDIR)/common $(TUSBDIR)/device \
          $(TUSBDIR)/class/cdc $(TUSBDIR)/portable/synopsys/dwc2
vpath %.s $(SRCDIR) $(COMDIR)/src

$(OBJDIR)/%.o: %.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.s
	@mkdir -p $(OBJDIR)
	$(AS) $(ASFLAGS) -o $@ $<

# Flash
flash: $(ELF)
	$(OCD) -f interface/stlink.cfg -f target/stm32f4x.cfg \
		-c "program $< verify reset exit"

# Clean
clean:
	$(RM) -r $(OBJDIR) $(BINDIR)
