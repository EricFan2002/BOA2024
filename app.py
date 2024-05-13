import pandas as pd
import plotly.graph_objects as go
from plotly.subplots import make_subplots
import dash
import dash_core_components as dcc
import dash_html_components as html
from dash.dependencies import Input, Output

# Read CSV data
exchange_report_df = pd.read_csv('output_exchange_report.csv')
client_report_df = pd.read_csv('output_client_report.csv')
instrument_report_df = pd.read_csv('output_instrument_report.csv')

# Create a Dash app
app = dash.Dash(__name__)

# Define the layout
app.layout = html.Div([
    html.H1("Trading Dashboard"),
    dcc.Tabs([
        dcc.Tab(label="Exchange Report", children=[
            html.Div([
                dcc.Dropdown(
                    id='exchange-dropdown',
                    options=[{'label': order_id, 'value': order_id} for order_id in exchange_report_df['Order Id'].unique()],
                    placeholder="Select an Order ID"
                ),
                html.Div(id='exchange-chart')
            ])
        ]),
        dcc.Tab(label="Client Report", children=[
            html.Div([
                dcc.Dropdown(
                    id='client-dropdown',
                    options=[{'label': client_id, 'value': client_id} for client_id in client_report_df['Client ID'].unique()],
                    placeholder="Select a Client ID"
                ),
                html.Div(id='client-chart')
            ])
        ]),
        dcc.Tab(label="Instrument Report", children=[
            html.Div([
                dcc.Dropdown(
                    id='instrument-dropdown',
                    options=[{'label': instrument_id, 'value': instrument_id} for instrument_id in instrument_report_df['Instrument ID'].unique()],
                    placeholder="Select an Instrument ID"
                ),
                html.Div(id='instrument-chart')
            ])
        ])
    ])
])

# Callback for Exchange Report
@app.callback(Output('exchange-chart', 'children'),
              [Input('exchange-dropdown', 'value')])
def update_exchange_chart(order_id):
    if order_id is None:
        return None

    # Filter data based on selected Order ID
    filtered_df = exchange_report_df[exchange_report_df['Order Id'] == order_id]

    # Create chart based on filtered data
    chart = go.Figure(data=[go.Table(
        header=dict(values=["Order Id", "Rejection Reason"]),
        cells=dict(values=[filtered_df['Order Id'], filtered_df['Rejection Reason']])
    )])

    return dcc.Graph(figure=chart)

# Callback for Client Report
@app.callback(Output('client-chart', 'children'),
              [Input('client-dropdown', 'value')])
def update_client_chart(client_id):
    if client_id is None:
        return None

    # Filter data based on selected Client ID
    filtered_df = client_report_df[client_report_df['Client ID'] == client_id]

    # Create chart based on filtered data
    chart = go.Figure(data=[go.Table(
        header=dict(values=["Client ID", "Instrument ID", "Net Position"]),
        cells=dict(values=[filtered_df['Client ID'], filtered_df['Instrument ID'], filtered_df['Net Position']])
    )])

    return dcc.Graph(figure=chart)

# Callback for Instrument Report
@app.callback(Output('instrument-chart', 'children'),
              [Input('instrument-dropdown', 'value')])
def update_instrument_chart(instrument_id):
    if instrument_id is None:
        return None

    # Filter data based on selected Instrument ID
    filtered_df = instrument_report_df[instrument_report_df['Instrument ID'] == instrument_id]

    # Create chart based on filtered data
    chart = go.Figure(data=[go.Table(
        header=dict(values=["Instrument ID", "Open Price", "Close Price", "Total Volume", "VWAP", "Day High", "Day Low"]),
        cells=dict(values=[
            filtered_df['Instrument ID'],
            filtered_df['Open Price'],
            filtered_df['Close Price'],
            filtered_df['Total Volume'],
            filtered_df['VWAP'],
            filtered_df['Day High'],
            filtered_df['Day Low']
        ])
    )])

    return dcc.Graph(figure=chart)

if __name__ == '__main__':
    app.run_server(debug=True)