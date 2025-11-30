# Comprehensive Code Explanation

This document provides a detailed, line-by-line explanation of all code used in the SEH500 Project. Each line of code is explained in simple words to help understand what the program is doing.

---

## Table of Contents

1. [Assembly Code: gpio_led.s](#assembly-code-gpio_leds)
2. [Main Application: SEH500_Project.c](#main-application-seh500_projectc)

---

## Assembly Code: gpio_led.s

This file contains assembly language functions that directly control the hardware registers for the LEDs on the FRDM-K66F board.

### File Header and Configuration

**Lines 3-6: File Setup**

- Line 3: `.syntax unified` - This tells the assembler to use unified ARM/Thumb syntax, which allows mixing 16-bit and 32-bit instructions.
- Line 4: `.cpu cortex-m4` - This specifies that the code is written for the ARM Cortex-M4 processor (the CPU on the FRDM-K66F board).
- Line 5: `.thumb` - This tells the assembler to use Thumb instruction set, which uses 16-bit instructions for better code density.
- Line 6: `.text` - This marks the start of the code section (where executable instructions go).

### Function: setup_green()

**Lines 9-32: Setup Green LED Pin Configuration**

**Lines 9-11: Function Declaration**

- Line 9: `.global setup_green` - This makes the function name `setup_green` available to other files (so C code can call it).
- Line 10: `.type setup_green, %function` - This tells the assembler that `setup_green` is a function type.
- Line 11: `setup_green:` - This is the label that marks the start of the function.

**Line 12: Save Return Address**

- `push {lr}` - Save the link register (lr) onto the stack. The link register contains the address to return to when this function finishes. We save it because we'll call other functions.

**Lines 14-17: Enable Clock for Port E**

- Line 14: `ldr r1, =0x40048038` - Load the address of the SIM_SCGC5 register (System Integration Module - System Clock Gating Control Register 5) into register r1. This register controls which peripheral clocks are enabled.
- Line 15: `ldr r0, [r1]` - Load the current value from the register at address in r1 (SIM_SCGC5) into register r0. The brackets mean "read from memory address".
- Line 16: `orr r0, r0, #(1<<13)` - Perform a bitwise OR operation. Take the value in r0, OR it with the number 1 shifted left by 13 positions (which sets bit 13 to 1). This enables the clock for Port E without affecting other bits.
- Line 17: `str r0, [r1]` - Store the modified value from r0 back into the register at address in r1. This writes the new value to SIM_SCGC5, enabling Port E's clock.

**Lines 19-21: Configure Pin PTE6 as GPIO**

- Line 19: `ldr r1, =0x4004D018` - Load the address of PORTE_PCR6 (Port E Pin Control Register for pin 6) into register r1.
- Line 20: `mov r0, #0x0100` - Move the value 0x0100 (256 in decimal) into register r0. This value sets the pin to GPIO mode (mux = 1).
- Line 21: `str r0, [r1]` - Store the value from r0 into the register at address in r1. This configures pin PTE6 to be a GPIO pin.

**Lines 23-26: Set Pin PTE6 as Output**

- Line 23: `ldr r1, =0x400FF114` - Load the address of GPIOE_PDDR (GPIO Port E Data Direction Register) into register r1. This register controls whether each pin is an input or output.
- Line 24: `ldr r0, [r1]` - Load the current value from GPIOE_PDDR into register r0.
- Line 25: `orr r0, r0, #(1<<6)` - OR the value in r0 with bit 6 set to 1. This sets pin 6 as an output (1 = output, 0 = input).
- Line 26: `str r0, [r1]` - Store the modified value back to GPIOE_PDDR, making pin 6 an output.

**Lines 28-31: Initialize LED to OFF State**

- Line 28: `ldr r1, =0x400FF100` - Load the address of GPIOE_PDOR (GPIO Port E Data Output Register) into register r1. This register controls the actual output value.
- Line 29: `ldr r0, [r1]` - Load the current value from GPIOE_PDOR into register r0.
- Line 30: `orr r0, r0, #(1<<6)` - OR the value in r0 with bit 6 set to 1. This sets pin 6 output to HIGH (1), which turns the LED OFF (because it's active-low).
- Line 31: `str r0, [r1]` - Store the modified value back to GPIOE_PDOR, turning the LED off.

**Line 32: Return from Function**

- `pop {pc}` - Pop (restore) the saved link register from the stack and put it into the program counter (pc). This makes the function return to where it was called from.

### Function: setup_red()

**Lines 35-58: Setup Red LED Pin Configuration**

This function follows the exact same pattern as `setup_green()`, but for the red LED on pin PTC9.

**Lines 35-37: Function Declaration**

- Line 35: `.global setup_red` - Makes the function available to other files.
- Line 36: `.type setup_red, %function` - Declares this as a function.
- Line 37: `setup_red:` - Function label.

**Line 38: Save Return Address**

- `push {lr}` - Save the link register.

**Lines 40-43: Enable Clock for Port C**

- Line 40: `ldr r1, =0x40048038` - Load SIM_SCGC5 address (same register as before).
- Line 41: `ldr r0, [r1]` - Read current value.
- Line 42: `orr r0, r0, #(1<<11)` - Set bit 11 to enable Port C clock (bit 11 is for Port C, bit 13 was for Port E).
- Line 43: `str r0, [r1]` - Write back to enable Port C clock.

**Lines 45-47: Configure Pin PTC9 as GPIO**

- Line 45: `ldr r1, =0x4004B024` - Load address of PORTC_PCR9 (Port C Pin Control Register for pin 9).
- Line 46: `mov r0, #0x0100` - Set value to configure as GPIO.
- Line 47: `str r0, [r1]` - Write to configure pin 9 as GPIO.

**Lines 49-52: Set Pin PTC9 as Output**

- Line 49: `ldr r1, =0x400FF094` - Load address of GPIOC_PDDR (Port C Data Direction Register).
- Line 50: `ldr r0, [r1]` - Read current value.
- Line 51: `orr r0, r0, #(1<<9)` - Set bit 9 to make pin 9 an output.
- Line 52: `str r0, [r1]` - Write back to configure as output.

**Lines 54-57: Initialize LED to OFF State**

- Line 54: `ldr r1, =0x400FF080` - Load address of GPIOC_PDOR (Port C Data Output Register).
- Line 55: `ldr r0, [r1]` - Read current value.
- Line 56: `orr r0, r0, #(1<<9)` - Set bit 9 to HIGH (LED OFF).
- Line 57: `str r0, [r1]` - Write back to turn LED off.

**Line 58: Return from Function**

- `pop {pc}` - Return to caller.

### Function: func_green_led_on()

**Lines 61-68: Turn On Green LED**

**Lines 61-63: Function Declaration**

- Line 61: `.global func_green_led_on` - Makes function available to C code.
- Line 62: `.type func_green_led_on, %function` - Declares as function.
- Line 63: `func_green_led_on:` - Function label.

**Lines 64-67: Set LED Output to LOW (LED ON)**

- Line 64: `ldr r1, =0x400FF100` - Load address of GPIOE_PDOR (Port E output register).
- Line 65: `ldr r0, [r1]` - Read current output value.
- Line 66: `bic r0, r0, #(1<<6)` - BIC means "bit clear". Clear bit 6 in r0 (set it to 0). Since LEDs are active-low, writing 0 turns the LED ON.
- Line 67: `str r0, [r1]` - Write the modified value back, turning the LED on.

**Line 68: Return**

- `bx lr` - Branch and exchange with link register. This returns from the function. `bx` is used instead of `pop {pc}` because we didn't push anything onto the stack.

### Function: func_green_led_off()

**Lines 71-78: Turn Off Green LED**

**Lines 71-73: Function Declaration**

- Line 71: `.global func_green_led_off` - Makes function available.
- Line 72: `.type func_green_led_off, %function` - Declares as function.
- Line 73: `func_green_led_off:` - Function label.

**Lines 74-77: Set LED Output to HIGH (LED OFF)**

- Line 74: `ldr r1, =0x400FF100` - Load GPIOE_PDOR address.
- Line 75: `ldr r0, [r1]` - Read current value.
- Line 76: `orr r0, r0, #(1<<6)` - Set bit 6 to 1 (HIGH), which turns LED OFF (active-low).
- Line 77: `str r0, [r1]` - Write back to turn LED off.

**Line 78: Return**

- `bx lr` - Return from function.

### Function: func_red_led_on()

**Lines 81-88: Turn On Red LED**

**Lines 81-83: Function Declaration**

- Line 81: `.global func_red_led_on` - Makes function available.
- Line 82: `.type func_red_led_on, %function` - Declares as function.
- Line 83: `func_red_led_on:` - Function label.

**Lines 84-87: Set LED Output to LOW (LED ON)**

- Line 84: `ldr r1, =0x400FF080` - Load address of GPIOC_PDOR (Port C output register).
- Line 85: `ldr r0, [r1]` - Read current value.
- Line 86: `bic r0, r0, #(1<<9)` - Clear bit 9 (set to 0), turning LED ON (active-low).
- Line 87: `str r0, [r1]` - Write back to turn LED on.

**Line 88: Return**

- `bx lr` - Return from function.

### Function: func_red_led_off()

**Lines 91-98: Turn Off Red LED**

**Lines 91-93: Function Declaration**

- Line 91: `.global func_red_led_off` - Makes function available.
- Line 92: `.type func_red_led_off, %function` - Declares as function.
- Line 93: `func_red_led_off:` - Function label.

**Lines 94-97: Set LED Output to HIGH (LED OFF)**

- Line 94: `ldr r1, =0x400FF080` - Load GPIOC_PDOR address.
- Line 95: `ldr r0, [r1]` - Read current value.
- Line 96: `orr r0, r0, #(1<<9)` - Set bit 9 to 1 (HIGH), turning LED OFF.
- Line 97: `str r0, [r1]` - Write back to turn LED off.

**Line 98: Return**

- `bx lr` - Return from function.

### Function: setup_leds()

**Lines 101-107: Master Setup Function**

**Lines 101-103: Function Declaration**

- Line 101: `.global setup_leds` - Makes function available.
- Line 102: `.type setup_leds, %function` - Declares as function.
- Line 103: `setup_leds:` - Function label.

**Lines 104-107: Call Setup Functions**

- Line 104: `push {lr}` - Save link register because we'll call other functions.
- Line 105: `bl setup_green` - Branch with link to `setup_green`. This calls the function to setup the green LED. The `bl` instruction automatically saves the return address in the link register.
- Line 106: `bl setup_red` - Branch with link to `setup_red`. This calls the function to setup the red LED.
- Line 107: `pop {pc}` - Restore link register to program counter and return.

---

## Main Application: SEH500_Project.c

This is the main C application file that ties everything together.

### File Header and Includes

**Lines 6-17: Include Files**

- Line 6: `#include <stdio.h>` - Include standard input/output library for printf-like functions.
- Line 7: `#include "board.h"` - Include board-specific definitions and initialization functions.
- Line 8: `#include "peripherals.h"` - Include peripheral configuration definitions.
- Line 9: `#include "pin_mux.h"` - Include pin multiplexing configuration.
- Line 10: `#include "clock_config.h"` - Include clock configuration settings.
- Line 11: `#include "fsl_debug_console.h"` - Include debug console functions (for PRINTF).
- Line 12: `#include "fsl_gpio.h"` - Include GPIO (General Purpose Input/Output) driver functions.
- Line 13: `#include "fsl_port.h"` - Include port configuration functions.
- Line 14: `#include "fsl_pit.h"` - Include PIT (Periodic Interrupt Timer) driver functions.
- Line 15: `#include "fsl_clock.h"` - Include clock control functions.
- Line 16: `#include "fsl_common.h"` - Include common utility functions.
- Line 17: `#include "fsl_uart.h"` - Include UART (serial communication) driver functions.

### Function Prototypes and Declarations

**Lines 20-24: External Assembly Function Prototypes**

- Line 20: `void setup_leds(void);` - Declare that there's a function called `setup_leds` that takes no parameters and returns nothing. This function is defined in the assembly file.
- Line 21: `void func_green_led_on(void);` - Declare function to turn green LED on.
- Line 22: `void func_green_led_off(void);` - Declare function to turn green LED off.
- Line 23: `void func_red_led_on(void);` - Declare function to turn red LED on.
- Line 24: `void func_red_led_off(void);` - Declare function to turn red LED off.

**Lines 27-29: Forward Function Declarations**

- Line 27: `static void setup_button_interrupts(void);` - Declare a function that will configure button interrupts. `static` means it's only used within this file.
- Line 28: `static void handle_water_alert(void);` - Declare function to handle water alert logic.
- Line 29: `static void handle_washroom_alert(void);` - Declare function to handle washroom alert logic.

**Lines 32-36: System State Enumeration**

- Line 32: `typedef enum {` - Start defining a new type called an enumeration (a list of named constants).
- Line 33: `    STATE_IDLE = 0,` - Define state constant for idle (no alert active), value is 0.
- Line 34: `    STATE_WATER_ALERT,` - Define state constant for water alert active, value is 1 (automatically assigned).
- Line 35: `    STATE_WASHROOM_ALERT` - Define state constant for washroom alert active, value is 2.
- Line 36: `} system_state_t;` - End the enum definition and name the type `system_state_t`.

**Lines 39-40: Global Variables**

- Line 39: `volatile static system_state_t current_state = STATE_IDLE;` - Declare a global variable `current_state` of type `system_state_t`, initialized to `STATE_IDLE`. `volatile` tells the compiler this variable can change unexpectedly (like from interrupts), so don't optimize it away. `static` means it's only accessible within this file.
- Line 40: `volatile static int led_blink_state = 0;` - Declare a global variable to track LED blink state (0 = OFF, 1 = ON), initialized to 0. Also volatile because interrupts change it.

**Lines 45-48: Button Pin Definitions**

- Line 45: `#define WATER_BUTTON_PORT      GPIOD` - Define a constant: WATER_BUTTON_PORT means GPIOD (GPIO Port D).
- Line 46: `#define WATER_BUTTON_PIN       11U` - Define a constant: WATER_BUTTON_PIN is pin number 11. The `U` means unsigned integer.
- Line 47: `#define WASHROOM_BUTTON_PORT   GPIOA` - Define a constant: WASHROOM_BUTTON_PORT means GPIOA (GPIO Port A).
- Line 48: `#define WASHROOM_BUTTON_PIN    10U` - Define a constant: WASHROOM_BUTTON_PIN is pin number 10.

### Main Function

**Line 51: Function Declaration**

- `int main(void) {` - Start of the main function. This is where the program starts executing.

**Lines 52-55: Board Initialization**

- Line 52: `BOARD_InitBootPins();` - Call function to initialize all the pin configurations for the board.
- Line 53: `BOARD_InitBootClocks();` - Call function to initialize the clock system (CPU speed, peripheral clocks, etc.).
- Line 54: `BOARD_InitBootPeripherals();` - Call function to initialize all the peripheral modules.
- Line 55: `BOARD_InitDebugConsole();` - Call function to initialize the debug console (UART) so we can use PRINTF.

**Lines 56-57: Print Welcome Messages**

- Line 56: `PRINTF("=== Assistive Audio-Visual Communicator ===\r\n");` - Print a title message. `\r\n` is carriage return and newline (end of line).
- Line 57: `PRINTF("Initializing system...\r\n");` - Print a message saying the system is initializing.

**Lines 60-63: Initialize LEDs**

- Line 60: `setup_leds();` - Call the assembly function to configure both LED pins.
- Line 61: `func_green_led_off();` - Call assembly function to turn green LED off (ensure it starts off).
- Line 62: `func_red_led_off();` - Call assembly function to turn red LED off (ensure it starts off).
- Line 63: `PRINTF("Onboard LEDs initialized (Green=Water, Red=Washroom)\r\n");` - Print confirmation message.

**Lines 66-72: Initialize PIT Timer**

- Line 66: `pit_config_t pitConfig;` - Declare a variable of type `pit_config_t` to hold timer configuration settings.
- Line 67: `PIT_GetDefaultConfig(&pitConfig);` - Call function to fill `pitConfig` with default timer settings. The `&` means "address of" - we're passing the address of the variable.
- Line 68: `PIT_Init(PIT, &pitConfig);` - Initialize the PIT timer module with the configuration. `PIT` is the timer peripheral, `&pitConfig` is the configuration.
- Line 69: `PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, USEC_TO_COUNT(500000U, CLOCK_GetFreq(kCLOCK_BusClk)));` - Set timer period to 500ms. `PIT` is the timer, `kPIT_Chnl_0` is channel 0, `USEC_TO_COUNT` converts 500000 microseconds to timer counts based on bus clock frequency.
- Line 70: `PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);` - Enable interrupts for the timer. When the timer expires, it will trigger an interrupt.
- Line 71: `EnableIRQ(PIT0_IRQn);` - Enable the PIT0 interrupt in the NVIC (Nested Vectored Interrupt Controller). This allows the interrupt to actually fire.
- Line 72: `PRINTF("Timer initialized (500ms LED flicker)\r\n");` - Print confirmation message.

**Lines 75-76: Setup Button Interrupts**

- Line 75: `setup_button_interrupts();` - Call function to configure button interrupts (defined later in the file).
- Line 76: `PRINTF("Button interrupts configured\r\n");` - Print confirmation message.

**Lines 78-81: Print Instructions**

- Line 78: `PRINTF("System ready.\r\n");` - Print that system is ready.
- Line 79: `PRINTF("SW2 - Toggle water alert (Green LED flicker)\r\n");` - Print instruction for SW2 button.
- Line 80: `PRINTF("SW3 - Toggle washroom alert (Red LED flicker)\r\n");` - Print instruction for SW3 button.
- Line 81: `PRINTF("Keyboard: 'W' - Water alert, 'T' - Washroom alert\r\n");` - Print instruction for keyboard commands.
- Line 82: `PRINTF("All inputs now use interrupts (optimized - no polling!)\r\n");` - Print message indicating that all inputs use interrupts for optimization.

**Lines 79-81: Setup UART Interrupts**

- Line 79: `setup_uart_interrupts();` - Call function to configure UART interrupts for keyboard input (defined later in the file).
- Line 80: `PRINTF("UART interrupts configured (keyboard input)\r\n");` - Print confirmation message.

**Lines 89-95: Main Loop (Interrupt-Driven)**

- Line 89: `// Main loop: Now empty! CPU can sleep or do other work` - Comment explaining that the main loop is now empty because all input handling is done via interrupts.
- Line 90: `// All input handling is done via interrupts (buttons and keyboard)` - Comment explaining that interrupts handle all inputs.
- Line 91: `while(1) {` - Start an infinite loop. The program will run this loop forever.
- Line 92: `// CPU can enter low-power mode or do other tasks` - Comment explaining that the CPU can sleep or do other work.
- Line 93: `// No polling needed - interrupts handle everything!` - Comment explaining that polling is no longer needed.
- Line 94: `__WFI();` - Wait For Interrupt instruction. This tells the CPU to enter low-power mode and wait until an interrupt occurs. When an interrupt happens (button press, keyboard input, or timer), the CPU wakes up, handles the interrupt, then returns here to sleep again. This is much more efficient than constantly polling for input.
- Line 95: `}` - End of the while loop.

**Line 96: Return Statement**

- `return 0;` - Return 0 from main function (indicates successful program completion). This line is never reached because of the infinite loop, but it's good practice to include it.

### Function: handle_water_alert()

**Line 113: Function Declaration**

- `static void handle_water_alert(void) {` - Define a function that handles water alert logic. `static` means it's only used within this file.

**Lines 114-121: Cancel Water Alert (if already active)**

- Line 114: `if (current_state == STATE_WATER_ALERT) {` - Check if the current state is already WATER_ALERT.
- Line 116: `current_state = STATE_IDLE;` - Change state to IDLE (no alert active).
- Line 117: `func_green_led_off();` - Turn off the green LED.
- Line 118: `func_red_led_off();` - Also turn off red LED to ensure it's off.
- Line 119: `led_blink_state = 0;` - Set blink state to 0 (OFF).
- Line 120: `PIT_StopTimer(PIT, kPIT_Chnl_0);` - Stop the timer so LED stops blinking.
- Line 121: `PRINTF("Water alert cancelled (LED flicker OFF)\r\n");` - Print confirmation message.

**Lines 122-133: Start Water Alert (if not active)**

- Line 122: `} else {` - Otherwise (if state is not WATER_ALERT).
- Line 124: `if (current_state == STATE_WASHROOM_ALERT) {` - Check if washroom alert is currently active.
- Line 125: `func_red_led_off();` - Turn off red LED (cancel washroom alert).
- Line 126: `PRINTF("Cancelled washroom alert, starting water alert\r\n");` - Print message about switching alerts.
- Line 127: `}` - End of if statement checking for washroom alert.
- Line 128: `current_state = STATE_WATER_ALERT;` - Change state to WATER_ALERT.
- Line 129: `func_green_led_on();` - Turn on green LED.
- Line 130: `led_blink_state = 1;` - Set blink state to 1 (ON).
- Line 131: `PIT_StartTimer(PIT, kPIT_Chnl_0);` - Start the timer to begin LED blinking.
- Line 132: `PRINTF("Water alert started (Green LED flicker ON)\r\n");` - Print confirmation message.
- Line 133: `}` - End of else block.
- Line 134: `}` - End of function.

### Function: PORTD_IRQHandler()

**Line 139: Function Declaration**

- `void PORTD_IRQHandler(void) {` - Define the interrupt handler function. This function is automatically called by the hardware when Port D interrupt occurs (when SW2 is pressed).

**Lines 140-142: Handle Interrupt**

- Line 140: `GPIO_PortClearInterruptFlags(WATER_BUTTON_PORT, 1U << WATER_BUTTON_PIN);` - Clear the interrupt flag for the water button. This is critical - if we don't clear it, the interrupt will fire repeatedly. `1U << WATER_BUTTON_PIN` creates a bitmask with bit 11 set.
- Line 141: `PRINTF("[BUTTON PRESS] SW2 pressed!\r\n");` - Print a message that SW2 was pressed.
- Line 142: `handle_water_alert();` - Call the function to handle water alert (same logic as keyboard 'W').
- Line 143: `}` - End of function.

### Function: handle_washroom_alert()

**Line 145: Function Declaration**

- `static void handle_washroom_alert(void) {` - Define function to handle washroom alert.

**Lines 146-153: Cancel Washroom Alert (if already active)**

- Line 146: `if (current_state == STATE_WASHROOM_ALERT) {` - Check if washroom alert is already active.
- Line 148: `current_state = STATE_IDLE;` - Change state to IDLE.
- Line 149: `func_red_led_off();` - Turn off red LED.
- Line 150: `func_green_led_off();` - Also turn off green LED to ensure it's off.
- Line 151: `led_blink_state = 0;` - Set blink state to 0.
- Line 152: `PIT_StopTimer(PIT, kPIT_Chnl_0);` - Stop the timer.
- Line 153: `PRINTF("Washroom alert cancelled (LED flicker OFF)\r\n");` - Print confirmation.

**Lines 154-164: Start Washroom Alert (if not active)**

- Line 154: `} else {` - Otherwise (if not already in washroom alert state).
- Line 156: `if (current_state == STATE_WATER_ALERT) {` - Check if water alert is currently active.
- Line 157: `func_green_led_off();` - Turn off green LED (cancel water alert).
- Line 158: `PRINTF("Cancelled water alert, starting washroom alert\r\n");` - Print message about switching.
- Line 159: `}` - End of if statement.
- Line 160: `current_state = STATE_WASHROOM_ALERT;` - Change state to WASHROOM_ALERT.
- Line 161: `func_red_led_on();` - Turn on red LED.
- Line 162: `led_blink_state = 1;` - Set blink state to 1.
- Line 163: `PIT_StartTimer(PIT, kPIT_Chnl_0);` - Start the timer.
- Line 164: `PRINTF("Washroom alert started (Red LED flicker ON)\r\n");` - Print confirmation.
- Line 165: `}` - End of else block.
- Line 166: `}` - End of function.

### Function: PORTA_IRQHandler()

**Line 171: Function Declaration**

- `void PORTA_IRQHandler(void) {` - Define the interrupt handler for Port A.

**Lines 172-174: Handle Interrupt**

- Line 172: `GPIO_PortClearInterruptFlags(WASHROOM_BUTTON_PORT, 1U << WASHROOM_BUTTON_PIN);` - Clear the interrupt flag for the washroom button.
- Line 173: `PRINTF("[BUTTON PRESS] SW3 pressed!\r\n");` - Print message that SW3 was pressed.
- Line 174: `handle_washroom_alert();` - Call function to handle washroom alert.
- Line 175: `}` - End of function.

### Function: PIT0_IRQHandler()

**Line 178: Function Declaration**

- `void PIT0_IRQHandler(void) {` - Define the interrupt handler for PIT channel 0.

**Line 178: Clear Timer Flag**

- Line 178: `PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);` - Clear the timer interrupt flag. This is important to prevent the interrupt from firing repeatedly.

**Lines 181-197: Toggle Appropriate LED**

- Line 181: `if (current_state == STATE_WATER_ALERT) {` - Check if we're in water alert state.
- Line 182: `if (led_blink_state == 0) {` - Check if LED is currently off.
- Line 183: `func_green_led_on();` - Turn green LED on.
- Line 184: `led_blink_state = 1;` - Set blink state to 1 (ON).
- Line 185: `} else {` - Otherwise (LED is on).
- Line 186: `func_green_led_off();` - Turn green LED off.
- Line 187: `led_blink_state = 0;` - Set blink state to 0 (OFF).
- Line 188: `}` - End of inner if-else.
- Line 189: `} else if (current_state == STATE_WASHROOM_ALERT) {` - Otherwise, if we're in washroom alert state.
- Line 190: `if (led_blink_state == 0) {` - Check if LED is currently off.
- Line 191: `func_red_led_on();` - Turn red LED on.
- Line 192: `led_blink_state = 1;` - Set blink state to 1.
- Line 193: `} else {` - Otherwise (LED is on).
- Line 194: `func_red_led_off();` - Turn red LED off.
- Line 195: `led_blink_state = 0;` - Set blink state to 0.
- Line 196: `}` - End of inner if-else.
- Line 197: `}` - End of outer if-else.
- Line 198: `}` - End of function.

### Function: setup_button_interrupts()

**Line 203: Function Declaration**

- `static void setup_button_interrupts(void) {` - Define function to setup button interrupts.

**Lines 204-205: Declare Configuration Variables**

- Line 204: `port_pin_config_t portConfig;` - Declare variable to hold port pin configuration.
- Line 205: `gpio_pin_config_t gpioConfig;` - Declare variable to hold GPIO pin configuration.

**Line 208: Set GPIO Direction**

- Line 208: `gpioConfig.pinDirection = kGPIO_DigitalInput;` - Set pin direction to input (buttons are inputs).

**Lines 211-217: Configure Port Pin Settings**

- Line 211: `portConfig.pullSelect = kPORT_PullUp;` - Enable internal pull-up resistor (keeps pin HIGH when button not pressed).
- Line 212: `portConfig.slewRate = kPORT_FastSlewRate;` - Set fast slew rate (fast signal transitions).
- Line 213: `portConfig.passiveFilterEnable = kPORT_PassiveFilterDisable;` - Disable passive filter (no hardware debouncing).
- Line 214: `portConfig.openDrainEnable = kPORT_OpenDrainDisable;` - Disable open-drain mode (use push-pull output).
- Line 215: `portConfig.driveStrength = kPORT_LowDriveStrength;` - Set low drive strength (sufficient for buttons).
- Line 216: `portConfig.mux = kPORT_MuxAsGpio;` - Set pin multiplexer to GPIO mode (not alternate function).
- Line 217: `portConfig.lockRegister = kPORT_UnlockRegister;` - Unlock the register (allow changes).

**Lines 220-223: Configure SW2 (Water Button)**

- Line 220: `CLOCK_EnableClock(kCLOCK_PortD);` - Enable the clock for Port D (required for Port D to work).
- Line 221: `PORT_SetPinConfig(PORTD, WATER_BUTTON_PIN, &portConfig);` - Apply the port configuration to pin 11 of Port D.
- Line 222: `PORT_SetPinInterruptConfig(PORTD, WATER_BUTTON_PIN, kPORT_InterruptFallingEdge);` - Configure pin 11 to trigger interrupt on falling edge (when button is pressed, signal goes from HIGH to LOW).
- Line 223: `GPIO_PinInit(WATER_BUTTON_PORT, WATER_BUTTON_PIN, &gpioConfig);` - Initialize the GPIO pin with the GPIO configuration (set as input).

**Lines 226-229: Configure SW3 (Washroom Button)**

- Line 226: `CLOCK_EnableClock(kCLOCK_PortA);` - Enable the clock for Port A.
- Line 227: `PORT_SetPinConfig(PORTA, WASHROOM_BUTTON_PIN, &portConfig);` - Apply port configuration to pin 10 of Port A.
- Line 228: `PORT_SetPinInterruptConfig(PORTA, WASHROOM_BUTTON_PIN, kPORT_InterruptFallingEdge);` - Configure pin 10 for falling edge interrupt.
- Line 229: `GPIO_PinInit(WASHROOM_BUTTON_PORT, WASHROOM_BUTTON_PIN, &gpioConfig);` - Initialize GPIO pin as input.

**Lines 232-233: Enable NVIC Interrupts**

- Line 232: `EnableIRQ(PORTD_IRQn);` - Enable Port D interrupt in the NVIC (allows PORTD_IRQHandler to be called).
- Line 233: `EnableIRQ(PORTA_IRQn);` - Enable Port A interrupt in the NVIC (allows PORTA_IRQHandler to be called).

**Line 235: Print Confirmation**

- `PRINTF("Button interrupts configured: SW2(PTD11), SW3(PTA10)\r\n");` - Print confirmation message with pin information.

**Line 236: End Function**

- `}` - End of function.

### Function: UART0_RX_TX_IRQHandler()

**Line 191: Function Declaration**

- `void UART0_RX_TX_IRQHandler(void) {` - Define the UART interrupt handler function. This function is automatically called by the hardware when UART0 receives data (when a keyboard character arrives). The handler name must match the interrupt vector table.

**Lines 192-193: Get UART Base Address and Status Flags**

- Line 192: `UART_Type *uartBase = (UART_Type *)BOARD_DEBUG_UART_BASEADDR;` - Create a pointer variable `uartBase` that points to the UART hardware. Cast `BOARD_DEBUG_UART_BASEADDR` (which is a number) to a `UART_Type *` pointer type.
- Line 193: `uint32_t statusFlags = UART_GetStatusFlags(uartBase);` - Read the UART status flags register to see what event triggered the interrupt. Store the result in `statusFlags`.

**Lines 195-196: Check if Data Was Received**

- Line 195: `// Check if data was received` - Comment explaining what we're checking.
- Line 196: `if (statusFlags & kUART_RxDataRegFullFlag) {` - Check if the "Receive Data Register Full" flag is set. The `&` is a bitwise AND operation. If the flag is set, it means a character was received and is ready to be read.

**Lines 197-198: Read Character**

- Line 197: `// Read character from UART receive register` - Comment explaining what we're doing.
- Line 198: `uint8_t ch = UART_ReadByte(uartBase);` - Read one byte (character) from the UART receive register and store it in variable `ch`. This removes the character from the UART's receive buffer.

**Lines 200-210: Process Keyboard Commands**

- Line 200: `// Process keyboard commands` - Comment explaining what we're doing.
- Line 201: `if (ch == 'W' || ch == 'w') {` - Check if the character is uppercase 'W' OR lowercase 'w'. The `||` means logical OR.
- Line 202: `PRINTF("[KEYBOARD] 'W' pressed - Water alert\r\n");` - Print a message that 'W' was pressed.
- Line 203: `handle_water_alert();` - Call the function to handle water alert (toggle it on or off).
- Line 204: `} else if (ch == 'T' || ch == 't') {` - Otherwise, if the character is 'T' or 't'.
- Line 205: `PRINTF("[KEYBOARD] 'T' pressed - Washroom alert\r\n");` - Print a message that 'T' was pressed.
- Line 206: `handle_washroom_alert();` - Call the function to handle washroom alert.
- Line 207: `} else if (ch != '\r' && ch != '\n') {` - Otherwise, if the character is NOT carriage return AND NOT newline (these are line ending characters we want to ignore).
- Line 209: `PRINTF("[KEYBOARD] Received: '%c' (0x%02X) - ignored\r\n", ch, ch);` - Print a message showing what character was received (as character and as hexadecimal number), but we're ignoring it.
- Line 210: `}` - End the else-if block.
- Line 211: `}` - End the if statement checking for received data.
- Line 212: `}` - End of function.

### Function: setup_uart_interrupts()

**Line 253: Function Declaration**

- `static void setup_uart_interrupts(void) {` - Define a function that configures UART interrupts for keyboard input. `static` means it's only used within this file.

**Line 254: Get UART Base Address**

- `UART_Type *uartBase = (UART_Type *)BOARD_DEBUG_UART_BASEADDR;` - Create a pointer variable `uartBase` that points to the UART hardware register.

**Lines 256-257: Enable UART RX Interrupt**

- Line 256: `// Enable UART RX interrupt (triggered when data arrives in receive register)` - Comment explaining what we're doing.
- Line 257: `UART_EnableInterrupts(uartBase, kUART_RxDataRegFullInterruptEnable);` - Enable the UART receive interrupt. This tells the UART hardware to generate an interrupt signal when data arrives in the receive register. The interrupt will trigger `UART0_RX_TX_IRQHandler()` automatically.

**Lines 259-261: Enable NVIC Interrupt**

- Line 259: `// Enable UART interrupt in NVIC (interrupt controller)` - Comment explaining what we're doing.
- Line 260: `// Note: UART0_RX_TX_IRQn is the interrupt number for UART0 on FRDM-K66F` - Comment explaining which interrupt number we're using.
- Line 261: `EnableIRQ(UART0_RX_TX_IRQn);` - Enable the UART0 interrupt in the NVIC (Nested Vectored Interrupt Controller). This allows the interrupt handler to actually be called when the UART receives data. Without this, the interrupt would be generated but not processed.

**Line 263: Print Confirmation**

- `PRINTF("UART RX interrupts enabled - keyboard input now interrupt-driven\r\n");` - Print confirmation message indicating that UART interrupts are configured.

**Line 264: End Function**

- `}` - End of function.

---

## Summary

This code implements an assistive communication device with the following key features:

1. **LED Control (Assembly)**: Direct hardware register manipulation to control green and red LEDs.
2. **Button Interrupts**: Hardware interrupts detect button presses instantly without polling.
3. **UART Interrupts**: Hardware interrupts detect keyboard input instantly without polling.
4. **State Machine**: Three states (IDLE, WATER_ALERT, WASHROOM_ALERT) manage system behavior.
5. **Timer Interrupts**: PIT timer generates periodic interrupts to blink LEDs during alerts.
6. **UART Communication**: Bidirectional serial communication allows keyboard control and debug output.

The code follows embedded systems best practices:

- Interrupt-driven design (no polling)
- Proper interrupt flag clearing
- Volatile variables for shared state
- Modular function design
- Clear state machine logic
