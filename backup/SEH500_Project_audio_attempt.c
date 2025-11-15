/*
 * SEH500 Project - Assistive Audio-Visual Communicator
 * AUDIO ATTEMPT VERSION - Saved state before reverting
 * For non-verbal patients to communicate needs (water, washroom)
 */

#include <stdio.h>
#include <string.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_pit.h"
#include "fsl_clock.h"
#include "fsl_common.h"
#include "fsl_sai.h"
#include "fsl_sai_edma.h"
#include "fsl_i2c.h"
#include "fsl_dmamux.h"
#include "fsl_edma.h"
#include "fsl_device_registers.h"

// External assembly function prototypes
void setup_leds(void);
void func_green_led_on(void);
void func_green_led_off(void);
void func_red_led_on(void);
void func_red_led_off(void);

// WAV header structure (must be defined before forward declarations)
typedef struct {
    char riff[4];           // "RIFF"
    uint32_t chunkSize;     // File size - 8
    char wave[4];           // "WAVE"
    char fmt[4];            // "fmt "
    uint32_t fmtSize;       // Format chunk size (usually 16)
    uint16_t audioFormat;   // Audio format (1 = PCM)
    uint16_t numChannels;   // Number of channels
    uint32_t sampleRate;    // Sample rate
    uint32_t byteRate;      // Byte rate
    uint16_t blockAlign;    // Block align
    uint16_t bitsPerSample; // Bits per sample
    char data[4];           // "data"
    uint32_t dataSize;      // Data chunk size
} wav_header_t;

// Forward declarations
static void setup_button_interrupts(void);
static void play_audio(const char* audio_name);
static int init_audio_hardware(void);
static void stream_audio_samples(const uint8_t* samples, uint32_t num_samples, const wav_header_t* header);

// System states
typedef enum {
    STATE_IDLE = 0,
    STATE_WATER_ALERT,
    STATE_WASHROOM_ALERT
} system_state_t;

// Global variables
volatile static system_state_t current_state = STATE_IDLE;
volatile static int led_blink_state = 0;  // 0 = OFF, 1 = ON
volatile static int timer_tick_count = 0; // Count timer ticks for 10-second audio interval

// Audio hardware globals
static sai_edma_handle_t s_saiTxHandle;
static edma_handle_t s_dmaHandle;
static bool s_audioPlaying = false;

// Button pin definitions - Using onboard SW2 and SW3
// SW2 is on PTD11 (Port D, pin 11) - Water button
// SW3 is on PTA10 (Port A, pin 10) - Washroom button
#define WATER_BUTTON_PORT      GPIOD
#define WATER_BUTTON_PIN       11U
#define WASHROOM_BUTTON_PORT   GPIOA
#define WASHROOM_BUTTON_PIN    10U

int main(void) {
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    BOARD_InitDebugConsole();

    PRINTF("=== Assistive Audio-Visual Communicator ===\r\n");
    PRINTF("Initializing system...\r\n");

    // Setup onboard RGB LEDs using assembly functions
    setup_leds();
    func_green_led_off();
    func_red_led_off();
    PRINTF("Onboard LEDs initialized (Green=Water, Red=Washroom)\r\n");

    // Initialize PIT timer for LED flicker (500ms intervals)
    pit_config_t pitConfig;
    PIT_GetDefaultConfig(&pitConfig);
    PIT_Init(PIT, &pitConfig);
    PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, USEC_TO_COUNT(500000U, CLOCK_GetFreq(kCLOCK_BusClk))); // 500ms
    PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);
    EnableIRQ(PIT0_IRQn);
    PRINTF("Timer initialized (500ms LED flicker, audio every 10s)\r\n");

    // Initialize audio hardware
    if (init_audio_hardware() == 0) {
        PRINTF("Audio hardware initialized successfully\r\n");
    } else {
        PRINTF("WARNING: Audio hardware initialization failed - audio may not play\r\n");
    }

    // Setup GPIO interrupts for buttons
    setup_button_interrupts();
    PRINTF("Button interrupts configured\r\n");

    PRINTF("System ready.\r\n");
    PRINTF("SW2 - Toggle water alert (Green LED flicker + audio)\r\n");
    PRINTF("SW3 - Toggle washroom alert (Red LED flicker + audio)\r\n");
    PRINTF("Connect headphones/speaker to 3.5mm jack (J28) to hear audio\r\n");

    while(1) {
        __asm volatile ("nop");
    }
    return 0;
}

