If you are having trouble running the repository try the PROS command `pros make all` to completely rebuild the project. This should only be necessary to
run the first time you run the program after downloading.
# Valor-Display

A PROS-based VEX V5 robotics project that provides a custom brain display UI for autonomous selection, alliance choice, and motor temperature monitoring.

This project uses LVGL for the display, along with PROS for VEX V5 robot control.

## Features

- Autonomous routine selector with LVGL-based buttons
- Alliance color selection (Blue / Red)
- Variation selection for multiple autonomous setups
- Motor temperature monitoring screen
- Controller display feedback for selected routine and status
- Integration with the VEX V5 brain and PROS tasks
- (Switching between arcade, tank, and split drive modes)

## Project Overview

This repository contains a custom display system for a VEX robot brain. It manages:

- routine selection from a touchscreen-style UI
- display updates based on alliance and variation
- motor health monitoring during driver control
- controller LCD feedback while disabled and driving

The primary logic is split into:
- `src/main.cpp` — main robot code, tasks, drive config, and competition lifecycle
- `src/displays.cpp` — LVGL display UI and temperature monitoring implementation
- `include/` — project headers and PROS/LVGL interface files

## Hardware Targets

- VEX V5 Brain
- PROS (Project for Robot Operating System)
- V5 motors and sensors used by the robot
- Useed with a controller and display screen

## Repository Structure

```text
Valor-Display/
├── firmware/
│   ├── libc.a
│   ├── liblvgl.a
│   ├── libm.a
│   ├── libpros.a
│   ├── v5-common.ld
│   ├── v5-hot.ld
│   └── v5.ld
├── include/
│   ├── api.h
│   ├── displays.h
│   ├── jb_math.h
│   ├── main.h
│   ├── liblvgl/
│   └── pros/
├── src/
│   ├── displays.cpp
│   ├── jb_math.cpp
│   └── main.cpp
├── .gitignore
├── Makefile
├── common.mk
├── project.pros
└── README.md
