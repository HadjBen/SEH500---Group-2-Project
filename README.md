# SEH500 Project - Assistive Communication Device

## Quick Start

1. **Open Project:** Open `SEH500_Project` in MCUXpresso IDE
2. **Build:** Project → Build All (should compile successfully)
3. **Flash:** Flash to FRDM-K66F board
4. **Test:**
   - Press SW2 → Green LED flickers (water alert)
   - Press SW3 → Red LED flickers (washroom alert)
   - Type 'W' in serial terminal → Water alert
   - Type 'T' in serial terminal → Washroom alert
   - Press same button/key again → Cancels alert

## Current Status

✅ **Complete:** Buttons, LEDs, UART keyboard control, state machine, interrupt handlers, assembly code

## Documentation

All documentation is in the `documents/` folder:

- **`documents/SEH500_Project_Report_Revised.md`** - Final technical report (for submission)
- **`documents/CODE_EXPLANATION.md`** - Detailed line-by-line code explanation
- **`documents/README.md`** - Detailed README

## Hardware

- **Board:** FRDM-K66F
- **Buttons:** SW2 (PTD11), SW3 (PTA10)
- **LEDs:** Onboard Green (PTE6), Red (PTC9)
- **Serial:** UART0, 115200 baud

## Peripherals Implemented

1. **UART** - Serial communication (keyboard input + console output)
2. **GPIO** - Buttons and LEDs
3. **PIT Timer** - LED flickering (500ms intervals)

## Project Structure

```
SEH500_Project/
├── source/              # Source code (C and assembly)
├── documents/           # All documentation
├── backup/             # Reference code
├── drivers/            # SDK drivers
├── fatfs/              # FatFS filesystem (not used in final implementation)
└── board/              # Board configuration
```

## Features

- **Interrupt-driven GPIO** - Button presses trigger hardware interrupts
- **State machine** - Three states (IDLE, WATER_ALERT, WASHROOM_ALERT)
- **Assembly language** - Direct hardware register manipulation for LED control
- **Bidirectional UART** - Keyboard input and debug output
- **PIT Timer** - Periodic interrupts for LED blinking (500ms intervals)

## Code Overview

- **`source/SEH500_Project.c`** - Main application with state machine and interrupt handlers
- **`source/gpio_led.s`** - Assembly functions for LED control
- **`source/wav_parser.s`** - Assembly functions for WAV file parsing (reference only, not used)

See `documents/CODE_EXPLANATION.md` for detailed line-by-line explanations of all code.