// SW2 button interrupt handler (PTD11) - Water button
// Handler name must match the interrupt vector table: PORTD_IRQHandler
void PORTD_IRQHandler(void) {
    GPIO_PortClearInterruptFlags(WATER_BUTTON_PORT, 1U << WATER_BUTTON_PIN);
    
    PRINTF("[BUTTON PRESS] SW2 pressed!\r\n");
    
    if (current_state == STATE_WATER_ALERT) {
        // Cancel water alert (same button pressed again)
        current_state = STATE_IDLE;
        func_green_led_off();
        func_red_led_off();  // Ensure red is also off
        led_blink_state = 0;
        PIT_StopTimer(PIT, kPIT_Chnl_0);
        PRINTF("SW2 - Water alert cancelled (LED flicker OFF)\r\n");
    } else {
        // Start water alert (cancel any other alert first)
        if (current_state == STATE_WASHROOM_ALERT) {
            func_red_led_off();
            PRINTF("SW2 - Cancelled washroom alert, starting water alert\r\n");
        }
        current_state = STATE_WATER_ALERT;
        func_green_led_on();
        led_blink_state = 1;
        timer_tick_count = 0;  // Reset audio timer
        PIT_StartTimer(PIT, kPIT_Chnl_0);
        PRINTF("SW2 - Water alert started (Green LED flicker ON)\r\n");
    }
}

// SW3 button interrupt handler (PTA10) - Washroom button
// Handler name must match the interrupt vector table: PORTA_IRQHandler
void PORTA_IRQHandler(void) {
    GPIO_PortClearInterruptFlags(WASHROOM_BUTTON_PORT, 1U << WASHROOM_BUTTON_PIN);
    
    PRINTF("[BUTTON PRESS] SW3 pressed!\r\n");
    
    if (current_state == STATE_WASHROOM_ALERT) {
        // Cancel washroom alert (same button pressed again)
        current_state = STATE_IDLE;
        func_red_led_off();
        func_green_led_off();  // Ensure green is also off
        led_blink_state = 0;
        PIT_StopTimer(PIT, kPIT_Chnl_0);
        PRINTF("SW3 - Washroom alert cancelled (LED flicker OFF)\r\n");
    } else {
        // Start washroom alert (cancel any other alert first)
        if (current_state == STATE_WATER_ALERT) {
            func_green_led_off();
            PRINTF("SW3 - Cancelled water alert, starting washroom alert\r\n");
        }
        current_state = STATE_WASHROOM_ALERT;
        func_red_led_on();
        led_blink_state = 1;
        timer_tick_count = 0;  // Reset audio timer
        PIT_StartTimer(PIT, kPIT_Chnl_0);
        PRINTF("SW3 - Washroom alert started (Red LED flicker ON)\r\n");
    }
}

// PIT Timer interrupt handler - triggers every 500ms for LED flicker
// Audio plays every 10 seconds (20 ticks of 500ms each)
void PIT0_IRQHandler(void) {
    PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
    
    // Toggle LED blink state (every 500ms)
    if (current_state == STATE_WATER_ALERT) {
        if (led_blink_state == 0) {
            func_green_led_on();
            led_blink_state = 1;
        } else {
            func_green_led_off();
            led_blink_state = 0;
        }
        
        // Check if 10 seconds have passed (20 ticks of 500ms each)
        timer_tick_count++;
        if (timer_tick_count >= 20) {
            timer_tick_count = 0;
            play_audio("water.wav");
        }
    } else if (current_state == STATE_WASHROOM_ALERT) {
        if (led_blink_state == 0) {
            func_red_led_on();
            led_blink_state = 1;
        } else {
            func_red_led_off();
            led_blink_state = 0;
        }
        
        // Check if 10 seconds have passed (20 ticks of 500ms each)
        timer_tick_count++;
        if (timer_tick_count >= 20) {
            timer_tick_count = 0;
            play_audio("restroom.wav");
        }
    }
}

