import ctypes
import numpy as np
import os
import sys

import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.animation import FuncAnimation

# import drops_plot

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

# Run frames
for i in range(100):
    lib.computeNextPositions(pos)
    np_pos = np.ctypeslib.as_array(pos)
    print("Frame", i, np_pos)
