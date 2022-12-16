import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np


f_cwnd = open("congestion-cwnd.data", "r")
f_rtt = open("congestion-rtt.data", "r")
f_cwnd2 = open("congestion-cwnd2.data", "r")
f_rtt2 = open("congestion-rtt2.data", "r")
#f_txtp = open("two-routes-tx-throughput.data", "r")
#f_rxtp = open("two-routes-rx-throughput.data", "r")
#print(f.read().splitlines())

cwnd_time = []
cwnd_values = []

rtt_time = []
rtt_values = []


cwnd_time2 = []
cwnd_values2 = []

rtt_time2 = []
rtt_values2 = []


txtp_time = []
txtp_values = []

rxtp_time = []
rxtp_values = []


for i in f_cwnd.read().splitlines():
    cwnd_splitdata = i.split()
    cwnd_time.append(float(cwnd_splitdata[0]))
    cwnd_values.append(int(cwnd_splitdata[1]))

for i in f_rtt.read().splitlines():
    rtt_splitdata = i.split()
    rtt_time.append(float(rtt_splitdata[0]))
    rtt_values.append(float(rtt_splitdata[1]))


for i in f_cwnd2.read().splitlines():
    cwnd_splitdata = i.split()
    cwnd_time2.append(float(cwnd_splitdata[0]))
    cwnd_values2.append(int(cwnd_splitdata[1]))

for i in f_rtt2.read().splitlines():
    rtt_splitdata = i.split()
    rtt_time2.append(float(rtt_splitdata[0]))
    rtt_values2.append(float(rtt_splitdata[1]))





fig, axs = plt.subplots(2, 2)

axs[0,0].set_title('Congestion window')
axs[0,0].plot(cwnd_time, cwnd_values)

axs[0,1].set_title('Round-trip time')
#axs[0,1].plot(rxtp_time, rxtp_values, color='red')
axs[0,1].plot(rtt_time, rtt_values, color='red')

axs[1,0].set_title('Congestion window')
axs[1,0].plot(cwnd_time2, cwnd_values2)

axs[1,1].set_title('Round-trip time')
#axs[0,1].plot(rxtp_time, rxtp_values, color='red')
axs[1,1].plot(rtt_time2, rtt_values2, color='red')


plt.show()