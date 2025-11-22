# SEH500 Project - Assistive Communication Device

## Quick Start

1. **Open Project:** Open `SEH500_Project` in MCUXpresso IDE
2. **Build:** Project → Build All
3. **Flash:** Flash to FRDM-K66F board
4. **Test:**
   - Press SW2 → Green LED flickers (water alert)
   - Press SW3 → Red LED flickers (washroom alert)
   - Type 'W' in serial terminal → Water alert
   - Type 'T' in serial terminal → Washroom alert
   - Press same button/key again → Cancels alert

## Documentation

- **`SEH500_Project_Report_Revised.md`** - Final technical report 
- **`CODE_EXPLANATION.md`** - Detailed line-by-line code explanation

## Hardware

- **Board:** FRDM-K66F
- **Buttons:** SW2 (PTD11), SW3 (PTA10)
- **LEDs:** Onboard Green (PTE6), Red (PTC9)
- **Serial:** UART0, 115200 baud

## Peripherals Implemented

1. **UART** - Serial communication (keyboard input + console output)
2. **GPIO** - Buttons and LEDs
3. **PIT Timer** - LED flickering (500ms intervals)
