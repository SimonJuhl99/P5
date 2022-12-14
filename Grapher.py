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

from initializeArrays import *

org_path = os.getcwd()
data_path = "../output_files/"
draft_mode = False


def getData(scenario: str):
    file_path = data_path + scenario
    os.chdir(file_path)

    files = os.listdir('.')
    files = [f for f in files if f.endswith(".data")]
    print(f"Gathering data from the following files: {files}")
 
    for data_file in files:
        tmp = data_file.split('-')
        tcp_ver = tmp[0]
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
                    break
                        

def specificPlot(tcp_ver: str, data: str):
    if (tcp_ver == 'NewReno'):
        color='red'
    elif (tcp_ver == 'Bbr'):
        color = 'blue'
    elif (tcp_ver == 'Vegas'):
        color = 'green'
    else:
        print(f'Unsuported tcp version: {tcp_ver}')
        
    fig, ax = plt.subplots(layout='constrained')
    ax.set_title(data.upper())
    ax.plot(data_dict[tcp_ver][data][0], data_dict[tcp_ver][data][1], label="{}".format(tcp_ver), color=color)

    ax.legend(bbox_to_anchor=(1,0), loc='lower left')

    fig.set_size_inches(6.064, 4.445)
    if (draft_mode):
        plt.savefig(f'{tcp_ver}-{data}.png')
    else:
        plt.savefig(f'{tcp_ver}-{data}.pgf')
        
def plotCWND():
    fig, ax = plt.subplots(layout='constrained')
    ax.set_title("CWND")
    ax.set_xlabel("sec", loc='right')
    ax.set_ylabel("B", loc='top')
    ax.plot(data_dict['NewReno']['cwnd'][0], data_dict['NewReno']['cwnd'][1], label='NewReno', color='red')
    ax.plot(data_dict['Bbr']['cwnd'][0], data_dict['Bbr']['cwnd'][1], label='BBR', color='blue')
    ax.plot(data_dict['Vegas']['cwnd'][0], data_dict['Vegas']['cwnd'][1], label='Vegas', color='green')
    ax.legend(bbox_to_anchor=(1,0), loc='lower left')

    fig.set_size_inches(6.064, 4.445)
    if (draft_mode):
        plt.savefig('cwnd_plot.png')
    else: 
        plt.savefig('cwnd_plot.pgf')


def plotRTT():
    fig, ax = plt.subplots(layout='constrained')
    ax.set_title("RTT")
    ax.set_xlabel("sec", loc='right')
    ax.set_ylabel("sec", loc='top')
    ax.plot(data_dict['NewReno']['rtt'][0], data_dict['NewReno']['rtt'][1], label='NewReno', color='red')
    ax.plot(data_dict['Bbr']['rtt'][0], data_dict['Bbr']['rtt'][1], label='BBR', color='blue')
    ax.plot(data_dict['Vegas']['rtt'][0], data_dict['Vegas']['rtt'][1], label='Vegas', color='green')
    ax.legend(bbox_to_anchor=(1,0), loc='lower left')
    
    fig.set_size_inches(6.064, 4.445)
    if (draft_mode):
        plt.savefig('rtt_plot.png')
    else: 
        plt.savefig('rtt_plot.pgf')


def plotTX():
    fig, ax = plt.subplots(layout='constrained')
    ax.set_title("TX")
    ax.set_xlabel("sec", loc='right')
    ax.set_ylabel("Mb/s", loc='top')
    ax.plot(data_dict['NewReno']['tx'][0], data_dict['NewReno']['tx'][1], label='NewReno', color='red')
    ax.plot(data_dict['Bbr']['tx'][0], data_dict['Bbr']['tx'][1], label='BBR', color='blue')
    ax.plot(data_dict['Vegas']['tx'][0], data_dict['Vegas']['tx'][1], label='Vegas', color='green')
    ax.legend(bbox_to_anchor=(1,0), loc='lower left')

    fig.set_size_inches(6.064, 4.445)
    if (draft_mode):
        plt.savefig('tx_plot.png')
    else: 
        plt.savefig('tx_plot.pgf')


def plotRX():
    fig, ax = plt.subplots(layout='constrained')
    ax.set_title("RX")
    ax.set_xlabel("sec", loc='right')
    ax.set_ylabel("Mb/s", loc='top')
    ax.plot(data_dict['NewReno']['rx'][0], data_dict['NewReno']['rx'][1], label='NewReno', color='red')
    ax.plot(data_dict['Bbr']['rx'][0], data_dict['Bbr']['rx'][1], label='BBR', color='blue')
    ax.plot(data_dict['Vegas']['rx'][0], data_dict['Vegas']['rx'][1], label='Vegas', color='green')
    ax.legend(bbox_to_anchor=(1,0), loc='lower left')

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
    
    specificPlot('NewReno', 'cwnd')
    specificPlot('Bbr', 'cwnd')
    specificPlot('Vegas', 'cwnd')

    specificPlot('NewReno', 'rtt')
    specificPlot('Bbr', 'rtt')
    specificPlot('Vegas', 'rtt')

    specificPlot('NewReno', 'rx')
    specificPlot('Bbr', 'rx')
    specificPlot('Vegas', 'rx')

    specificPlot('NewReno', 'tx')
    specificPlot('Bbr', 'tx')
    specificPlot('Vegas', 'tx')

    
