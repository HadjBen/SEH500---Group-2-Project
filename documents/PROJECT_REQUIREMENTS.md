# SEH500 Project Requirements - Assistive Audio-Visual Communicator

## Project Overview
Assistive communication device for non-verbal patients to communicate basic needs (water, washroom) to caregivers.

## Functionality

### Patient Button Press (Alert Trigger)
1. **Patient presses button** → Triggers alert system
   - **Button 1 (Water)** → Green LED flashes + `water.wav` plays
   - **Button 2 (Washroom)** → Red LED flashes + `restroom.wav` plays
2. **LED behavior** → Color-coded LED flashes continuously
   - Water → Green LED
   - Washroom → Red LED
3. **Audio playback** → Pre-recorded WAV file plays from SD card
   - Repeats every 10 seconds
   - Plays from SD card (not memory)
4. **Continuous alert** → LEDs flash and audio repeats until cancelled

### Caretaker Cancellation
1. **Caretaker presses SAME button** → Cancels that specific alert
2. **Stops audio playback** → Audio stops immediately
3. **Turns off LED** → Associated LED turns off
4. **System returns to idle** → Ready for next request

## Technical Requirements

### Course Requirements
- ✅ **100+ lines of GNU Assembler** → WAV file header reading/parsing (wav_parser.s)
- ✅ **One type of interrupt** → GPIO interrupts for buttons (SW2, SW3)
- ✅ **Three+ peripherals:**
  1. ✅ **Serial Communication (UART)** → Bidirectional PC communication (keyboard + console)
  2. ✅ **Onboard LED** → Status indicator (Green/Red LEDs)
  3. ✅ **General Purpose I/O** → Buttons (SW2, SW3) and LED control
  4. 🎵 **SD Card** → Audio file storage (TODO: Audio playback)
  5. 🎵 **Audio (SAI)** → Audio playback (TODO: Integration)

### Peripherals Needed

#### GPIO & Interrupts
- **External Button 1 (Water)** → GPIO interrupt
- **External Button 2 (Washroom)** → GPIO interrupt  
- **External Green LED** → GPIO output (water indicator)
- **External Red LED** → GPIO output (washroom indicator)
- **Onboard RGB LED** → GPIO output (status)

#### Timer
- **PIT Timer** → 10-second intervals for audio playback repetition
- Similar to Lab 7's timer but 10-second intervals

#### Audio Playback
- **SD Card** → Stores WAV files (`water.wav`, `restroom.wav`)
- **DAC or SAI** → Audio output (proposal mentions DAC, but SAI drivers are in project)
- **Assembly code** → 100+ lines for WAV file header parsing from SD card

#### Serial Communication
- **UART** → Debug/logging output

### State Machine
- **IDLE** → Waiting for patient button press
- **WATER_ALERT** → Green LED flashing, water.wav playing every 10s
- **WASHROOM_ALERT** → Red LED flashing, restroom.wav playing every 10s
- **CANCELLED** → Return to IDLE

## Implementation Plan

### Phase 1: GPIO Setup ✅ COMPLETE
- ✅ Assembly functions for LED control (`gpio_led.s`)
- ✅ GPIO interrupt setup for buttons (SW2, SW3)
- ✅ Onboard LED configuration (Green, Red)

### Phase 2: Timer Setup ✅ COMPLETE
- ✅ PIT timer for LED flickering (500ms intervals)
- ✅ Timer interrupt handler (`PIT0_IRQHandler`)
- 🎵 TODO: Extend to 10-second intervals for audio playback

### Phase 3: SD Card & Audio 🎵 TODO
- 🎵 SD card initialization (reference code in `backup/`)
- ✅ **Assembly code (100+ lines)** → WAV file header parsing (`wav_parser.s`)
- 🎵 Audio playback setup (SAI + DMA integration)

### Phase 4: State Machine ✅ COMPLETE
- ✅ Button interrupt handlers (`PORTD_IRQHandler`, `PORTA_IRQHandler`)
- ✅ State management (`handle_water_alert`, `handle_washroom_alert`)
- ✅ LED control logic
- 🎵 TODO: Audio control logic integration

### Phase 5: UART Logging ✅ COMPLETE
- ✅ Serial communication setup
- ✅ Status message logging
- ✅ Keyboard input ('W', 'T' commands)