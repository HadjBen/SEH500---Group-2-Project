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
✅ **Fully Interrupt-Driven:** All inputs (buttons and keyboard) use hardware interrupts - no polling

## Documentation

All documentation is in the `documents/` folder:

- **`documents/SEH500_Project_Report.md`** - Final technical report (for submission)
- **`documents/CODE_EXPLANATION.md`** - Detailed line-by-line code explanation

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

- **Fully Interrupt-Driven Architecture** - All inputs use hardware interrupts (no polling)
  - GPIO interrupts for button presses (SW2, SW3)
  - UART interrupts for keyboard input ('W', 'T')
  - PIT timer interrupts for LED blinking (500ms intervals)
- **State machine** - Three states (IDLE, WATER_ALERT, WASHROOM_ALERT)
- **Assembly language** - Direct hardware register manipulation for LED control (100+ lines)
- **Bidirectional UART** - Keyboard input and debug output at 115200 baud
- **Low-Power Design** - Main loop uses `__WFI()` (Wait For Interrupt) for CPU sleep mode

## Code Overview

- **`source/SEH500_Project.c`** - Main application with state machine and interrupt handlers
  - Four interrupt service routines: PORTD (SW2), PORTA (SW3), UART0 (keyboard), PIT0 (timer)
  - Main loop uses `__WFI()` for low-power operation
- **`source/gpio_led.s`** - Assembly functions for LED control (setup and on/off functions)
- **`source/wav_parser.s`** - Assembly functions for WAV file parsing (reference only, not used in final implementation)

See `documents/CODE_EXPLANATION.md` for detailed line-by-line explanations of all code.
