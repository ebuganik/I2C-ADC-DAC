#!/usr/bin/python3
import csv
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as plticker


osc=np.genfromtxt("waveform.csv", delimiter=",", names=["x","y"])

fig, a1 = plt.subplots()

plt.title("Waveform", fontsize=18)
a1.set_xlabel("time [ms]", fontsize=15)
a1.set_ylabel("voltage [V]", fontsize=15)
a1.plot(osc['x']*1000,osc['y'],'r')

a1.grid()
plt.show()


