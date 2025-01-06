# ToDo

## Priority One

- Minimize RAM (use `make size` for reference). 
	- Optimize stack usage 
	- Use a memory pool for shared buffers
	- Revisit FreeRTOS and FatFs configs






# Bugs

- Printf is janky and weird, might want to rethink it
- Why does FreeRTOS want to use configTIMER_TASK_STACK_DEPTH if we're not using FreeRtos' timers?

