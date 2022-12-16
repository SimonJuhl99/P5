import argparse
import os
import matplotlib as mpl
mpl.use("pgf")
mpl.rcParams.update({
    "pgf.texsystem": "pdflatex",
    'font.family': 'serif',
    'text.usetex': True,
    'pgf.rcfonts': False,
})
import matplotlib.pyplot as plt
import numpy as np

org_path = os.getcwd()
data_path = "../output_files/"

R_cwnd_time = []
R_cwnd_values = []
R_rtt_time = []
R_rtt_values = []
R_txtp_time = []
R_txtp_values = []
R_rxtp_time = []
R_rxtp_values = []

B_cwnd_time = []
B_cwnd_values = []
B_rtt_time = []
B_rtt_values = []
B_txtp_time = []
B_txtp_values = []
B_rxtp_time = []
B_rxtp_values = []

V_cwnd_time = []
V_cwnd_values = []
V_rtt_time = []
V_rtt_values = []
V_txtp_time = []
V_txtp_values = []
V_rxtp_time = []
V_rxtp_values = []

def gatherData(scenario: str):
    file_path = data_path + scenario
    os.chdir(file_path)

# -------------- TCP NewReno --------------
    
    # --- NewReno CWND ---
    with open('NewReno-cwnd.data', 'r') as f:
        for i in f.read().splitlines():
            data_splitdata = i.split()
            R_cwnd_time.append(float(data_splitdata[0]))
            R_cwnd_values.append(int(data_splitdata[1]))

    # --- NewReno RTT ---
    with open('NewReno-rtt.data', 'r') as f:
        for i in f.read().splitlines():
            data_splitdata = i.split()
            R_rtt_time.append(float(data_splitdata[0]))
            R_rtt_values.append(float(data_splitdata[1]))

    # --- NewReno TX ---
    with open('NewReno-tx-throughput.data', 'r') as f:
        for i in f.read().splitlines():
            data_splitdata = i.split()
            try:
                data = i.split('e+', 1)
                mantisa = float(data[0])
                exp = float(str(data[1]).split('ns')[0])
                value = float(data_splitdata[1])
            except:
                mantisa = int(i.split('n')[0])
                exp = 0
                value = float(0)
            exp = exp - 9
            combined = float( (mantisa * (10**exp)) )
            
            R_txtp_time.append(float(combined))
            R_txtp_values.append(value)

    # --- NewReno RX ---
    with open('NewReno-rx-throughput.data', 'r') as f:
        for i in f.read().splitlines():
            data_splitdata = i.split()
            try:
                data = i.split('e+', 1)
                mantisa = float(data[0])
                exp = float(str(data[1]).split('ns')[0])
            except:
                mantisa = int(i.split('n')[0])
                exp = 0
            exp = exp - 9
            combined = float( (mantisa * (10**exp)) )
            
            R_rxtp_time.append(float(combined))
            R_rxtp_values.append(float(data_splitdata[1]))



# -------------- TCP BBR --------------

    # --- BBR CWND ---
    with open('Bbr-cwnd.data', 'r') as f:
        for i in f.read().splitlines():
            data_splitdata = i.split()
            B_cwnd_time.append(float(data_splitdata[0]))
            B_cwnd_values.append(int(data_splitdata[1]))

    # --- BBR RTT ---
    with open('Bbr-rtt.data', 'r') as f:
        for i in f.read().splitlines():
            data_splitdata = i.split()
            B_rtt_time.append(float(data_splitdata[0]))
            B_rtt_values.append(float(data_splitdata[1]))

    # --- BBR TX ---
    with open('Bbr-tx-throughput.data', 'r') as f:
        for i in f.read().splitlines():
            data_splitdata = i.split()
            try:
                data = i.split('e+', 1)
                mantisa = float(data[0])
                exp = float(str(data[1]).split('ns')[0])
                value = float(data_splitdata[1])
            except:
                mantisa = int(i.split('n')[0])
                exp = 0
                value = float(0)
            exp = exp - 9
            combined = float( (mantisa * (10**exp)) )
            
            B_txtp_time.append(float(combined))
            B_txtp_values.append(value)
           
    # --- BBR RX ---
    with open('Bbr-rx-throughput.data', 'r') as f:
        for i in f.read().splitlines():
            data_splitdata = i.split()
            try:
                data = i.split('e+', 1)
                mantisa = float(data[0])
                exp = float(str(data[1]).split('ns')[0])
            except:
                mantisa = int(i.split('n')[0])
                exp = 0
            exp = exp - 9
            combined = float( (mantisa * (10**exp)) )
            
            B_rxtp_time.append(float(combined))
            B_rxtp_values.append(float(data_splitdata[1]))
            
