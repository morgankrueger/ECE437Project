import pandas as pd 
import numpy as np
import matplotlib.pyplot as plt
import csv

# List of CSV file paths
csv_files = ["2cars7seats.csv", "4cars7seats.csv", "6cars7seats.csv"]

#plot the waiting queue
plt.figure(1)  # First figure
for file in csv_files:
    data = pd.read_csv(file)
    x = data.iloc[:, 0]  # Column 1
    y = data.iloc[:, 2]  # Column 3
    plt.plot(x, y, label=file)

# Plot all data on the same plot
plt.xlabel('Time (Minutes)')
plt.ylabel('# of persons in waiting')
plt.title('Waiting Queue')
plt.legend()  # Add a legend to distinguish files
plt.grid(True)

# Plot the rejected
plt.figure(2)  # Second figure
for file in csv_files:
    data = pd.read_csv(file)
    x = data.iloc[:, 0]  # Column 1
    y = data.iloc[:, 3]  # Column 4
    plt.plot(x, y, label=file)

plt.xlabel('Time (minutes)')
plt.ylabel('# of persons being rejected')
plt.title('GoAway People')
plt.legend()
plt.grid(True)

# Plot Column 2 vs Column 1 for a specific file
specific_file = "2cars7seats.csv" 
plt.figure(3)  # Third figure
data = pd.read_csv(specific_file)
x = data.iloc[:, 0]  # Column 1
y = data.iloc[:, 1]  # Column 2
plt.plot(x, y, "r-D", markersize=4, linewidth='.5',label="25,45,35,25")

plt.xlabel('Time (minutes)')
plt.ylabel('Arrival #')
plt.title('People Arrival')
plt.legend()
plt.grid(linestyle='dashed')

# Show all figures
plt.show()
