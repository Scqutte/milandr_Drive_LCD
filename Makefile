.RECIPEPREFIX := >

TARGET = milandr_test
BUILD_DIR = build

MDR_DIR = Milandr_1986be93y_driver-main/MDR32F9

PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size

CPU = -mcpu=cortex-m3 -mthumb
OPT = -Og
DEBUG = 1

C_DEFS = \
-DUSE_MDR1986VE9x \
-D__STARTUP_CLEAR_BSS \
-D__START=main

C_SOURCES = \
main.c \
$(MDR_DIR)/Libraries/MDR32F9Qx_StdPeriph_Driver/src/MDR32F9Qx_port.c \
$(MDR_DIR)/Libraries/MDR32F9Qx_StdPeriph_Driver/src/MDR32F9Qx_rst_clk.c \
$(MDR_DIR)/Libraries/MDR32F9Qx_StdPeriph_Driver/src/MDR32F9Qx_power.c \
$(MDR_DIR)/Libraries/CMSIS/CM3/DeviceSupport/MDR32F9Qx/startup/gcc/system_MDR32F9Qx.c

ASM_SOURCES = \
$(MDR_DIR)/Libraries/CMSIS/CM3/DeviceSupport/MDR32F9Qx/startup/gcc/startup_MDR32F9Qx.s

INCLUDES = \
-I. \
-I$(MDR_DIR)/Config \
-I$(MDR_DIR)/Libraries/CMSIS/CM3/CoreSupport \
-I$(MDR_DIR)/Libraries/CMSIS/CM3/DeviceSupport/MDR32F9Qx/inc \
-I$(MDR_DIR)/Libraries/CMSIS/CM3/DeviceSupport/MDR32F9Qx/startup/gcc \
-I$(MDR_DIR)/Libraries/MDR32F9Qx_StdPeriph_Driver/inc

LDSCRIPT = $(MDR_DIR)/Libraries/CMSIS/CM3/DeviceSupport/MDR32F9Qx/startup/gcc/MDR32F9Qx.ld

LIBS = -lc -lm -lnosys

CFLAGS = $(CPU) $(OPT) $(C_DEFS) $(INCLUDES) \
-Wall -Wextra \
-ffunction-sections -fdata-sections \
-MMD -MP

ifeq ($(DEBUG),1)
CFLAGS += -g -gdwarf-2
endif

LDFLAGS = $(CPU) \
-specs=nano.specs \
-T$(LDSCRIPT) \
-Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref \
-Wl,--gc-sections

OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))

vpath %.c $(sort $(dir $(C_SOURCES)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) | $(BUILD_DIR)
>$(CC) $(OBJECTS) $(LDFLAGS) $(LIBS) -o $@
>$(SZ) $@

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
>$(CP) -O ihex $< $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
>$(CP) -O binary $< $@

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
>$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
>$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR):
>mkdir -p $(BUILD_DIR)

clean:
>rm -rf $(BUILD_DIR)

-include $(wildcard $(BUILD_DIR)/*.d)