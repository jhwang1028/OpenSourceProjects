import socket
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from scipy import stats
import time

# TCP server settings
HOST = '192.168.0.170'  # PC IP address
PORT = 5000  # Port number

# Function to receive data from ESP32
def receive_data(duration=60):
    start_time = time.time()  # Record the start time
    data = []
    
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen()
        print('Waiting for connection...')
        conn, addr = s.accept()
        with conn:
            print('Connected by', addr)
            while True:
                # Stop receiving after the specified duration
                if time.time() - start_time > duration:
                    print(f"Data collection finished after {duration} seconds.")
                    break

                try:
                    current_data = conn.recv(1024).decode().strip()  # Receive and strip whitespace/newlines
                    if not current_data:
                        break
                    current_lines = current_data.split("\n")  # Split by newline
                    for line in current_lines:
                        line = line.strip()  # Strip extra spaces
                        if line:  # Ensure line is not empty
                            try:
                                current_float = float(line)  # Convert to float
                                data.append(current_float)
                                print(f"Received current: {current_float} A")  # Print received current
                            except ValueError:
                                print(f"Received invalid data: {line}")
                                continue
                except Exception as e:
                    print(f"Error receiving data: {e}")
                    break

    return data

# Function to perform data analysis
def analyze_data(data):
    time_array = np.arange(0, len(data) * 0.002, 0.002)  # Time array (seconds)
    power = np.array(data) * 220  # Power consumption (Current * 220V)

    # Plot current over time
    plt.figure()
    plt.plot(time_array, data)
    plt.title("Current over Time")
    plt.xlabel("Time (s)")
    plt.ylabel("Current (A)")
    plt.savefig("current_over_time.png")
    plt.show()

    # Statistical analysis: max, min, mean, variance, and range
    print(f"Max current: {np.max(data)} A")
    print(f"Min current: {np.min(data)} A")
    print(f"Mean current: {np.mean(data)} A")
    print(f"Current variance: {np.var(data)}")
    print(f"Current range: {np.ptp(data)}")

    # Plot power consumption over time
    plt.figure()
    plt.plot(time_array, power)
    plt.title("Power Consumption over Time")
    plt.xlabel("Time (s)")
    plt.ylabel("Power (W)")
    plt.savefig("power_over_time.png")
    plt.show()

    # Time series analysis using moving average
    moving_avg = pd.Series(data).rolling(window=60).mean()
    plt.figure()
    plt.plot(time_array, data, label="Original Data")
    plt.plot(time_array, moving_avg, label="Moving Average (60 seconds)", color='orange')
    plt.title("Time Series Analysis")
    plt.xlabel("Time (s)")
    plt.ylabel("Current (A)")
    plt.legend()
    plt.savefig("time_series_analysis.png")
    plt.show()

    # Outlier detection using z-scores
    z_scores = np.abs(stats.zscore(data))
    outliers = np.where(z_scores > 3)
    print(f"Outlier indices: {outliers}")
    plt.figure()
    plt.plot(time_array, data, label="Current Data")
    plt.scatter(time_array[outliers], np.array(data)[outliers], color='red', label="Outliers")
    plt.title("Outlier Detection")
    plt.xlabel("Time (s)")
    plt.ylabel("Current (A)")
    plt.legend()
    plt.savefig("outlier_detection.png")
    plt.show()

# Main execution
data = receive_data()
analyze_data(data)
