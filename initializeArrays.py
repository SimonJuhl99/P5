NewReno_cwnd_time = []
NewReno_cwnd_values = []
NewReno_rtt_time = []
NewReno_rtt_values = []
NewReno_tx_time = []
NewReno_tx_values = []
NewReno_rx_time = []
NewReno_rx_values = []

Bbr_cwnd_time = []
Bbr_cwnd_values = []
Bbr_rtt_time = []
Bbr_rtt_values = []
Bbr_tx_time = []
Bbr_tx_values = []
Bbr_rx_time = []
Bbr_rx_values = []

Vegas_cwnd_time = []
Vegas_cwnd_values = []
Vegas_rtt_time = []
Vegas_rtt_values = []
Vegas_tx_time = []
Vegas_tx_values = []
Vegas_rx_time = []
Vegas_rx_values = []

data_dict = {
    'NewReno' : {
        'cwnd' : [NewReno_cwnd_time, NewReno_cwnd_values],
        'rtt' : [NewReno_rtt_time, NewReno_rtt_values],
        'tx' : [NewReno_tx_time, NewReno_tx_values],
        'rx' : [NewReno_rx_time, NewReno_rx_values],
    },
    'Bbr' : {
        'cwnd' : [Bbr_cwnd_time, Bbr_cwnd_values],
        'rtt' : [Bbr_rtt_time, Bbr_rtt_values],
        'tx' : [Bbr_tx_time, Bbr_tx_values],
        'rx' : [Bbr_rx_time, Bbr_rx_values],
    },
    'Vegas' : {
        'cwnd' : [Vegas_cwnd_time, Vegas_cwnd_values],
        'rtt' : [Vegas_rtt_time, Vegas_rtt_values],
        'tx' : [Vegas_tx_time, Vegas_tx_values],
        'rx' : [Vegas_rx_time, Vegas_rx_values],
    }
}