# -------------- TCP Vegas --------------

    # --- Vegas CWND ---
    with open('Vegas-cwnd.data', 'r') as f:
        for i in f.read().splitlines():
            data_splitdata = i.split()
            V_cwnd_time.append(float(data_splitdata[0]))
            V_cwnd_values.append(int(data_splitdata[1]))

    # --- Vegas RTT ---
    with open('Vegas-rtt.data', 'r') as f:
        for i in f.read().splitlines():
            data_splitdata = i.split()
            V_rtt_time.append(float(data_splitdata[0]))
            V_rtt_values.append(float(data_splitdata[1]))

    # --- Vegas TX ---
    with open('Vegas-tx-throughput.data', 'r') as f:
        for i in f.read().splitlines():
            data_splitdata = i.split()
            try:
                data = i.split('e+', 1)
                mantisa = float(data[0])
                exp = float(str(data[1]).split('ns')[0])
                value = float(data_splitdata[1])
            except:
                mantisa = int(i.split('n')[0])
                exp = 0
                value = float(0)
            exp = exp - 9
            combined = float( (mantisa * (10**exp)) )
            
            V_txtp_time.append(float(combined))
            V_txtp_values.append(value)
            
    # --- Vegas RX ---
    with open('Vegas-rx-throughput.data', 'r') as f:
        for i in f.read().splitlines():
            data_splitdata = i.split()
            try:
                data = i.split('e+', 1)
                mantisa = float(data[0])
                exp = float(str(data[1]).split('ns')[0])
            except:
                mantisa = int(i.split('n')[0])
                exp = 0
            exp = exp - 9
            combined = float( (mantisa * (10**exp)) )
            
            V_rxtp_time.append(float(combined))
            V_rxtp_values.append(float(data_splitdata[1]))
    
            
    
def plotCWND():
    fig, ax = plt.subplots()
    ax.set_title("CWND")
    ax.plot(R_cwnd_time, R_cwnd_values, label='NewReno', color='red')
    #ax.plot(B_cwnd_time, B_cwnd_values, label='BBR', color='blue')
    ax.plot(V_cwnd_time, V_cwnd_values, label='Vegas', color='green')

    fig.set_size_inches(6.064, 4.445)
    plt.savefig('cwnd_plot.pgf')


def plotRTT():
    fig, ax = plt.subplots()
    ax.set_title("RTT")
    ax.plot(R_rtt_time, R_rtt_values, label='NewReno', color='red')
    ax.plot(B_rtt_time, B_rtt_values, label='BBR', color='blue')
    ax.plot(V_rtt_time, V_rtt_values, label='Vegas', color='green')
    ax.legend()
    
    fig.set_size_inches(6.064, 4.445)
    plt.savefig('rtt_plot.pgf')


def plotTX():
    fig, ax = plt.subplots()
    ax.set_title("TX")
    ax.plot(R_txtp_time, R_txtp_values, label='NewReno', color='red')
    ax.plot(B_txtp_time, B_txtp_values, label='BBR', color='blue')
    ax.plot(V_txtp_time, V_txtp_values, label='Vegas', color='green')
    ax.legend()

    fig.set_size_inches(6.064, 4.445)
    plt.savefig('tx_plot.pgf')


def plotRX():
    fig, ax = plt.subplots()
    ax.set_title("RX")
    ax.plot(R_rxtp_time, R_rxtp_values, label='NewReno', color='red')
    ax.plot(B_rxtp_time, B_rxtp_values, label='BBR', color='blue')
    ax.plot(V_rxtp_time, V_rxtp_values, label='Vegas', color='green')
    ax.legend()

    fig.set_size_inches(6.064, 4.445)
    plt.savefig('rx_plot.pgf')
    
    
    

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Dynamic graphing program for test results')
    parser.add_argument('-s',  type=str, required=True, help="Scenario one wants to plot. (ood, con, rtt)")
    
    args = parser.parse_args()
    gatherData(args.s)
    os.chdir(org_path+'/figures/'+args.s)
    plotCWND()
    plotRTT()
    plotTX()
    plotRX()
