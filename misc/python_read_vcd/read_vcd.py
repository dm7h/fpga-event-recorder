### 
# Example parsing a .vcd file and performing some analysis  
#
import numpy as np
import pandas as pd
import matplotlib
import matplotlib.pyplot as plt
matplotlib.style.use('ggplot')

# import the vcd parser
try:
    import Verilog_VCD
except ImportError:
    raise ImportError("This script needs Verilog_VCD (you can use 'pip install Verliog_VCD' to install it)")

# parse example file
vcd = Verilog_VCD.parse_vcd('reaper_4.vcd')

# get the first time value
last = vcd["!"]["tv"][0][0]

# calculate time deltas
deltas = []
for item in vcd["!"]["tv"][1:]: # ignore first value
    deltas.append(item[0]-last)
    last = item[0]

# convert to numpy array 
# (only take every 2nd entry (the distances between clock pulses) and ignore the first two) 
print(deltas[:10])

# add high signals to low signals for complete period
# see: https://stackoverflow.com/a/9161588

if len(deltas) % 2: deltas.append(0)
deltas = [a+b for a,b in zip(deltas[::2], deltas[1::2])]
print(deltas[:10])

np_deltas = np.array(deltas[::2][2:])

# convert to nanoseconds
np_deltas_ns = np_deltas * 10

expected_delta = 31250000 # expected clock period in nanoseconds
np_deltas_ns -= expected_delta

# convert to pandas series
deltas_series = pd.Series(pd.to_timedelta(np_deltas_ns, unit='ns'))

print(deltas_series.dt.components)

# print some statistics
print(deltas_series.describe())

# plot a histogramm
(deltas_series/pd.Timedelta(milliseconds = 1)).hist(density=1, bins=100, color="skyblue")

# show mean and 0
mean = deltas_series.mean()/pd.Timedelta(milliseconds = 1)
plt.axvline(mean, color='k', linestyle='dashed', linewidth=1)
plt.axvline(0, color='red', linewidth=2)

# prettify
plt.xlabel('deviation (in ms)')
plt.ylabel('probability')

# show it
plt.show()


