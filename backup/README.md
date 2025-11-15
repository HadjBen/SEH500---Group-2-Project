# Backup Folder - Reference Code

## Contents

### `SEH500_Project_audio_attempt.c`
Previous attempt at implementing audio playback. This file contains:

**What's Useful:**
- SD card initialization code (`init_sd_card()`)
- Audio hardware initialization (`init_audio_hardware()`)
- SAI/DMA configuration
- Audio streaming setup

**Note:** This file uses embedded audio data (water_audio.h, restroom_audio.h) which is NOT the approach we're using. We're using SD card file reading instead.

**Why It Was Backed Up:**
- Audio playback had issues (hardware initialization problems)
- Used embedded audio approach (not SD card file reading)
- Reverted to simpler version with just LEDs and buttons
- Kept as reference for audio hardware initialization only

**Important:** Do NOT use the embedded audio approach from this file. Use SD card file reading as described in `documents/PARTNER_HANDOFF.md`.

**How to Use:**
- Reference the SD card initialization code
- Reference the audio hardware setup
- Integrate working parts into `source/SEH500_Project.c`
- Don't copy directly - adapt to current working code structure

**Key Functions to Extract:**
1. `init_sd_card()` - SD card initialization
2. `init_audio_hardware()` - SAI/I2C/DMA setup
3. `play_audio()` - Audio playback logic
4. `stream_audio_samples()` - DMA audio streaming

**Note:** This file is NOT included in the build (moved to backup to avoid linker errors).

