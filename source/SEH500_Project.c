/*
 * SEH500 Project - Assistive Audio-Visual Communicator
 * For non-verbal patients to communicate needs (water, washroom)
 */

#include <stdio.h>
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
#include "fsl_uart.h"

// External assembly function prototypes
void setup_leds(void);
void func_green_led_on(void);
void func_green_led_off(void);
void func_red_led_on(void);
void func_red_led_off(void);

// Forward declarations
static void setup_button_interrupts(void);
static void handle_water_alert(void);
static void handle_washroom_alert(void);

// System states
typedef enum {
    STATE_IDLE = 0,
    STATE_WATER_ALERT,
    STATE_WASHROOM_ALERT
} system_state_t;

// Global variables
volatile static system_state_t current_state = STATE_IDLE;
volatile static int led_blink_state = 0;  // 0 = OFF, 1 = ON

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
    PRINTF("Timer initialized (500ms LED flicker)\r\n");

    // Setup GPIO interrupts for buttons
    setup_button_interrupts();
    PRINTF("Button interrupts configured\r\n");

    PRINTF("System ready.\r\n");
    PRINTF("SW2 - Toggle water alert (Green LED flicker)\r\n");
    PRINTF("SW3 - Toggle washroom alert (Red LED flicker)\r\n");
    PRINTF("Keyboard: 'W' - Water alert, 'T' - Washroom alert\r\n");

    // Main loop: check for UART keyboard input
    while(1) {
        // Check if UART has received data (non-blocking)
        // Cast BOARD_DEBUG_UART_BASEADDR back to UART_Type* pointer
        UART_Type *uartBase = (UART_Type *)BOARD_DEBUG_UART_BASEADDR;
        uint32_t statusFlags = UART_GetStatusFlags(uartBase);
        if (statusFlags & kUART_RxDataRegFullFlag) {
            // Read character from UART
            uint8_t ch = UART_ReadByte(uartBase);
            
            // Process keyboard commands
            if (ch == 'W' || ch == 'w') {
                PRINTF("[KEYBOARD] 'W' pressed - Water alert\r\n");
                handle_water_alert();
            } else if (ch == 'T' || ch == 't') {
                PRINTF("[KEYBOARD] 'T' pressed - Washroom alert\r\n");
                handle_washroom_alert();
            } else if (ch != '\r' && ch != '\n') {
                // Ignore carriage return and newline, but echo other characters
                PRINTF("[KEYBOARD] Received: '%c' (0x%02X) - ignored\r\n", ch, ch);
            }
        }
        
        // Small delay to prevent CPU spinning
        __asm volatile ("nop");
    }
    return 0;
}

// Shared function to handle water alert (called from button or keyboard)
static void handle_water_alert(void) {
    if (current_state == STATE_WATER_ALERT) {
        // Cancel water alert (same command again)
        current_state = STATE_IDLE;
        func_green_led_off();
        func_red_led_off();  // Ensure red is also off
        led_blink_state = 0;
        PIT_StopTimer(PIT, kPIT_Chnl_0);
        PRINTF("Water alert cancelled (LED flicker OFF)\r\n");
    } else {
        // Start water alert (cancel any other alert first)
        if (current_state == STATE_WASHROOM_ALERT) {
            func_red_led_off();
            PRINTF("Cancelled washroom alert, starting water alert\r\n");
        }
        current_state = STATE_WATER_ALERT;
        func_green_led_on();
        led_blink_state = 1;
        PIT_StartTimer(PIT, kPIT_Chnl_0);
        PRINTF("Water alert started (Green LED flicker ON)\r\n");
    }
}

// SW2 button interrupt handler (PTD11) - Water button
// Handler name must match the interrupt vector table: PORTD_IRQHandler
void PORTD_IRQHandler(void) {
    GPIO_PortClearInterruptFlags(WATER_BUTTON_PORT, 1U << WATER_BUTTON_PIN);
    PRINTF("[BUTTON PRESS] SW2 pressed!\r\n");
    handle_water_alert();
}

