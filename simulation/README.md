# Simulation for raindrops

This folder contains the simulation code for the Kinetic Drizzle project, which simulates kinetic rain or drizzle effects.

## Build the rain_compute shared library
Build the shared library from the main rain_compute.cpp file. This workflow runs on windows and uses cmake and Ninja. 

`mkdir build`

`cmake -S . -B build -G Ninja`

`cmake --build build `

## Run the simulation
Run the main simulation script after building the shared library. 

`python simulation.py`

