import numpy as np
import re
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.animation import FuncAnimation
import time

x_size = 4
y_size = 8
z_upper_limit = 0
z_lower_limit = 10000
inverted = False

class Visualizer3D:

    idle = False

    def __init__(self, x_size=4, y_size=8, z_min=z_lower_limit, z_max=z_upper_limit):
        plt.ion()  # interactive mode ON

        self.fig = plt.figure()
        self.ax = self.fig.add_subplot(111, projection="3d")

        self.ax.set_xlim(0, x_size)
        self.ax.set_ylim(0, y_size)
        self.ax.set_zlim(z_min, z_max)

        self.ax.set_xticks(range(x_size + 1))
        self.ax.set_yticks(range(y_size + 1))
        # self.ax.set_zticks(range(z_min, z_max + 1, 10))
        # Create ONE scatter object
        self.scatter = self.ax.scatter([], [], [])

        plt.show(block=False)

    def update(self, frame_data):
        """
        frame_data: list of (x, y, z)
        """
        if not frame_data:
            return

        xs, ys, zs = zip(*frame_data)

        # Update scatter data
        self.scatter._offsets3d = (xs, ys, zs)

        self.fig.canvas.draw_idle()
        plt.pause(0.001)


### Example usage ###
# viz = Visualizer3D()

# while True:
#     viz.update([(0, 0, 0), (1, 2, 30), (2, 4, 60), (3, 6, 90)])
#     time.sleep(2)
#     viz.update([(0, 1, 10), (1, 3, 40), (2, 5, 70), (3, 7, 100)])
#     time.sleep(2)




