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
draft_mode = False

N_cwnd_time = []
N_cwnd_values = []
N_rtt_time = []
N_rtt_values = []
N_tx_time = []
N_tx_values = []
N_rx_time = []
N_rx_values = []

B_cwnd_time = []
B_cwnd_values = []
B_rtt_time = []
B_rtt_values = []
B_tx_time = []
B_tx_values = []
B_rx_time = []
B_rx_values = []

V_cwnd_time = []
V_cwnd_values = []
V_rtt_time = []
V_rtt_values = []
V_tx_time = []
V_tx_values = []
V_rx_time = []
V_rx_values = []

data_dict = {
    'N' : {
        'cwnd' : [N_cwnd_time, N_cwnd_values],
        'rtt' : [N_rtt_time, N_rtt_values],
        'tx' : [N_tx_time, N_tx_values],
        'rx' : [N_rx_time, N_rx_values],
    },
    'B' : {
        'cwnd' : [B_cwnd_time, B_cwnd_values],
        'rtt' : [B_rtt_time, B_rtt_values],
        'tx' : [B_tx_time, B_tx_values],
        'rx' : [B_rx_time, B_rx_values],
    },
    'V' : {
        'cwnd' : [V_cwnd_time, V_cwnd_values],
        'rtt' : [V_rtt_time, V_rtt_values],
        'tx' : [V_tx_time, V_tx_values],
        'rx' : [V_rx_time, V_rx_values],
    }
}

def getData(scenario: str):
    file_path = data_path + scenario
    os.chdir(file_path)

    files = os.listdir('.')
    files = [f for f in files if f.endswith(".data")]
 
    for data_file in files:
        tmp = data_file.split('-')
        tcp_ver = tmp[0][0]
        type_data = tmp[1].split('.')[0]
        with open(data_file, 'r') as f:
            for i in f.read().splitlines():
                data_splitdata = i.split()
                if (type_data == 'cwnd'):
                    data_dict[tcp_ver][type_data][0].append(float(data_splitdata[0]))
                    data_dict[tcp_ver][type_data][1].append(int(data_splitdata[1]))
                elif (type_data == 'rtt'):
                    data_dict[tcp_ver][type_data][0].append(float(data_splitdata[0]))
                    data_dict[tcp_ver][type_data][1].append(float(data_splitdata[1]))
                elif (type_data == 'tx'):
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

                    data_dict[tcp_ver][type_data][0].append(float(combined))
                    data_dict[tcp_ver][type_data][1].append(value)
                elif (type_data == 'rx'):
                    try:
                        data = i.split('e+', 1)
                        mantisa = float(data[0])
                        exp = float(str(data[1]).split('ns')[0])
                    except:
                        mantisa = int(i.split('n')[0])
                        exp = 0
                    exp = exp - 9
                    combined = float((mantisa * (10**exp)))

                    data_dict[tcp_ver][type_data][0].append(float(combined))
                    data_dict[tcp_ver][type_data][1].append(float(data_splitdata[1]))
                elif (type_data == 'routes'):
                    pass
                else:
                    print(f"Unsuported data type: {type_data}")
                        

def specificPlot(tcp_ver: str, data: str):
    fig, ax = plt.subplots()
    ax.set_title(data.upper())
    ax.plot(data_dict[tcp_ver][data][0], data_dict[tcp_ver][data][1], label="{}".format(tcp_ver), color='green')

    ax.legend()

    fig.set_size_inches(6.064, 4.445)
    if (draft_mode):
        plt.savefig(f'{tcp_ver}-{data}.png')
    else:
        plt.savefig(f'{tcp_ver}-{data}.pgf')
        
def plotCWND():
    fig, ax = plt.subplots()
    ax.set_title("CWND")
    ax.set_xlabel("sec", loc='right')
    ax.set_ylabel("CWND", loc='top')
    ax.plot(data_dict['N']['cwnd'][0], data_dict['N']['cwnd'][1], label='NewReno', color='red')
    ax.plot(data_dict['B']['cwnd'][0], data_dict['B']['cwnd'][1], label='BBR', color='blue')
    ax.plot(data_dict['V']['cwnd'][0], data_dict['V']['cwnd'][1], label='Vegas', color='green')
    ax.legend()

    fig.set_size_inches(6.064, 4.445)
    if (draft_mode):
        plt.savefig('cwnd_plot.png')
    else: 
        plt.savefig('cwnd_plot.pgf')


def plotRTT():
    fig, ax = plt.subplots()
    ax.set_title("RTT")
    ax.set_xlabel("sec", loc='right')
    ax.set_ylabel("msec", loc='top')
    ax.plot(data_dict['N']['rtt'][0], data_dict['N']['rtt'][1], label='NewReno', color='red')
    ax.plot(data_dict['B']['rtt'][0], data_dict['B']['rtt'][1], label='BBR', color='blue')
    ax.plot(data_dict['V']['rtt'][0], data_dict['V']['rtt'][1], label='Vegas', color='green')
    ax.legend()
    
    fig.set_size_inches(6.064, 4.445)
    if (draft_mode):
        plt.savefig('rtt_plot.png')
    else: 
        plt.savefig('rtt_plot.pgf')


def plotTX():
    fig, ax = plt.subplots()
    ax.set_title("TX")
    ax.plot(data_dict['N']['tx'][0], data_dict['N']['tx'][1], label='NewReno', color='red')
    ax.plot(data_dict['B']['tx'][0], data_dict['B']['tx'][1], label='BBR', color='blue')
    ax.plot(data_dict['V']['tx'][0], data_dict['V']['tx'][1], label='Vegas', color='green')
    ax.legend()

    fig.set_size_inches(6.064, 4.445)
    if (draft_mode):
        plt.savefig('tx_plot.png')
    else: 
        plt.savefig('tx_plot.pgf')


def plotRX():
    fig, ax = plt.subplots()
    ax.set_title("RX")
    ax.plot(data_dict['N']['rx'][0], data_dict['N']['rx'][1], label='NewReno', color='red')
    ax.plot(data_dict['B']['rx'][0], data_dict['B']['rx'][1], label='BBR', color='blue')
    ax.plot(data_dict['V']['rx'][0], data_dict['V']['rx'][1], label='Vegas', color='green')
    ax.legend()

    fig.set_size_inches(6.064, 4.445)
    if (draft_mode):
        plt.savefig('rx_plot.png')
    else:
        plt.savefig('rx_plot.pgf')
    
    
    

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Dynamic graphing program for test results')
    parser.add_argument('-s',  type=str, required=True, help="Scenario one wants to plot. (ood, con, rtt)")
    parser.add_argument('-d', action=argparse.BooleanOptionalAction, required=False, help="Draft mode. (Output is png)")

    args = parser.parse_args()
    draft_mode = True if args.d else False

    getData(args.s)
    os.chdir(org_path+'/figures/'+args.s)

    plotCWND()
    plotRTT()
    plotTX()
    plotRX()
    
    specificPlot('N', 'cwnd')
    specificPlot('B', 'cwnd')
    specificPlot('V', 'cwnd')
