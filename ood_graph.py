import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np


f_cwnd = open("two-routes-cwnd.data", "r")
f_rtt = open("two-routes-rtt.data", "r")
f_tp = open("two-routes-throughput.data", "r")
#print(f.read().splitlines())

cwnd_time = []
cwnd_values = []

rtt_time = []
rtt_values = []

tp_time = []
tp_values = []

for i in f_cwnd.read().splitlines():
    cwnd_splitdata = i.split()
    cwnd_time.append(float(cwnd_splitdata[0]))
    cwnd_values.append(int(cwnd_splitdata[1]))

for i in f_rtt.read().splitlines():
    rtt_splitdata = i.split()
    rtt_time.append(float(rtt_splitdata[0]))
    rtt_values.append(float(rtt_splitdata[1]))

for i in f_tp.read().splitlines():
    tp_splitdata = i.split()
    tp_time.append(float(tp_splitdata[0].split('+')[1].split('e')[0]))
    tp_values.append(float(tp_splitdata[1]))


#fig, (ax0, ax1, ax2, ax3) = plt.subplots(nrows=2, ncols=2, sharex=False,
#                                    figsize=(12, 22))

fig, axs = plt.subplots(2, 2)

axs[0,0].set_title('Congestion window')
axs[0,0].plot(cwnd_time, cwnd_values)

axs[0,1].set_title('Round-trip time')
axs[0,1].plot(rtt_time, rtt_values, color='red')

axs[1,0].set_title('Throughput')
axs[1,0].plot(tp_time, tp_values, color='green')

# Combined plot
twin1 = axs[1,1].twinx()
twin2 = axs[1,1].twinx()

# offset right spine of twin2
twin2.spines.right.set_position(("axes", 1.1))

axs[1,1].set_title('Combined')

# Create plots with values and define colors and labels
comb_p1, = axs[1,1].plot(cwnd_time, cwnd_values, "b-", label="CWND")
comb_p2, = twin1.plot(rtt_time, rtt_values, "r-", label="RTT")
comb_p3, = twin2.plot(tp_time, tp_values, "g-", label="TP")

# Define limits of the different axes
axs[1,1].set_xlim(1, 4.2)
axs[1,1].set_ylim(0, 150000)
twin1.set_ylim(0, 0.5)
twin2.set_ylim(0, 32)

# Set labels of axes
axs[1,1].set_xlabel("Time")
axs[1,1].set_ylabel("CWND")
twin1.set_ylabel("RTT")
twin2.set_ylabel("TP")

# Set label colors
axs[1,1].yaxis.label.set_color(comb_p1.get_color())
twin1.yaxis.label.set_color(comb_p2.get_color())
twin2.yaxis.label.set_color(comb_p3.get_color())

# Set color of ticks and axis values
tkw = dict(size=4, width=1.5)
axs[1,1].tick_params(axis='y', colors=comb_p1.get_color(), **tkw)
twin1.tick_params(axis='y', colors=comb_p2.get_color(), **tkw)
twin2.tick_params(axis='y', colors=comb_p3.get_color(), **tkw)
axs[1,1].tick_params(axis='x', **tkw)

axs[1,1].legend(handles=[comb_p1, comb_p2, comb_p3])

plt.show()
