# PEMaker

PEMaker makes a bootable ISO file for a Windows Preinstallation Environment (WindowsPE).


## Features

* Run on Windows 7/10/11
* Just need a [Windows installer file (ISO)](https://www.microsoft.com/software-download/)
* No need Windows AIK/ADK
* Support for MBR(BIOS)/GPT(UEFI)
* Support for creating an ISO image for 32bit/64bit
* Support for customizing applications by editing setting files
* Portable. No installation needed
* Free for personal and commercial use

## Project Status

This project is in development phase.

## Compiling

* msys2(mingw32)
* ckcore
* ckfilesystem(modified version including in this repo is needed)
* libcdio-2.3.0
* wimlib-1.14.4
* wxWidgets-3.2.10

