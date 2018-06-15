#!/usr/bin/env python
import signal
import sys
import time
import rtmidi
from functools import partial

# MIDI CLOCK TEMPO (beats per minute)
BPM = 80

# define clock messages
clock_start = [0xFA]
clock_tick  = [0xF8]
clock_stop  = [0xFC]

def run():
    
    # midi out port
    midiout = 0

    # define SIGINT handler so we can gracefully quit 
    def sigint(midiout, signal, frame):
        print("stopping midi clock ..")
        if midiout:
            midiout.send_message(clock_stop)
            del midiout
        sys.exit(0)

    signal.signal(signal.SIGINT, partial(sigint, midiout))

    # get availbale midi out ports
    midiout = rtmidi.MidiOut()
    available_ports = midiout.get_ports()

    # find (first) teensy in it
    if available_ports:
        teensy_idx = [i for i,p in enumerate(available_ports) if 'Teensy' in p]

    # open the midi port
    if len(teensy_idx) > 0:
        midiout.open_port(teensy_idx[0])
    else:
        midiout.open_virtual_port("Virtual Midi Clock")

    # calculate speed
    # midi clock is 24 pulses per quarter note (beat)
    # clock freq => 60s / (BPM * 24 ppqn)
    clock_period = 60/(BPM*24)


    # and go ..
    print("starting midi clock with a clock period of " + str(clock_period * 1000) + " ms")
    midiout.send_message(clock_start)

    while (True):
        midiout.send_message(clock_tick)
        time.sleep(clock_period)

if __name__ == '__main__':
    run()

