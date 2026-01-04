![Logo](docs/assets/logo.svg)
                           

![Version](https://img.shields.io/badge/Version-0.5.0-brightgreen?style=flat-square) ![Platform](https://img.shields.io/badge/Platform-Cortex_M0%2B-blue?style=flat-square&logo=arm&logoColor=rgb(0,145,189)&color=rgb(0,145,189)) ![License: Proprietary](https://img.shields.io/badge/License-Proprietary-red?style=flat-square)

## TODO

- Clean up bootloader Makefile
	- everything for the bootloader should build in src/bootloader/build

- Add support for other SD card types

- Place all driver files in src/drivers

- Make printf make sense between bootloader/ app

- Implement firmware update

- Add memory_map.ld and import this into linkerscripts

- Move LOAD task logic into DRIVE and delete LOAD task

<p align="center">© 2025 Henry Wynberg. All Rights Reserved.</p>