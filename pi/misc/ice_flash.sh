#!/bin/bash

# GPIO for FPGA reset
GPIO=24

usage() { echo "Usage: $0 [-b /path/to/binary.bin]" 1>&2; }



while getopts :b: OPTION ; do
	case "$OPTION" in
	b) 	
		BIN_FILE=$OPTARG
		;;
	h | *) 
		usage; exit 0
		;;  
	esac
done

# check arguments
if [ -f "$BIN_FILE" ]; then
       echo "Binary file: $BIN_FILE"
else
	echo "File \"$BIN_FILE\" does not exist!"
	usage; exit 1
fi       

# creating 0-padded output binary
if [ ! -f padded_out.bin ]; then
	echo "Creating zero-padded output binary .."
	truncate -s 2M padded_out.bin
	dd if=$BIN_FILE conv=notrunc of=padded_out.bin
fi 

if [ -f padded_out.bin ]; then 
	# export gpio
	echo "$GPIO" > /sys/class/gpio/export 
	
	# pull fpga reset low
	echo "Put FPGA in reset mode .."
	echo out > /sys/class/gpio/gpio${GPIO}/direction
	
	# flash
	echo "Flashing the binary image via SPI .."
	sudo flashrom -p linux_spi:dev=/dev/spidev0.0,spispeed=20000 -w padded_out.bin

	# check
	if [ $? -eq 0 ]; then
		echo -e "\e[32mSuccessfully flashed binary image to FPGA ..\e[39m"
	else 
		echo -e "\e[31mFailed to flash binary image to FPGA!\3[39m"
	fi
	
	# releasing the reset
	echo in > /sys/class/gpio/gpio${GPIO}/direction
	echo "Cleaning up .. done!!" 
	rm padded_out.bin
fi

