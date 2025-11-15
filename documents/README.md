# SEH500 Project - Assistive Audio-Visual Communicator

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

✅ **Working:** Buttons, LEDs, UART keyboard control, state machine  
🎵 **TODO:** SD card audio playback (see `PARTNER_HANDOFF.md`)

## Documentation

- **`PARTNER_HANDOFF.md`** - **START HERE** - Complete handoff guide for audio implementation (AI-friendly)
- **`PROJECT_REPORT.md`** - Final technical report (for submission)
- **`PROJECT_REQUIREMENTS.md`** - Full project requirements

## Hardware

- **Board:** FRDM-K66F
- **Buttons:** SW2 (PTD11), SW3 (PTA10)
- **LEDs:** Onboard Green (PTE6), Red (PTC9)
- **Serial:** UART0, 115200 baud

## Peripherals Implemented

1. **UART** - Serial communication (keyboard input + console output)
2. **GPIO** - Buttons and LEDs
3. **PIT Timer** - LED flickering (500ms intervals)

## Next Steps

See `PARTNER_HANDOFF.md` for complete SD card audio playback implementation guide. This document is designed to be uploaded to AI assistants for help with implementation.

