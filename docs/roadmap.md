# Roadmap to Arcadia 1.0.0 (Closed Beta Testing)

### ☑️ version 0.6.0
<b>Hardware</b>:
- Arcadia Prototype PCB Rev. A1
- Enclosure development

<b>New firmware features</b>:
- Bookmarking
- Full arcproject pipeline testing

---

### ☑️ version 0.5.0
<b>Hardware</b>:
- Arcadia Prototype PCB Rev. A1

<b>New firmware features</b>:
- RAM optimization
- DISPLAY task
- Display drivers
- Loadable character sets
- Basic text advancing, wrapping
- Menu & menu behaviour rendering

---

### ☑️ version 0.4.0
<b>Hardware</b>:
- Arcadia Prototype PCB Rev. A1 smoke test & bringup, including validation of:
	- LiPo Battery management
	- Power selection
	- SD bus mux/ protection
	- Snap dome interfaces

<b>New firmware features</b>:
- 64 pin map support for ATSAMC21J18A
- Basic firmware image validation
- SD card detection

---

### ✅ version 0.3.0

<b>Notes</b>:
<br>
<br>
This was a mixed bag of higher lever/ application-specific components of the project. Quite of a bit of directory traversal/ file system manipulation logic + CLI commands were added. JSON support was built on top of jsmn. We began writing the state machines which decouple the application state from the display logic. We also decided to simulate the physical interface buttons on the keyboard so application state can be tracked on the CLI. This should work using the actual physical buttons as well, when they're added. We also worked on developing a spec for arcprojects and began adding the requisite firmware support.

<b>Hardware</b>:
- Wonka Board + SD module

<b>New firmware features</b>:
- Directory navigation
- Story structure implementation
- Story recognition and processing development
- Application state machine development
- "Mirrored Interface" Development
- JSON encoding/ decoding interface
- Button interface

---

### ✅ version 0.2.0

<b>Notes</b>:
<br>
<br>
This version was all about the SD card stack. We went from SPI all the way up to having a named and formatted SD card enumerate on a PC as a FAT32 volume. Lots of CLI commands were added. Learned a decent bit about FAT file systems, FatFs, and SD cards. An SD card adapter was added to the Wonka Board and seems to work relatively well. Sometimes SD card initialization fails due to some elusive electrical problem. I can usually get around this issue by removing and reinserting the adapter.

<b>Hardware</b>:
- Wonka Board + SD module

<b>New firmware features</b>:
- SPI driver + SD card interface
- FatFs integration
- PC / device FAT32 file system interoperability

---

### ✅ version 0.1.0

<b>Notes</b>:
<br>
<br>
FreeRTOS integration and wrapper/ interface for the project's tasks. Validating inter-task communication via CLI. A lot of work went into CHRONO's message scheduling which may be removed in the future to save memory. All of CHRONO's scheduling is performed via a pool of hardware timers clocked off the internal 32.768 KHz oscillator. Much of this and the inter-task messaging was an educational journey, as it was our first crack at integrating FreeRTOS into a personal project. We blew our stack multiple times and it took a while to get things more or less correct in FreeRTOSConfig.h. The debug shell grew considerably in this version, and was refactored a few times.

<b>Hardware</b>:
- Wonka Board

<b>New firmware features</b>:
- FreeRTOS Integration
- Continued driver development
- SHELL, CHRONO, DRIVE tasks + development
- Inter-task messaging support
- Expanded CLI utilities

---

### ✅ version 0.0.0

<b>Notes</b>:
<br>
<br>
The very basics. Internal oscillator and system clock configuration and validation. A few drivers, validating our toolchain, and sketching out a debug shell. The "Wonka Board" doesn't have much on it at all. It's just a 48 pin SAMC21, 3 decoupling capacitors and a TAG-Connect interface for SWD. Very boring. We like boring.

<b>Hardware</b>:
- Wonka Board

<b>New firmware features</b>:
- Toolchain development
- Low level system setup and driver developement.
- Debug shell development