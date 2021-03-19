## SPH0645 on the Raspberry Pi Pico

This repository now represents a working example of recording a second of audio using the SPH0645, and printing the samples to UART.

Note: The SPH0645 uses a modified version of the typical I2S timing
diagram, it sends data to be sampled when the clock signal is low. If
you'd like to use another I2S mic that actually follows the standard,
toggle the 1s and 0s in the CLK signal in the .pio file.
