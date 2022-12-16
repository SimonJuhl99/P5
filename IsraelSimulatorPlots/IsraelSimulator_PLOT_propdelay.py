import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np


f_same = open("parallel_dir(1).data", "r")
f_opposite = open("oppo_dir(1).data", "r")
#print(f.read().splitlines())

same_time = []
same_values = []
same_distance = []

opposite_time = []
opposite_values = []
opposite_distance = []


#txtp_time = []
#txtp_values = []

#rxtp_time = []
#rxtp_values = []


for i in f_same.read().splitlines():
    same_splitdata = i.split()
    same_time.append(float(same_splitdata[0]))
    same_values.append(float(same_splitdata[1]))
    same_distance.append(float(same_splitdata[2])/299792458)

for i in f_opposite.read().splitlines():
    opposite_splitdata = i.split()
    opposite_time.append(float(opposite_splitdata[0]))
    opposite_values.append(float(opposite_splitdata[1]))
    opposite_distance.append(float(opposite_splitdata[2])/299792458)
    




#fig, (ax0, ax1, ax2, ax3) = plt.subplots(nrows=2, ncols=2, sharex=False,
#                                    figsize=(12, 22))

fig, axs = plt.subplots(2,2)

axs[0,0].set_title('Parallel satellites')
axs[0,0].plot(same_time, same_distance)

axs[0,1].set_title('Opposite satellites')
#axs[0,1].plot(rxtp_time, rxtp_values, color='red')
axs[0,1].plot(opposite_time, opposite_distance, color='red')

#twin_tp_plot = axs[1,0].twinx()
#axs[1,0].set_title('Throughput')
#comb_tp_p1 = axs[1,0].plot(txtp_time, txtp_values, color='darkgreen', label="txOffered")
#comb_tp_p2 = twin_tp_plot.plot(rxtp_time, rxtp_values, color='lightgreen', label="TP")

#axs[1,0].set_ylabel("tx")
#twin_tp_plot.set_ylabel("rx")

#tp_plt_ylim = axs[1,0].get_ylim()[1]
#axs[1,0].set_ylim(0, tp_plt_ylim)
#twin_tp_plot.set_ylim(0, tp_plt_ylim)

# Combined plot
twin1 = axs[1,1].twinx()
#twin2 = axs[1,1].twinx()
#twin3 = axs[1,1].twinx()

# offset right spine of twin2
#twin2.spines.right.set_position(("axes", 1.1))
#twin3.spines.right.set_position(("axes", 1.1))

axs[1,1].set_title('BasicSimulator Propagation Delay for Satellites')

# Create plots with values and define colors and labels
comb_p1, = axs[1,1].plot(same_time, same_distance, "b-", label="Parallel sats")
comb_p2, = twin1.plot(opposite_time, opposite_distance, "r-", label="Opposite sats")
#comb_p3, = twin2.plot(txtp_time, txtp_values, color='darkgreen', label="txOffered")
#comb_p4, = twin3.plot(rxtp_time, rxtp_values, color='lightgreen', label="TP")


cwnd_ylim = axs[0,0].get_ylim()[1]
rtt_ylim = axs[0,1].get_ylim()[1]
#tp_ylim = axs[1,0].get_ylim()[1]

# Define limits of the different axes
#axs[1,1].set_xlim(1, 4.2)
axs[1,1].set_ylim(0, 0.06)
twin1.set_ylim(0, 0.06)
#twin2.set_ylim(0, tp_ylim*1.4)
#twin3.set_ylim(0, tp_ylim*1.4)

# Set labels of axes
axs[1,1].set_xlabel("Time (seconds)")
axs[1,1].set_ylabel("Propagation Delay (seconds)")
#twin1.set_ylabel("RTT")
#twin2.set_ylabel("TP")

# Set label colors
axs[1,1].yaxis.label.set_color(comb_p1.get_color())
twin1.yaxis.label.set_color(comb_p2.get_color())
#twin2.yaxis.label.set_color(comb_p3.get_color())

# Set color of ticks and axis values
tkw = dict(size=4, width=1.5)
axs[1,1].tick_params(axis='y', colors=comb_p1.get_color(), **tkw)
twin1.tick_params(axis='y', colors=comb_p2.get_color(), **tkw)
#twin2.tick_params(axis='y', colors=comb_p3.get_color(), **tkw)
axs[1,1].tick_params(axis='x', **tkw)

# Shrink current axis's height by 10% on the bottom
#box = axs.get_position()
#axs.set_position([box.x0, box.y0 + box.height * 0.1,
#                 box.width, box.height * 0.9])

# Put a legend below current axis
#axs[1,1].legend(loc='lower left', bbox_to_anchor=(0.2, -0.02),handles=[comb_p1,comb_p2]),


#axs[1,1].legend(loc='upper left',handles=[comb_p1, comb_p2])
axs[1,1].legend(bbox_to_anchor=(1.04, 1), loc="upper left",handles=[comb_p1,comb_p2])
axs[1,1].grid(True)

plt.savefig('propagation_delay_plot.pdf') 
plt.savefig('propagation_delay_hq.png', dpi=300)  
plt.show()
