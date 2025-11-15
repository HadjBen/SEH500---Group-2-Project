# Partner Handoff Document: SD Card Audio Implementation

## Context for AI Assistant

This document provides complete context for implementing SD card audio playback functionality in an embedded system project. When providing assistance, reference this document for project structure, existing code, and implementation requirements.

---

## Project Overview

**Project Name:** SEH500 Assistive Audio-Visual Communicator  
**Platform:** FRDM-K66F Development Board (NXP MK66FN2M0VMD18 microcontroller)  
**IDE:** MCUXpresso IDE  
**Current Status:** Buttons, LEDs, and UART communication are fully functional. SD card audio playback needs to be implemented.

**Objective:** Add audio playback functionality that plays WAV files from an SD card when buttons are pressed. Audio should repeat every 10 seconds while an alert is active.

---

## Current System State

### What's Working ✅

1. **GPIO Buttons:** SW2 (PTD11) triggers water alert, SW3 (PTA10) triggers washroom alert
2. **LED Control:** Green LED (PTE6) flickers for water alert, Red LED (PTC9) flickers for washroom alert
3. **UART Communication:** Bidirectional serial communication at 115200 baud
   - Keyboard input: 'W' for water, 'T' for washroom
   - Console output for debugging
4. **State Machine:** Three states (IDLE, WATER_ALERT, WASHROOM_ALERT) with proper transitions
5. **PIT Timer:** 500ms interrupts for LED flickering
6. **Assembly Code:** 100+ lines in `gpio_led.s` and `wav_parser.s`

### What Needs Implementation 🎵

**SD Card Audio Playback:**
- Initialize SD card and mount FatFS filesystem
- Read WAV files (`water.wav`, `restroom.wav`) from SD card
- Initialize audio hardware (SAI, DMA, codec)
- Stream audio data from SD card to audio output
- Trigger audio playback when alerts activate
- Repeat audio every 10 seconds during active alerts
- Stop audio when alerts are cancelled

---

## Project Structure

```
SEH500_Project/
├── source/
│   ├── SEH500_Project.c          ← Main application (WORKING - modify this)
│   ├── gpio_led.s                ← LED control assembly (complete)
│   ├── wav_parser.s              ← WAV parsing assembly (complete, ready to use)
│   └── ffconf.h                  ← FatFS config (SD_DISK_ENABLE already set)
│
├── audio/
│   ├── water.wav                 ← Audio file (copy to SD card root)
│   └── restroom.wav              ← Audio file (copy to SD card root)
│
├── backup/
│   └── SEH500_Project_audio_attempt.c  ← Reference code with SD/audio init
│
├── fatfs/source/fsl_sd_disk/    ← SD card FatFS driver (already included)
├── drivers/                      ← All SDK drivers (fsl_sai, fsl_sdhc, fsl_edma, etc.)
├── sdmmc/                        ← SD/MMC host driver
└── board/                        ← Board configuration files
```

---

## Key Code Locations

### Main Application File
**File:** `source/SEH500_Project.c`

**Key Functions:**
- `main()` - System initialization (lines 51-108)
- `handle_water_alert()` - Water alert state management (lines 112-133)
- `handle_washroom_alert()` - Washroom alert state management (lines 144-165)
- `PIT0_IRQHandler()` - Timer interrupt (500ms, currently handles LED flicker) (lines 176-223)
- `setup_button_interrupts()` - GPIO interrupt configuration (lines 228-275)

**Current State Variables:**
- `current_state` - System state (IDLE, WATER_ALERT, WASHROOM_ALERT)
- `led_blink_state` - LED on/off state
- `timer_tick_count` - Counter for 10-second audio repeat (needs to be added)

### Assembly Functions Available
**File:** `source/wav_parser.s`

**Functions:**
- `parse_wav_header(buffer_addr, wav_info_addr)` - Parses WAV header, returns 0 if valid
- `calculate_audio_duration(wav_info_addr)` - Returns duration in milliseconds
- `validate_wav_format(wav_info_addr, sample_rate, channels)` - Validates format compatibility

**Usage:** These functions are already compiled and linked. Call them from C code after reading WAV header from SD card.

### Reference Implementation
**File:** `backup/SEH500_Project_audio_attempt.c`

**Contains:**
- SD card initialization code (`init_sd_card()` function)
- Audio hardware initialization (`init_audio_hardware()` function)
- SAI/DMA configuration
- Audio playback structure
- WAV file reading code

**Note:** This file had issues and was backed up. Use it as reference for initialization sequences, but integrate into the working `SEH500_Project.c` file.

---

## Implementation Steps