// Shared function to handle washroom alert (called from button or keyboard)
static void handle_washroom_alert(void) {
    if (current_state == STATE_WASHROOM_ALERT) {
        // Cancel washroom alert (same command again)
        current_state = STATE_IDLE;
        func_red_led_off();
        func_green_led_off();  // Ensure green is also off
        led_blink_state = 0;
        PIT_StopTimer(PIT, kPIT_Chnl_0);
        PRINTF("Washroom alert cancelled (LED flicker OFF)\r\n");
    } else {
        // Start washroom alert (cancel any other alert first)
        if (current_state == STATE_WATER_ALERT) {
            func_green_led_off();
            PRINTF("Cancelled water alert, starting washroom alert\r\n");
        }
        current_state = STATE_WASHROOM_ALERT;
        func_red_led_on();
        led_blink_state = 1;
        PIT_StartTimer(PIT, kPIT_Chnl_0);
        PRINTF("Washroom alert started (Red LED flicker ON)\r\n");
    }
}


// SW3 button interrupt handler (PTA10) - Washroom button
// Handler name must match the interrupt vector table: PORTA_IRQHandler
void PORTA_IRQHandler(void) {
    GPIO_PortClearInterruptFlags(WASHROOM_BUTTON_PORT, 1U << WASHROOM_BUTTON_PIN);
    PRINTF("[BUTTON PRESS] SW3 pressed!\r\n");
    handle_washroom_alert();
}

// PIT Timer interrupt handler - triggers every 500ms for LED flicker
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
    } else if (current_state == STATE_WASHROOM_ALERT) {
        if (led_blink_state == 0) {
            func_red_led_on();
            led_blink_state = 1;
        } else {
            func_red_led_off();
            led_blink_state = 0;
        }
    }
}

// Setup button GPIO interrupts
// Manually configure pins since ConfigTools wasn't used
static void setup_button_interrupts(void) {
    port_pin_config_t portConfig;
    gpio_pin_config_t gpioConfig;
    
    // Configure GPIO as input (same for both buttons)
    gpioConfig.pinDirection = kGPIO_DigitalInput;
    
    // Configure pin settings (same for both buttons)
    portConfig.pullSelect = kPORT_PullUp;
    portConfig.slewRate = kPORT_FastSlewRate;
    portConfig.passiveFilterEnable = kPORT_PassiveFilterDisable;
    portConfig.openDrainEnable = kPORT_OpenDrainDisable;
    portConfig.driveStrength = kPORT_LowDriveStrength;
    portConfig.mux = kPORT_MuxAsGpio;
    portConfig.lockRegister = kPORT_UnlockRegister;
    
    // Configure SW2 (PTD11) - Water button
    CLOCK_EnableClock(kCLOCK_PortD);
    PORT_SetPinConfig(PORTD, WATER_BUTTON_PIN, &portConfig);
    PORT_SetPinInterruptConfig(PORTD, WATER_BUTTON_PIN, kPORT_InterruptFallingEdge);
    GPIO_PinInit(WATER_BUTTON_PORT, WATER_BUTTON_PIN, &gpioConfig);
    
    // Configure SW3 (PTA10) - Washroom button
    CLOCK_EnableClock(kCLOCK_PortA);
    PORT_SetPinConfig(PORTA, WASHROOM_BUTTON_PIN, &portConfig);
    PORT_SetPinInterruptConfig(PORTA, WASHROOM_BUTTON_PIN, kPORT_InterruptFallingEdge);
    GPIO_PinInit(WASHROOM_BUTTON_PORT, WASHROOM_BUTTON_PIN, &gpioConfig);
    
    // Enable NVIC interrupts
    EnableIRQ(PORTD_IRQn);
    EnableIRQ(PORTA_IRQn);
    
    PRINTF("Button interrupts configured: SW2(PTD11), SW3(PTA10)\r\n");
}