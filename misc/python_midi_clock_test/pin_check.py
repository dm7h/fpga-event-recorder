#!/usr/bin/env python
import time
import rtmidi

def run():
    
    # get availbale midi out ports
    midiout = rtmidi.MidiOut()
    available_ports = midiout.get_ports()

    # find teensy in it
    if available_ports:
        teensy_idx = [i for i,p in enumerate(available_ports) if 'Teensy' in p]

    # open the midi port
    if len(teensy_idx) > 0:
        midiout.open_port(teensy_idx[0])
    else:
        midiout.open_virtual_port("Virtual Midi Clock")

    # and go ..
    print("running pin test ..")
    midiout.send_message([0x90, 12, 120])
    midiout.send_message([0x90, 1, 120])
    midiout.send_message([0x90, 3, 120])
    midiout.send_message([0x90, 5, 120])
    midiout.send_message([0x90, 7, 120])
    midiout.send_message([0x90, 8, 120])
    midiout.send_message([0x90, 6, 120])
    midiout.send_message([0x90, 4, 120])
    midiout.send_message([0x90, 2, 120])




if __name__ == '__main__':
    run()