// Setup button GPIO interrupts
// Manually configure pins since ConfigTools wasn't used
static void setup_button_interrupts(void) {
    port_pin_config_t portConfig;
    gpio_pin_config_t gpioConfig;
    
    // Configure SW2 (PTD11) - Water button
    // Enable clock for PORTD
    CLOCK_EnableClock(kCLOCK_PortD);
    
    // Configure pin as GPIO with pull-up and interrupt on falling edge
    portConfig.pullSelect = kPORT_PullUp;
    portConfig.slewRate = kPORT_FastSlewRate;
    portConfig.passiveFilterEnable = kPORT_PassiveFilterDisable;
    portConfig.openDrainEnable = kPORT_OpenDrainDisable;
    portConfig.driveStrength = kPORT_LowDriveStrength;
    portConfig.mux = kPORT_MuxAsGpio;
    portConfig.lockRegister = kPORT_UnlockRegister;
    PORT_SetPinConfig(PORTD, WATER_BUTTON_PIN, &portConfig);
    PORT_SetPinInterruptConfig(PORTD, WATER_BUTTON_PIN, kPORT_InterruptFallingEdge);
    
    // Configure GPIO as input
    gpioConfig.pinDirection = kGPIO_DigitalInput;
    GPIO_PinInit(WATER_BUTTON_PORT, WATER_BUTTON_PIN, &gpioConfig);
    
    // Configure SW3 (PTA10) - Washroom button
    // Enable clock for PORTA
    CLOCK_EnableClock(kCLOCK_PortA);
    
    // Configure pin as GPIO with pull-up and interrupt on falling edge
    portConfig.pullSelect = kPORT_PullUp;
    portConfig.slewRate = kPORT_FastSlewRate;
    portConfig.passiveFilterEnable = kPORT_PassiveFilterDisable;
    portConfig.openDrainEnable = kPORT_OpenDrainDisable;
    portConfig.driveStrength = kPORT_LowDriveStrength;
    portConfig.mux = kPORT_MuxAsGpio;
    portConfig.lockRegister = kPORT_UnlockRegister;
    PORT_SetPinConfig(PORTA, WASHROOM_BUTTON_PIN, &portConfig);
    PORT_SetPinInterruptConfig(PORTA, WASHROOM_BUTTON_PIN, kPORT_InterruptFallingEdge);
    
    // Configure GPIO as input
    gpioConfig.pinDirection = kGPIO_DigitalInput;
    GPIO_PinInit(WASHROOM_BUTTON_PORT, WASHROOM_BUTTON_PIN, &gpioConfig);
    
    // Enable NVIC interrupts
    EnableIRQ(PORTD_IRQn);
    EnableIRQ(PORTA_IRQn);
    
    PRINTF("Button interrupts configured: SW2(PTD11), SW3(PTA10)\r\n");
}

// Include embedded audio data
#include "water_audio.h"
#include "restroom_audio.h"

// Audio data structures
typedef struct {
    const uint8_t* data;      // Pointer to audio data
    uint32_t size;            // Size in bytes
    const char* name;         // Audio file name
} audio_clip_t;

// Audio clips using embedded data
static const audio_clip_t water_clip = {
    .data = water_audio_data,
    .size = water_audio_data_size,
    .name = "water.wav"
};

static const audio_clip_t restroom_clip = {
    .data = restroom_audio_data,
    .size = restroom_audio_data_size,
    .name = "restroom.wav"
};

// WAV header structure already defined above

