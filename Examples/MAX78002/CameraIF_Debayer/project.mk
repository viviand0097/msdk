# This file can be used to set build configuration
# variables.  These variables are defined in a file called 
# "Makefile" that is located next to this one.

# For instructions on how to use this system, see
# https://github.com/Analog-Devices-MSDK/VSCode-Maxim/tree/develop#build-configuration

#MXC_OPTIMIZE_CFLAGS = -Og
# ^ For example, you can uncomment this line to 
# optimize the project for debugging

# **********************************************************

# Set the camera driver to the HM0360 color drivers
# These are the only drivers supported by this example.
CAMERA=HM0360_COLOR

# Enable optimization level 2 (faster code but this should be turned off for debugging)
MXC_OPTIMIZE_CFLAGS = -O2

# Uncomment to use legacy Adafruit 3315 TFT drivers (TFT = ADAFRUIT)
# Otherwise, default drivers (TFT = NEWHAVEN) will be used for NewHaven NHD-2.4
# TFT = ADAFRUIT
