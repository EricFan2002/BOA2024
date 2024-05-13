import pandas as pd
import plotly.graph_objects as go
from plotly.subplots import make_subplots

# Read CSV data
exchange_report_df = pd.read_csv('output_exchange_report.csv')
client_report_df = pd.read_csv('output_client_report.csv')
instrument_report_df = pd.read_csv('output_instrument_report.csv')

# Create subplots with domain type for tables
fig = make_subplots(
    rows=3, cols=1,
    subplot_titles=("Exchange Report", "Client Report", "Instrument Report"),
    specs=[[{"type": "domain"}], [{"type": "domain"}], [{"type": "domain"}]]
)

# Exchange Report
exchange_table = go.Table(
    header=dict(values=["Order Id", "Rejection Reason"]),
    cells=dict(values=[exchange_report_df['Order Id'], exchange_report_df['Rejection Reason']])
)
fig.add_trace(exchange_table, row=1, col=1)

# Client Report
client_table = go.Table(
    header=dict(values=["Client ID", "Instrument ID", "Net Position"]),
    cells=dict(values=[client_report_df['Client ID'], client_report_df['Instrument ID'], client_report_df['Net Position']])
)
fig.add_trace(client_table, row=2, col=1)

# Instrument Report
instrument_table = go.Table(
    header=dict(values=["Instrument ID", "Open Price", "Close Price", "Total Volume", "VWAP", "Day High", "Day Low"]),
    cells=dict(values=[
        instrument_report_df['Instrument ID'],
        instrument_report_df['Open Price'],
        instrument_report_df['Close Price'],
        instrument_report_df['Total Volume'],
        instrument_report_df['VWAP'],
        instrument_report_df['Day High'],
        instrument_report_df['Day Low']
    ])
)
fig.add_trace(instrument_table, row=3, col=1)

# Update layout
fig.update_layout(
    height=800,
    showlegend=False,
    title_text="Trading Dashboard",
    template="plotly_white"
)

# Show the dashboard
fig.show()