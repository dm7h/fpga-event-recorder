

ifeq ($(shell bash -c 'type -p icozprog'),)
SSH_RASPI ?= ssh pi@raspi
else
SSH_RASPI ?= sh -c
endif

icozprog: icozprog.cc
	gcc -o icozprog -Wall -Os icozprog.cc -lwiringPi -lrt -lstdc++

example.blif: example.v
	yosys -p 'synth_ice40 -blif example.blif' example.v

example.asc: example.blif example.pcf
	arachne-pnr -d 8k -P tq144:4k -p example.pcf -o example.asc example.blif

example.bin: example.asc
	icetime -d hx8k -P tq144:4k -c 100 example.asc
	icepack example.asc example.bin

example_sram: example.bin
	$(SSH_RASPI) 'icozprog -p' < example.bin

example_flash: example.bin
	$(SSH_RASPI) 'icozprog -f' < example.bin
	$(SSH_RASPI) 'icozprog -b'

reset:
	$(SSH_RASPI) 'icozprog -f' < example.pcf
	$(SSH_RASPI) 'icozprog -b'

install: icozprog
	sudo install icozprog /usr/local/bin/
	sudo chmod u+s /usr/local/bin/icozprog

uninstall:
	sudo rm -f /usr/local/bin/icozprog

clean:
	rm -f icozprog example.blif example.asc example.bin

.PHONY: example_sram example_flash reset install clean

