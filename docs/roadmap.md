# Roadmap to Arcadia 1.0.0 (Closed Beta Testing)

### About

This roadmap serves as a high-level progress tracker for Arcadia's firmware and hardware development, outlining the milestones leading up to version 1.0.0 (Closed Beta Testing). It is not intended as full system documentation but rather as an overview of key development phases, highlighting major features, hardware revisions, and architectural decisions.

Each version milestone reflects incremental progress toward a stable and feature-complete firmware, ensuring that core systems are functional and ready to support further development. Hardware iterations are also tracked, from initial development on the Wonka Board to the Arcadia Prototype PCB Rev. A.

The ultimate goal of this roadmap is to reach version 1.0.0, marking the transition to Closed Beta Testing, where Arcadia's hardware and firmware will be put into real-world user scenarios to validate stability, usability, and performance before wider release.

---

### ☑️ version 0.6.0 - <i>App-specific Development II</i>
<b>Hardware</b>:
- Arcadia Prototype PCB Rev. A
- Enclosure development

<b>New firmware features</b>:
- Bookmarking (save/ resume)
- Full arcproject pipeline testing

---

### ☑️ version 0.5.0 - <i>Rev. A Bring-up</i>
<b>Hardware</b>:
- Arcadia Prototype PCB Rev. A smoke test & bringup, including validation of:
	- Display validation
	- SD bus mux/ protection
	- Snap dome interfaces

<b>New firmware features</b>:
- 64 pin map support for ATSAMC21J18A
- Basic firmware image validation
- SD card detection

---

### ☑️ version 0.4.0 - <i>Display Development</i>
<b>Hardware</b>:
- Protoboard setup (SD module, display, pushbuttons, etc., but no battery/ charging)

<b>New firmware features</b>:
- RAM optimization
- DISPLAY task
- Display drivers
- Loadable character set(s)
- Basic text advancing, wrapping
- Menu behaviour validation on display
- Button module
- SD detection

---

### ✅ version 0.3.0 - <i>App-specific Development I</i>

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

### ✅ version 0.2.0 - <i>File System Development</i>

<b>Notes</b>:
<br>
<br>
This version pertained almost entirely to the SD card stack and the DRIVE task. We implemented an SPI driver and logical SD channel, and worked up from that layer all the way to having a named and formatted SD card enumerate on a PC as a FAT32 volume. Lots of CLI commands were added and we learned a decent bit about FAT file systems, FatFs, and SD cards. An SD card adapter was added to the Wonka Board and seems to work relatively well. Sometimes SD card initialization fails due to some elusive electrical problem. We can usually get around this issue by removing and reinserting the adapter. This could be an issue with the adapter design itself - requires some further investigation. The SD card and the TFT display are the two main components of the system. Having a solid SD card stack in place is a major step forward.

<b>Hardware</b>:
- Wonka Board + SD module

<b>New firmware features</b>:
- SPI driver + SD card interface
- FatFs integration
- PC / device FAT32 file system interoperability

---

### ✅ version 0.1.0 - <i>FreeRTOS Integration</i>

<b>Notes</b>:
<br>
<br>
In this version we implemented a FreeRTOS wrapper/ interface for the project's tasks. We refactored our debug shell into an actual task, and then added the DRIVE and CHRONO tasks. The concept of messages and message payloads were added and inter-task communication was validated via the CLI. As an aside, a lot of work went into CHRONO's message scheduling which may be removed in the future to save memory. All of CHRONO's scheduling is performed via a pool of hardware timers clocked from the internal 32.768 KHz oscillator. It's definitely worth preserving this functionality, but as RAM usage increases, we may want to roll CHRONO into the DRIVE task. Or something similar. Much of the work in this version was an educational journey, as it was our first attempt at integrating FreeRTOS into a personal project. We regularly overflowed our stack and it took a while to get things more or less correct in FreeRTOSConfig.h. The debug shell grew considerably in this version, and was refactored a few times.

<b>Hardware</b>:
- Wonka Board

<b>New firmware features</b>:
- FreeRTOS Integration
- Continued driver development
- SHELL, CHRONO, DRIVE tasks + development
- Inter-task messaging support
- Expanded CLI utilities

---

### ✅ version 0.0.0 - <i>The Basics</i>

<b>Notes</b>:
<br>
<br>
The very basics. Internal oscillator and system clock configuration and validation. Added UART/ SERCOM, NVMCTRL, and GPIO drivers and the necessary interrupt handlers, validated our toolchain, and sketched out a debug shell. All of this running in a simple superloop. The "Wonka Board" doesn't have much on it. It's just a 48 pin SAMC21 and a TAG-Connect interface for SWD.

<b>Hardware</b>:
- Wonka Board

<b>New firmware features</b>:
- Toolchain development
- Low level system setup and driver developement.
- Debug shell development