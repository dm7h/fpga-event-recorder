FPGA-Event-Recorder
-------------------

This project implements a FPGA based event recorder using the open source toolchain IceStorm. 
Similar to a logic analyzer it records logic level input signals, but in contrast to a logic analyzer it does not provide a continuous stream of the signal state. Instead the signals are filtered at capture time to match only relevant input combinations and signal changes. Using predefined events to filter the input stream is especially suitable for the analysis of known signals that are to be captured and examined at high temporal resolution.
The implementation of the event recorder is done on a Raspberry Pi Zero with a Lattice iCE40-based FPGA shield and is realized using the IcoSoc project with the IceStorm toolchain.   

### Getting started:

* install the IceStorm toolchain
see http://www.clifford.at/icestorm/#install

* install the RISC-V toolchain
```bash
git clone git@github.com:cliffordwolf/picorv32.git
cd picorv32
make download-tools
make -j$(nproc) build-tools                                                               
```
(can take a while)

* download the IcoSoc port for IceZero (on your local machine)
```bash
cd ..
git clone https://github.com/dm7h/icozsoc.git
```
* configure SSH on the raspi
```bash
# see: https://www.raspberrypi.org/documentation/remote-access/ssh/passwordless.md
# export ssh host, eg. pi@zero oder pi@192.168.1.43:
export SSH_RASPI=pi@zero
# check password-less ssh connection
ssh $SSH_RASPI                                                                               $
```
* install `icozctl` on the raspi 
```bash
git clone https://github.com/dm7h/icozctl
cd icozctl
sudo make install
```
* compile and flash the bitstream (local machine)
```bash
cd ../icosoc/examples/event-recorder/
make prog_flash
```
* on the raspi, edit `ìcozctl/config.yml` to your liking
```yaml
# event configuration
events: 
  - start:
          trigger: u
          funcion: start

  - stop:
          trigger: d
          function: stop

  - clk_up:
          trigger: 1u

  - clk_down:
          trigger: 1d
```
* start recording events `
```bash
icozctl -s -c config.yml -o midi_clock.vcd
```
see `icozctl -h` for options
