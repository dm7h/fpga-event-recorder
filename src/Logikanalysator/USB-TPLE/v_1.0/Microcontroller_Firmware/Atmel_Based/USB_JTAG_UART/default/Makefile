###############################################################################
# Makefile for the project usb_uart
###############################################################################

## General Flags
PROJECT = usb_uart
MCU = atmega32u4
TARGET = usb_uart.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=8000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=usb_uart.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = main.o uart_usb_lib.o usb_descriptors.o usb_specific_request.o usb_standard_request.o cdc_task.o usb_device_task.o usb_task.o scheduler.o start_boot.o usb_drv.o uart_lib.o power_drv.o flash_drv.o jtag_task.o jtag_usb_lib.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) usb_uart.hex usb_uart.eep usb_uart.lss size

## Compile
main.o: ../src/main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uart_usb_lib.o: ../src/usb/uart_usb_lib.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usb_descriptors.o: ../src/usb/usb_descriptors.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usb_specific_request.o: ../src/usb/usb_specific_request.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usb_standard_request.o: ../src/usb/usb_standard_request.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

cdc_task.o: ../src/tasks/cdc_task.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usb_device_task.o: ../src/tasks/usb_device_task.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usb_task.o: ../src/modules/usb/usb_task.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

scheduler.o: ../src/modules/scheduler/scheduler.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

start_boot.o: ../src/lib_mcu/util/start_boot.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usb_drv.o: ../src/lib_mcu/usb/usb_drv.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uart_lib.o: ../src/lib_mcu/uart/uart_lib.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

power_drv.o: ../src/lib_mcu/power/power_drv.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

flash_drv.o: ../src/lib_mcu/flash/flash_drv.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

jtag_task.o: ../src/tasks/jtag_task.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

jtag_usb_lib.o: ../src/usb/jtag_usb_lib.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) usb_uart.elf dep/* usb_uart.hex usb_uart.eep usb_uart.lss usb_uart.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

