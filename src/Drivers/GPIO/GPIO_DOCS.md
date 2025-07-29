# General Structure

The code is divided into low-level (kernel) module and high-level (OS) module. The kernel module provides only the most basic instructions for OS module to use in more abstracted and complicated ways.

## Kernel module

Kernel module contains GPIO initialization for output and input, which is hardcoded as analog-output for A bus and digital-output for B bus. The C bus can be selected by user. The module also contains GPIO read and write instructions. This module also has pin abstraction handling included and is based purely on that.