### Step 1: Add Required Headers and Includes

In `source/SEH500_Project.c`, add these includes at the top:

```c
#include "ff.h"                    // FatFS filesystem
#include "diskio.h"                // FatFS disk I/O
#include "fsl_sd_disk.h"           // SD card disk driver
#include "fsl_sai.h"               // Serial Audio Interface
#include "fsl_sai_edma.h"          // SAI with DMA
#include "fsl_i2c.h"               // I2C for audio codec
#include "fsl_dmamux.h"            // DMA multiplexer
#include "fsl_edma.h"              // Enhanced DMA
#include "fsl_device_registers.h"  // Device register definitions
#include "fsl_sdhc.h"              // SD Host Controller
```

### Step 2: Add Global Variables

Add these global variables after the existing state variables:

```c
// Audio playback variables
static FATFS g_fileSystem;         // FatFS filesystem object
static FIL g_audioFile;            // File handle for audio file
static bool g_audioPlaying = false;
static uint32_t g_audioTimerCount = 0;  // Counter for 10-second repeat
static wav_info_t g_wavInfo;       // WAV file information structure

// Audio hardware handles
static sai_edma_handle_t g_saiTxHandle;
static edma_handle_t g_dmaHandle;
static sai_transfer_t g_saiTransfer;

// Define WAV info structure (if not already defined)
typedef struct {
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    uint32_t dataSize;
    uint32_t dataOffset;
} wav_info_t;
```

### Step 3: Implement SD Card Initialization

Create a function to initialize the SD card. Reference `backup/SEH500_Project_audio_attempt.c` for the exact initialization sequence. Key steps:

1. Enable SDHC clock: `CLOCK_EnableClock(kCLOCK_Sdhc0);`
2. Initialize SDHC peripheral
3. Detect card presence (GPIO polling or SDHC detection)
4. Initialize SD card using `SD_Init()` from `fsl_sd.h`
5. Mount FatFS filesystem: `f_mount(&g_fileSystem, "", 1)`

**Function signature:**
```c
static status_t init_sd_card(void);
```

### Step 4: Implement Audio Hardware Initialization

Create a function to initialize audio hardware. Reference `backup/SEH500_Project_audio_attempt.c`. Key components:

1. **I2C Initialization:** Configure I2C for audio codec communication
2. **Codec Configuration:** Write registers to configure codec (DA7212 or board-specific codec)
3. **SAI Configuration:** Configure Serial Audio Interface for I2S protocol
4. **DMA Configuration:** Set up DMA channels for audio data transfer
5. **DMAMUX Configuration:** Route DMA requests to SAI

**Function signature:**
```c
static status_t init_audio_hardware(void);
```

**Important:** Check board documentation for codec type and I2C address. FRDM-K66F may use different codec than reference code.

### Step 5: Implement WAV File Reading Function

Create a function to read and parse WAV file header:

```c
static status_t read_wav_file(const char* filename, wav_info_t* wavInfo) {
    FRESULT result;
    UINT bytesRead;
    uint8_t headerBuffer[44];  // Standard WAV header is 44 bytes
    
    // Open file
    result = f_open(&g_audioFile, filename, FA_READ);
    if (result != FR_OK) {
        PRINTF("Error opening file: %s (code: %d)\r\n", filename, result);
        return kStatus_Fail;
    }
    
    // Read header
    result = f_read(&g_audioFile, headerBuffer, 44, &bytesRead);
    if (result != FR_OK || bytesRead != 44) {
        PRINTF("Error reading header\r\n");
        f_close(&g_audioFile);
        return kStatus_Fail;
    }
    
    // Parse header using assembly function
    if (parse_wav_header(headerBuffer, wavInfo) != 0) {
        PRINTF("Invalid WAV file format\r\n");
        f_close(&g_audioFile);
        return kStatus_Fail;
    }
    
    // Seek to data start (usually offset 44)
    f_lseek(&g_audioFile, wavInfo->dataOffset);
    
    PRINTF("WAV file loaded: %d Hz, %d channels, %d bits\r\n", 
           wavInfo->sampleRate, wavInfo->numChannels, wavInfo->bitsPerSample);
    
    return kStatus_Success;
}
```

### Step 6: Implement Audio Playback Function

Create a function to play audio from the currently open file:

```c
static status_t play_audio_stream(void) {
    FRESULT result;
    UINT bytesRead;
    uint8_t audioBuffer[4096];  // 4KB buffer for audio data
    sai_config_t saiConfig;
    sai_transfer_format_t format;
    
    if (g_audioPlaying) {
        return kStatus_Busy;  // Already playing
    }
    
    // Configure SAI based on WAV file format
    SAI_TxGetDefaultConfig(&saiConfig);
    saiConfig.syncMode = kSAI_ModeAsync;
    saiConfig.masterSlave = kSAI_Master;
    
    // Set audio format
    format.bitWidth = g_wavInfo.bitsPerSample;
    format.channel = (g_wavInfo.numChannels == 1) ? kSAI_MonoLeft : kSAI_Stereo;
    format.sampleRate_Hz = g_wavInfo.sampleRate;
    format.masterClockHz = g_wavInfo.sampleRate * 256;  // Typical for I2S
    
    SAI_TxSetFormat(SAI, &format, kSAI_TxBitClockPolarityActiveHigh, kSAI_TxFrameSyncPolarityActiveLow);
    
    // Read audio data from SD card
    result = f_read(&g_audioFile, audioBuffer, sizeof(audioBuffer), &bytesRead);
    if (result != FR_OK || bytesRead == 0) {
        return kStatus_Fail;
    }
    
    // Configure DMA transfer
    g_saiTransfer.data = audioBuffer;
    g_saiTransfer.dataSize = bytesRead;
    
    // Start DMA transfer
    SAI_TransferSendEDMA(SAI, &g_saiTxHandle, &g_saiTransfer);
    
    g_audioPlaying = true;
    return kStatus_Success;
}
```

### Step 7: Integrate Audio into State Handlers

Modify `handle_water_alert()` function:

```c
static void handle_water_alert(void) {
    if (current_state == STATE_WATER_ALERT) {
        // Cancel water alert
        current_state = STATE_IDLE;
        func_green_led_off();
        func_red_led_off();
        led_blink_state = 0;
        PIT_StopTimer(PIT, kPIT_Chnl_0);
        g_audioTimerCount = 0;
        
        // Stop audio
        if (g_audioPlaying) {
            SAI_TransferAbortEDMA(SAI, &g_saiTxHandle);
            f_close(&g_audioFile);
            g_audioPlaying = false;
        }
        
        PRINTF("Water alert cancelled\r\n");
    } else {
        // Start water alert
        if (current_state == STATE_WASHROOM_ALERT) {
            func_red_led_off();
            // Stop any playing audio
            if (g_audioPlaying) {
                SAI_TransferAbortEDMA(SAI, &g_saiTxHandle);
                f_close(&g_audioFile);
                g_audioPlaying = false;
            }
        }
        
        current_state = STATE_WATER_ALERT;
        func_green_led_on();
        led_blink_state = 1;
        PIT_StartTimer(PIT, kPIT_Chnl_0);
        g_audioTimerCount = 0;
        
        // Start audio playback
        if (read_wav_file("water.wav", &g_wavInfo) == kStatus_Success) {
            play_audio_stream();
        }
        
        PRINTF("Water alert started\r\n");
    }
}
```

Do the same for `handle_washroom_alert()` but use `"restroom.wav"` instead.

### Step 8: Add 10-Second Audio Repeat to Timer Handler

Modify `PIT0_IRQHandler()` to include audio repeat logic:

```c
void PIT0_IRQHandler(void) {
    PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
    
    // LED flickering (existing code)
    if (current_state == STATE_WATER_ALERT) {
        // Toggle LED
        if (led_blink_state == 0) {
            func_green_led_on();
            led_blink_state = 1;
        } else {
            func_green_led_off();
            led_blink_state = 0;
        }
        
        // Check for 10-second audio repeat (20 ticks * 500ms = 10 seconds)
        g_audioTimerCount++;
        if (g_audioTimerCount >= 20) {
            g_audioTimerCount = 0;
            // Restart audio playback
            f_lseek(&g_audioFile, g_wavInfo.dataOffset);  // Seek to audio data start
            play_audio_stream();
        }
    } else if (current_state == STATE_WASHROOM_ALERT) {
        // Similar code for washroom alert
        // ... (same pattern)
    }
}
```

### Step 9: Add Initialization Calls in main()

In the `main()` function, after existing initialization, add:

```c
// Initialize SD card
if (init_sd_card() != kStatus_Success) {
    PRINTF("ERROR: SD card initialization failed\r\n");
    // Continue anyway - system can work without audio
}

// Initialize audio hardware
if (init_audio_hardware() != kStatus_Success) {
    PRINTF("ERROR: Audio hardware initialization failed\r\n");
    // Continue anyway - LEDs will still work
} else {
    PRINTF("Audio hardware initialized\r\n");
}
```

### Step 10: Handle DMA Transfer Complete

Add a DMA callback function to handle when audio transfer completes:

```c
static void audio_transfer_complete_callback(I2S_Type *base, sai_edma_handle_t *handle, 
                                             status_t status, void *userData) {
    if (status == kStatus_SAI_TxIdle) {
        // Transfer complete - read next chunk or finish
        FRESULT result;
        UINT bytesRead;
        uint8_t audioBuffer[4096];
        
        // Read next chunk
        result = f_read(&g_audioFile, audioBuffer, sizeof(audioBuffer), &bytesRead);
        if (result == FR_OK && bytesRead > 0) {
            g_saiTransfer.data = audioBuffer;
            g_saiTransfer.dataSize = bytesRead;
            SAI_TransferSendEDMA(SAI, &g_saiTxHandle, &g_saiTransfer);
        } else {
            // End of file - playback complete
            g_audioPlaying = false;
            f_close(&g_audioFile);
        }
    }
}
```

Register this callback during audio hardware initialization.

---

## Technical Details

### SD Card Requirements
- Format: FAT32
- Files must be in root directory: `water.wav` and `restroom.wav`
- WAV format: PCM, 16-bit, mono or stereo, sample rate 8kHz-48kHz

### Audio Hardware
- **SAI Peripheral:** Serial Audio Interface for I2S protocol
- **DMA:** Enhanced DMA for data transfer
- **Codec:** Board-specific audio codec (check board documentation)
- **I2C:** Used for codec configuration

### Memory Considerations
- Audio buffer size: 4KB recommended (adjust based on available RAM)
- WAV header: 44 bytes (read once per file)
- FatFS buffers: Managed by filesystem driver

### Timing
- PIT timer: 500ms intervals
- Audio repeat: Every 20 ticks = 10 seconds
- LED flicker: Every 500ms (1 tick)

---

## Common Issues and Solutions

### Issue: SD Card Not Detected
**Solution:** 
- Check card detection GPIO pin configuration
- Verify card is properly inserted
- Check SDHC clock enable
- Try different SD card (some cards may not be compatible)

### Issue: Audio Not Playing
**Solution:**
- Verify codec I2C address and register values
- Check SAI clock configuration matches sample rate
- Verify DMA channel routing in DMAMUX
- Check audio buffer alignment (may need to be 32-byte aligned)

### Issue: Audio Cuts Out
**Solution:**
- Increase audio buffer size
- Check DMA transfer completion callback
- Verify continuous data reading from SD card
- Check for buffer underruns

### Issue: Wrong Playback Speed
**Solution:**
- Verify SAI sample rate matches WAV file sample rate
- Check master clock calculation
- Ensure correct bits per sample configuration

---

## Testing Checklist

- [ ] SD card initializes and mounts successfully
- [ ] WAV files can be opened and read
- [ ] WAV header parsing works correctly
- [ ] Audio hardware initializes without errors
- [ ] Audio plays when button pressed
- [ ] Audio stops when button pressed again (cancellation)
- [ ] Audio repeats every 10 seconds during alert
- [ ] No audio conflicts when switching between alerts
- [ ] System works correctly without SD card (graceful degradation)
- [ ] LED flickering continues to work during audio playback

---

## Integration Notes

**Important:** The current `SEH500_Project.c` file is working perfectly for buttons, LEDs, and UART. When adding audio functionality:

1. **Preserve existing functionality** - Don't break what's working
2. **Add audio code incrementally** - Test each step before proceeding
3. **Use reference code** - `backup/SEH500_Project_audio_attempt.c` has working initialization sequences
4. **Handle errors gracefully** - System should work even if audio fails (LEDs still function)

**Code Organization:**
- Keep state management functions (`handle_water_alert`, etc.) clean
- Add audio functions as separate, well-commented functions
- Use clear variable names and comments

---

## Resources

- **SDK Documentation:** NXP SDK documentation for fsl_sai, fsl_sdhc, fsl_edma
- **FatFS Documentation:** FatFS website for filesystem API
- **Board Manual:** FRDM-K66F User Guide for pin assignments and codec information
- **Reference Code:** `backup/SEH500_Project_audio_attempt.c` for initialization sequences

---

## Success Criteria

The implementation is complete when:
1. Pressing SW2 or typing 'W' plays `water.wav` and green LED flickers
2. Pressing SW3 or typing 'T' plays `restroom.wav` and red LED flickers
3. Audio repeats every 10 seconds while alert is active
4. Pressing the same button/key again stops audio and LED flicker
5. System handles missing SD card or audio files gracefully
6. All existing functionality (buttons, LEDs, UART) continues to work

---

*This document provides complete context for implementing SD card audio playback. Use it as a reference when working with AI assistants or debugging the implementation.*