// Play audio file
static void play_audio(const char* audio_name) {
    const audio_clip_t* clip = NULL;
    
    // Select the appropriate audio clip
    if (strcmp(audio_name, "water.wav") == 0) {
        clip = &water_clip;
    } else if (strcmp(audio_name, "restroom.wav") == 0) {
        clip = &restroom_clip;
    }
    
    if (clip == NULL) {
        PRINTF("[AUDIO] ERROR: Unknown audio file: %s\r\n", audio_name);
        return;
    }
    
    PRINTF("[AUDIO] Playing: %s (%lu bytes)\r\n", clip->name, clip->size);
    
    if (clip->size < 44) {
        PRINTF("[AUDIO] ERROR: File too small to be a valid WAV file\r\n");
        return;
    }
    
    // Parse WAV header
    const wav_header_t* header = (const wav_header_t*)clip->data;
    
    // Verify WAV format
    if (memcmp(header->riff, "RIFF", 4) != 0 || 
        memcmp(header->wave, "WAVE", 4) != 0 ||
        memcmp(header->fmt, "fmt ", 4) != 0) {
        PRINTF("[AUDIO] ERROR: Invalid WAV file format\r\n");
        return;
    }
    
    // Calculate audio duration
    uint32_t duration_ms = (header->dataSize * 1000) / header->byteRate;
    
    PRINTF("[AUDIO] Format: %d-bit, %d channel(s), %lu Hz\r\n", 
           header->bitsPerSample, header->numChannels, header->sampleRate);
    PRINTF("[AUDIO] Duration: %lu ms (%lu samples)\r\n", 
           duration_ms, header->dataSize / (header->bitsPerSample / 8) / header->numChannels);
    PRINTF("[AUDIO] Audio data starts at offset 44, size: %lu bytes\r\n", header->dataSize);
    
    // Verify audio data is present
    const uint8_t* audio_samples = clip->data + 44;  // Skip WAV header
    PRINTF("[AUDIO] First 8 audio bytes: ");
    for (int i = 0; i < 8 && i < (int)header->dataSize; i++) {
        PRINTF("0x%02X ", audio_samples[i]);
    }
    PRINTF("\r\n");
    
    // Stream audio samples to hardware
    PRINTF("[AUDIO] Streaming audio to hardware...\r\n");
    stream_audio_samples(audio_samples, header->dataSize, header);
    PRINTF("[AUDIO] Audio playback complete\r\n");
}

// Audio hardware globals
static bool audio_hw_initialized = false;

// DA7212 codec I2C address
#define DA7212_I2C_ADDR 0x1A

// DA7212 register addresses (simplified - basic audio path)
#define DA7212_REG_SYSTEM_STATUS     0x02
#define DA7212_REG_SYSTEM_ACTIVE     0xFD
#define DA7212_REG_PLL_CTRL          0x25
#define DA7212_REG_DAI_CLK_MODE      0x2B
#define DA7212_REG_DAI_CTRL          0x2C
#define DA7212_REG_DIG_ROUTING_DAI   0x21
#define DA7212_REG_SR                0x22
#define DA7212_REG_DAI_TDM_CTRL      0x2E
#define DA7212_REG_DAC_L             0x5A
#define DA7212_REG_DAC_R             0x5B
#define DA7212_REG_DAC_FILTERS1      0x40
#define DA7212_REG_CP_CTRL           0x47
#define DA7212_REG_HP_L              0x4F
#define DA7212_REG_HP_R              0x50
#define DA7212_REG_MIXOUT_L_SELECT   0x60
#define DA7212_REG_MIXOUT_R_SELECT   0x61

