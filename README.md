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
🎵 **TODO:** SD card audio playback (see `documents/PARTNER_HANDOFF.md`)

## Documentation

All documentation is in the `documents/` folder:

- **`documents/VSCODE_SETUP_GUIDE.md`** - **START HERE** - Complete VS Code setup and workflow guide
- **`documents/PARTNER_HANDOFF.md`** - Audio implementation guide (AI-friendly)
- **`documents/COLLABORATION_GUIDE.md`** - Git workflow guide (command line)
- **`documents/PROJECT_REPORT.md`** - Final technical report (for submission)
- **`documents/PROJECT_REQUIREMENTS.md`** - Full project requirements
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
├── source/              # Source code
├── documents/           # All documentation
├── audio/              # WAV files for SD card
├── backup/             # Reference code
├── drivers/            # SDK drivers
├── fatfs/              # FatFS filesystem
└── board/              # Board configuration
```

## Next Steps

See `documents/PARTNER_HANDOFF.md` for complete SD card audio playback implementation guide.
