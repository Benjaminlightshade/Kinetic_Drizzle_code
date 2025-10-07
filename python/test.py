import serial
import numpy as np
import re
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.animation import FuncAnimation


# Initialize serial
ser = serial.Serial('COM8', 115200)  
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

def update_graph():
    frame_data = []
    decimal_list = []
    x = 0    
    while True:
        if ser.in_waiting:
            raw_data = ser.readline().decode('utf-8').strip()  # Strip whitespace
            print(raw_data)
            if "ROW" in raw_data:
                raw_data = raw_data.split(')', 1)[-1]  # Get everything after ')'

                hex_data_line = re.findall(r'\b[A-Fa-f0-9]{4}\b', raw_data)

                # Convert XXXX to decimal value
                decimal_list = [int(hex_data, 16) for hex_data in hex_data_line]

            # Process and update x based on the row
            if "ROW0" in raw_data:
                x = 0
            elif "ROW1" in raw_data:
                x = 1
            elif "ROW2" in raw_data:
                x = 2
            elif "ROW3" in raw_data:   
                x = 3

        for index, value in enumerate(decimal_list):

            # Append the tuple to frame_data
            frame_data.append((x, index, value))

        if x == 3:
            break

    # Call function to update the position 
    plot_graph(frame_data)



def plot_graph(frame_data):
    ax.cla()  # Clear the previous plot data

    # Set limits and ticks after clearing
    ax.set_xlim(0, 4)
    ax.set_xticks(range(0, 5))  # Set x-axis ticks from 0 to 4 with interval of 1
    ax.set_ylim(0, 8)
    ax.set_yticks(range(0, 8))  # Set y-axis ticks from 0 to 7 with interval of 1
    ax.set_zlim(0, 100)
    ax.set_zticks(range(0, 101, 10))  # Set z-axis ticks from 0 to 100 with interval of 1

    # Create the new scatter plot
    for point in frame_data:
        ax.scatter(point[0], point[1], point[2])
    plt.draw()
    plt.pause(1)



# ani = FuncAnimation(fig, update, interval=100)  # Update every 100 ms


# Update the plot
# plt.pause(1)

# Start updating the graph
while 1:
    update_graph()