// Initialize audio hardware (I2C for DA7212 codec, SAI for audio streaming)
static int init_audio_hardware(void) {
    PRINTF("[AUDIO HW] Initializing audio hardware...\r\n");
    
    // Initialize I2C for DA7212 codec communication
    // Use I2C driver directly (not conditionally compiled board functions)
    i2c_master_config_t i2cConfig = {0};
    I2C_MasterGetDefaultConfig(&i2cConfig);
    I2C_MasterInit(I2C1, &i2cConfig, CLOCK_GetFreq(kCLOCK_BusClk));
    PRINTF("[AUDIO HW] I2C initialized\r\n");
    
    // Configure DA7212 codec - basic initialization
    i2c_master_transfer_t masterXfer;
    uint8_t data;
    
    // Enable system active
    data = 0x01;
    masterXfer.slaveAddress   = DA7212_I2C_ADDR;
    masterXfer.direction      = kI2C_Write;
    masterXfer.subaddress     = DA7212_REG_SYSTEM_ACTIVE;
    masterXfer.subaddressSize = 1;
    masterXfer.data           = &data;
    masterXfer.dataSize       = 1;
    masterXfer.flags          = kI2C_TransferDefaultFlag;
    I2C_MasterTransferBlocking(I2C1, &masterXfer);
    
    // Configure PLL for 44.1kHz
    data = 0x00; // PLL disabled, use MCLK directly
    masterXfer.subaddress = DA7212_REG_PLL_CTRL;
    I2C_MasterTransferBlocking(I2C1, &masterXfer);
    
    // Configure DAI clock mode (I2S, 44.1kHz)
    data = 0x08; // I2S mode, 44.1kHz
    masterXfer.subaddress = DA7212_REG_DAI_CLK_MODE;
    I2C_MasterTransferBlocking(I2C1, &masterXfer);
    
    // Configure DAI control (16-bit, I2S)
    data = 0x00; // 16-bit, I2S format
    masterXfer.subaddress = DA7212_REG_DAI_CTRL;
    I2C_MasterTransferBlocking(I2C1, &masterXfer);
    
    // Enable DAC
    data = 0x80; // DAC enabled
    masterXfer.subaddress = DA7212_REG_DAC_L;
    I2C_MasterTransferBlocking(I2C1, &masterXfer);
    masterXfer.subaddress = DA7212_REG_DAC_R;
    I2C_MasterTransferBlocking(I2C1, &masterXfer);
    
    // Enable headphone outputs
    data = 0xE0; // HP enabled, volume max
    masterXfer.subaddress = DA7212_REG_HP_L;
    I2C_MasterTransferBlocking(I2C1, &masterXfer);
    masterXfer.subaddress = DA7212_REG_HP_R;
    I2C_MasterTransferBlocking(I2C1, &masterXfer);
    
    // Route DAI to mixout
    data = 0x80; // DAI_L to MIXOUT_L
    masterXfer.subaddress = DA7212_REG_MIXOUT_L_SELECT;
    I2C_MasterTransferBlocking(I2C1, &masterXfer);
    data = 0x80; // DAI_R to MIXOUT_R
    masterXfer.subaddress = DA7212_REG_MIXOUT_R_SELECT;
    I2C_MasterTransferBlocking(I2C1, &masterXfer);
    
    PRINTF("[AUDIO HW] DA7212 codec configured\r\n");
    
    // Initialize SAI - first init with basic config
    sai_config_t saiBasicConfig;
    SAI_TxGetDefaultConfig(&saiBasicConfig);
    saiBasicConfig.syncMode = kSAI_ModeAsync;
    saiBasicConfig.masterSlave = kSAI_Master;
    SAI_TxInit(I2S0, &saiBasicConfig);
    
    // Then set transceiver config for I2S format
    sai_transceiver_t saiTxConfig;
    SAI_GetClassicI2SConfig(&saiTxConfig, kSAI_WordWidth16bits, kSAI_Stereo, kSAI_Channel0Mask | kSAI_Channel1Mask);
    SAI_TxSetConfig(I2S0, &saiTxConfig);
    PRINTF("[AUDIO HW] SAI initialized (I2S, 44.1kHz, 16-bit)\r\n");
    
    // Initialize DMAMUX
    DMAMUX_Init(DMAMUX0);
    
    // Initialize DMA
    edma_config_t dmaConfig;
    EDMA_GetDefaultConfig(&dmaConfig);
    EDMA_Init(DMA0, &dmaConfig);
    
    // Create DMA handle
    EDMA_CreateHandle(&s_dmaHandle, DMA0, 0);
    
    // Install TCD memory pool
    edma_tcd_t tcd[SAI_XFER_QUEUE_SIZE];
    EDMA_InstallTCDMemory(&s_dmaHandle, tcd, SAI_XFER_QUEUE_SIZE);
    
    // Configure DMA mux for I2S0 TX (channel 0)
    DMAMUX_SetSource(DMAMUX0, 0, kDmaRequestMux0I2S0Tx);
    DMAMUX_EnableChannel(DMAMUX0, 0);
    
    // Create SAI TX handle with DMA
    SAI_TransferTxCreateHandleEDMA(I2S0, &s_saiTxHandle, NULL, NULL, &s_dmaHandle);
    
    PRINTF("[AUDIO HW] DMA configured\r\n");
    PRINTF("[AUDIO HW] Audio hardware ready!\r\n");
    
    audio_hw_initialized = true;
    return 0;
}

