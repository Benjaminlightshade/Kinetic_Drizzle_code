import ctypes
import time
import numpy as np
import os
import sys

from drops_plot import Visualizer3D

# Constants (must match config.h)
X_SIZE = 4
Y_SIZE = 8

# Define ctypes 2D array TYPE
PosArray = (ctypes.c_int * Y_SIZE) * X_SIZE


def load_library():
    pwd_path = os.getcwd()
    
    # Load library
    if sys.platform.startswith("win"):
        path = os.path.join(pwd_path, "build", "librain_compute.dll")
        lib = ctypes.CDLL(path)
    else:
        path = os.path.join(pwd_path, "build", "librain_compute.so")
        lib = ctypes.CDLL(path)
    
    print("Path to find the library is in: " + path)
    return lib


def setup_library(lib):
    # Function signatures
    lib.initComputeNextPositions.restype = ctypes.c_int
    lib.computeNextPositions.argtypes = [ctypes.POINTER(ctypes.c_int * Y_SIZE)]
    lib.computeNextPositions.restype = ctypes.c_int
    lib.computePositionsWithTime.argtypes = [ctypes.POINTER(ctypes.c_int * Y_SIZE), ctypes.c_int64, ctypes.c_int]
    lib.computePositionsWithTime.restype = ctypes.c_int
    
    # Init
    lib.initComputeNextPositions()

def timed_run(pos, lib):
    
    # Run based on actual time 
    timetorun = 100           # Time in seconds 
    starttime = time.time()
    elapsed_time = 0
    print("Start time:", time.strftime("%H:%M:%S", time.localtime(starttime)))
    
    viz = Visualizer3D()
    
    while(elapsed_time < timetorun):
        elapsed_time = time.time() - starttime
    
        lib.computeNextPositions(pos)
        
        np_pos = np.ctypeslib.as_array(pos)
        viz.update([(x, y, np_pos[x][y]) for x in range(X_SIZE) for y in range(Y_SIZE)])
    
        print("Time now:", time.strftime("%H:%M:%S", time.localtime()))
        time.sleep(0.1)

def simulated_run(pos,lib):
    # Run based on simulated time 
    sim_time = 0
    total_sim_time = 100 * 1000000 # Simulate for 100 seconds given in microseconds
    sim_time_step = 5000000 # Time step between each frame update in microseconds
    refresh_rate = 5 # Speed of refresh in Hz. Increase this to run the simulation faster.
    print("Simulated start time: 0 us")
    
    viz = Visualizer3D()

    seq_select = 0

    while(sim_time < total_sim_time):
    
        lib.computePositionsWithTime(pos, sim_time, seq_select) # Get position at current simulated time for SEQ1
        
        np_pos = np.ctypeslib.as_array(pos)
        viz.update([(x, y, np_pos[x][y]) for x in range(X_SIZE) for y in range(Y_SIZE)])
    
        print(f"Simulated time now: {sim_time} us")
        sim_time += sim_time_step
        time.sleep(1/refresh_rate)


def main():
    lib = load_library()
    setup_library(lib)

    # Allocate buffer
    pos = PosArray()

    ### Uncomment one of the following to run the desired simulation mode ###

    # Run the simulation based on actual time or simulated time.
    # timed_run(pos, lib)

    # Simulated run with custom time inputs for testing patterns.
    simulated_run(pos, lib)



if __name__ == "__main__":
    main()





