# Roadmap to 1.0 Beta

### - [x] version 0.0.0

Hardware:
- Wonka Board

Firmware:<br>
- Low level system setup and driver developement.
- Debug shell development

### version 0.1.0
Hardware:
- Wonka Board

Firmware:<br>
- FreeRTOS Integration
- Inter-task messaging
- Expanded CLI utilities

### version 0.2.0
Hardware:
- Wonka Board + SD module

Firmware:<br>
- SPI driver + SD card interface
- FatFs integration


# ToDo

## Priority One

- Minimize RAM (use `make size` for reference). 
	- Optimize stack usage 
	- Use a memory pool for shared buffers
	- Revisit FreeRTOS and FatFs configs






# Bugs

- Printf is janky and weird, might want to rethink it
- Why does FreeRTOS want to use configTIMER_TASK_STACK_DEPTH if we're not using FreeRtos' timers?

