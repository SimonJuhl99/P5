import matplotlib as mpl
mpl.use("TkAgg")
import matplotlib.pyplot as plt
import numpy as np

# f_cwnd = open("two-routes-cwnd.data", "r")
# f_rtt = open("two-routes-rtt.data", "r")
# f_txtp = open("two-routes-tx-throughput.data", "r")
# f_rxtp = open("two-routes-rx-throughput.data", "r")
# f_cwnd = open("Vegas-cwnd.data", "r")
# f_rtt = open("Vegas-rtt.data", "r")
# f_txtp = open("Vegas-tx-throughput.data", "r")
# f_rxtp = open("Vegas-rx-throughput.data", "r")
f_cwnd = open("NewReno-cwnd.data", "r")
f_rtt = open("NewReno-rtt.data", "r")
f_txtp = open("NewReno-tx-throughput.data", "r")
f_rxtp = open("NewReno-rx-throughput.data", "r")
# f_cwnd = open("Bbr-cwnd.data", "r")
# f_rtt = open("Bbr-rtt.data", "r")
# f_txtp = open("Bbr-tx-throughput.data", "r")
# f_rxtp = open("Bbr-rx-throughput.data", "r")
# f_cwnd = open("Vegas-error-cwnd.data", "r")
# f_rtt = open("Vegas-error-rtt.data", "r")
# f_txtp = open("Vegas-error-tx-throughput.data", "r")
# f_rxtp = open("Vegas-error-rx-throughput.data", "r")
# f_cwnd = open("NewReno-error-cwnd.data", "r")
# f_rtt = open("NewReno-error-rtt.data", "r")
# f_txtp = open("NewReno-error-tx-throughput.data", "r")
# f_rxtp = open("NewReno-error-rx-throughput.data", "r")
# f_cwnd = open("Bbr-error-cwnd.data", "r")
# f_rtt = open("Bbr-error-rtt.data", "r")
# f_txtp = open("Bbr-error-tx-throughput.data", "r")
# f_rxtp = open("Bbr-error-rx-throughput.data", "r")
# print(f.read().splitlines())

cwnd_time = []
cwnd_values = []

rtt_time = []
rtt_values = []

txtp_time = []
txtp_values = []

rxtp_time = []
rxtp_values = []


#  -- Congestion Window Data  --
for i in f_cwnd.read().splitlines():
    cwnd_splitdata = i.split()
    cwnd_time.append(float(cwnd_splitdata[0]))
    cwnd_values.append(int(cwnd_splitdata[1]))

#  -- RTT Data  --
for i in f_rtt.read().splitlines():
    rtt_splitdata = i.split()
    rtt_time.append(float(rtt_splitdata[0]))
    rtt_values.append(float(rtt_splitdata[1]))


#  -- Transfered Data  --
for i in f_txtp.read().splitlines():
    tp_splitdata = i.split()
    print(tp_splitdata)
    try:
        data = i.split('e+', 1)
        mantisa = float(data[0])
        exp = float( str(data[1]).split('ns')[0])
        value = float(tp_splitdata[1])
    except:
        mantisa = int(i.split('n')[0])  
        exp = 0
        value = float(0)
    exp = exp - 9
    combined = float( (mantisa * (10**exp)))  
    print(f"Mantisa in Tx: {mantisa}")      
    print(f"Exp in Tx: {exp}")       
    print(f"Value in Tx: {value}")       
    print(f"Combined in Tx: {combined}\n")

    txtp_time.append(float(combined))        # New Split Thing
    txtp_values.append(value)

#  -- Received Data  --
for i in f_rxtp.read().splitlines():
    tp_splitdata = i.split()
    print(tp_splitdata)
    try:
        data = i.split('e+', 1)
        mantisa = float(data[0])
        exp = float( str(data[1]).split('ns')[0])
    except:
        mantisa = int(i.split('n')[0])  
        exp = 0
    exp = exp - 9
    combined = float( (mantisa * (10**exp)))  
    print(f"Mantisa in Rx: {mantisa}")      
    print(f"Exp in Rx: {exp}")       
    print(f"Value in Rx: {value}")       
    print(f"Combined in Rx: {combined}\n")

    rxtp_time.append(float(combined))
    rxtp_values.append(float(tp_splitdata[1]))

#fig, (ax0, ax1, ax2, ax3) = plt.subplots(nrows=2, ncols=2, sharex=False,
#                                    figsize=(12, 22))

fig, axs = plt.subplots(2, 2)

axs[0,0].set_title('Congestion window')
axs[0,0].plot(cwnd_time, cwnd_values)

axs[0,1].set_title('Round-trip time')
#axs[0,1].plot(rxtp_time, rxtp_values, color='red')
axs[0,1].plot(rtt_time, rtt_values, color='red')

twin_tp_plot = axs[1,0].twinx()
axs[1,0].set_title('Throughput')
comb_tp_p1 = axs[1,0].plot(txtp_time, txtp_values, color='darkgreen', label="txOffered")
comb_tp_p2 = twin_tp_plot.plot(rxtp_time, rxtp_values, color='lightgreen', label="TP")

axs[1,0].set_ylabel("tx")
twin_tp_plot.set_ylabel("rx")

tp_plt_ylim = axs[1,0].get_ylim()[1]
axs[1,0].set_ylim(0, tp_plt_ylim)
twin_tp_plot.set_ylim(0, tp_plt_ylim)

# Combined plot
twin1 = axs[1,1].twinx()
twin2 = axs[1,1].twinx()
twin3 = axs[1,1].twinx()

# offset right spine of twin2
twin2.spines.right.set_position(("axes", 1.1))
twin3.spines.right.set_position(("axes", 1.1))

axs[1,1].set_title('Combined')

# Create plots with values and define colors and labels
comb_p1, = axs[1,1].plot(cwnd_time, cwnd_values, "b-", label="CWND")
comb_p2, = twin1.plot(rtt_time, rtt_values, "r-", label="RTT")
comb_p3, = twin2.plot(txtp_time, txtp_values, color='darkgreen', label="txOffered")
comb_p4, = twin3.plot(rxtp_time, rxtp_values, color='lightgreen', label="TP")


cwnd_ylim = axs[0,0].get_ylim()[1]
rtt_ylim = axs[0,1].get_ylim()[1]
tp_ylim = axs[1,0].get_ylim()[1]

# Define limits of the different axes
#axs[1,1].set_xlim(1, 4.2)
axs[1,1].set_ylim(0, cwnd_ylim)
twin1.set_ylim(0, rtt_ylim*1.2)
twin2.set_ylim(0, tp_ylim*1.4)
twin3.set_ylim(0, tp_ylim*1.4)

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

axs[1,1].legend(handles=[comb_p1, comb_p2, comb_p3, comb_p4])
axs[1,1].grid(True)

plt.show()
