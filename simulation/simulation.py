import ctypes
import time
import numpy as np
import os
import sys

import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.animation import FuncAnimation

from drops_plot import Visualizer3D

pwd_path = os.getcwd()

# Load library
if sys.platform.startswith("win"):
    path = os.path.join(pwd_path, "build", "librain_compute.dll")
    lib = ctypes.CDLL(path)
else:
    path = os.path.join(pwd_path, "build", "librain_compute.so")
    lib = ctypes.CDLL(path)

print("Path to find the library is in: " + path)

# Constants (must match config.h)
X_SIZE = 4
Y_SIZE = 8

# Test function to see if the library can be loaded correctly
# lib.testfunc.restype = ctypes.c_int
# print("Test function returns:", lib.testfunc())


# Define ctypes 2D array TYPE
PosArray = (ctypes.c_int * Y_SIZE) * X_SIZE

# Function signatures
lib.initComputeNextPositions.restype = ctypes.c_int
lib.computeNextPositions.argtypes = [ctypes.POINTER(ctypes.c_int * Y_SIZE)]
lib.computeNextPositions.restype = ctypes.c_int

# Init
lib.initComputeNextPositions()

# Allocate buffer
pos = PosArray()

# Run based on time 
timetorun = 100           # Time in seconds 
starttime = time.time()
elapsed_time = 0
print("Start time:", time.strftime("%H:%M:%S", time.localtime(starttime)))

viz = Visualizer3D()

while(elapsed_time < timetorun):
    elapsed_time = time.time() - starttime
    computeresult = lib.computeNextPositions(pos)

    lib.computeNextPositions(pos)
    np_pos = np.ctypeslib.as_array(pos)
    viz.update([(x, y, np_pos[x][y]) for x in range(X_SIZE) for y in range(Y_SIZE)])

    print("Time now:", time.strftime("%H:%M:%S", time.localtime()))
    time.sleep(0.1)