// Stream audio samples to audio hardware
static void stream_audio_samples(const uint8_t* samples, uint32_t num_samples, const wav_header_t* header) {
    if (!audio_hw_initialized) {
        PRINTF("[AUDIO] ERROR: Audio hardware not initialized\r\n");
        return;
    }
    
    if (s_audioPlaying) {
        PRINTF("[AUDIO] Audio already playing, skipping\r\n");
        return;
    }
    
    uint32_t num_audio_samples = num_samples / (header->bitsPerSample / 8);
    PRINTF("[AUDIO] Streaming %lu samples (%d-bit, %lu Hz, %d channel)...\r\n", 
           num_audio_samples, 
           header->bitsPerSample, 
           header->sampleRate,
           header->numChannels);
    
    // Configure SAI for this audio format
    sai_transceiver_t saiTxConfig;
    sai_word_width_t wordWidth = kSAI_WordWidth16bits;
    if (header->bitsPerSample == 8) wordWidth = kSAI_WordWidth8bits;
    else if (header->bitsPerSample == 16) wordWidth = kSAI_WordWidth16bits;
    else if (header->bitsPerSample == 24) wordWidth = kSAI_WordWidth24bits;
    else if (header->bitsPerSample == 32) wordWidth = kSAI_WordWidth32bits;
    
    sai_mono_stereo_t channels = (header->numChannels == 1) ? kSAI_MonoLeft : kSAI_Stereo;
    uint32_t channelMask = (header->numChannels == 1) ? kSAI_Channel0Mask : (kSAI_Channel0Mask | kSAI_Channel1Mask);
    
    SAI_GetClassicI2SConfig(&saiTxConfig, wordWidth, channels, channelMask);
    SAI_TxSetConfig(I2S0, &saiTxConfig);
    
    // Prepare transfer
    sai_transfer_t xfer;
    xfer.data = (uint8_t*)samples;
    xfer.dataSize = num_samples;
    
    s_audioPlaying = true;
    
    // Start transfer
    status_t status = SAI_TransferSendEDMA(I2S0, &s_saiTxHandle, &xfer);
    if (status != kStatus_Success) {
        PRINTF("[AUDIO] ERROR: Failed to start transfer: %d\r\n", status);
        s_audioPlaying = false;
        return;
    }
    
    PRINTF("[AUDIO] Transfer started, playing audio...\r\n");
    
    // Wait for transfer to complete
    uint32_t transferred = 0;
    while (s_audioPlaying && transferred < num_samples) {
        SAI_TransferGetSendCountEDMA(I2S0, &s_saiTxHandle, &transferred);
        if (transferred >= num_samples) {
            s_audioPlaying = false;
            break;
        }
    }
    
    PRINTF("[AUDIO] Playback complete\r\n");
}

