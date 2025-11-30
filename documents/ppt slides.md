# SEH500 Project Presentation: Assistive Communication Device

**Project Members:** Hadj Benseddik, Danial Ebadati

---

## Slide 1: Introduction

**Assistive Communication Device**
**Proof of Concept**

A prototype device for non-verbal patients to signal basic needs

**Hardware:** NXP FRDM-K66F Microcontroller Board

**Features:**

- Two buttons: SW2 (water) and SW3 (washroom)
- Color-coded LEDs: Green for water, Red for washroom
- Serial communication for keyboard control

**Purpose:** Demonstrate embedded systems concepts: GPIO, interrupts, and assembly programming

**Future Vision:** This proof of concept could be scaled into a battery-powered device similar to a TV remote, with customizable buttons for each patient's needs

---

## Slide 2: LED Setup Process

**Enabling the LEDs - Step by Step**

**LEDs Used:** Green (PTE6), Red (PTC9)

**Setup Process:**

1. **Enable Clock** - Set bit in SIM_SCGC5 register

   - Port E: bit 13
   - Port C: bit 11
2. **Configure Pin Mode** - Set PORT_PCR register to GPIO mode (0x0100)
3. **Set as Output** - Configure GPIO_PDDR register (set bit = output)
4. **Initialize State** - Set GPIO_PDOR to turn LED OFF initially

**Why Assembly:** Direct register access for efficient hardware control

**Code Location:** `gpio_led.s` - `setup_green()` and `setup_red()` functions

**Assembly Code Example:**

```assembly
// Enable Port E clock
ldr r1, =0x40048038     // SIM_SCGC5
ldr r0, [r1]
orr r0, r0, #(1<<13)    // Enable Port E (bit 13)
str r0, [r1]
```

---

## Slide 3: Assembly LED Control Functions

**Assembly Functions for LED Control**

**Key Functions in `gpio_led.s`:**

- `setup_leds()` - Initializes both LEDs
- `func_green_led_on()` / `func_green_led_off()`
- `func_red_led_on()` / `func_red_led_off()`

**How They Work:**

- Read current GPIO_PDOR register value
- Modify specific bit using BIC (clear) or ORR (set)
- Write back to register

**Active-Low Logic:** Writing 0 turns LED ON, writing 1 turns LED OFF

**Read-Modify-Write:** Ensures other pins on same port are not affected

**Assembly Code Example:**

```assembly
func_green_led_on:
    ldr r1, =0x400FF100     // GPIOE_PDOR
    ldr r0, [r1]
    bic r0, r0, #(1<<6)     // Clear bit 6 (LOW = LED ON)
    str r0, [r1]
    bx  lr                  // Return
```

---

## Slide 4: PIT Timer Setup for LED Flashing

**Periodic Interrupt Timer (PIT) Configuration**

**Purpose:** Generate interrupts every 500ms to toggle LED states

**Setup Steps:**

1. Get default PIT configuration
2. Initialize PIT peripheral
3. Set timer period: 500,000 microseconds
4. Enable timer interrupts
5. Enable NVIC interrupt for PIT0

**How It Works:** Timer counts down, generates interrupt when it reaches zero, then reloads

**Code Location:** `SEH500_Project.c` - lines 66-72

**C Code:**

```c
pit_config_t pitConfig;
PIT_GetDefaultConfig(&pitConfig);
PIT_Init(PIT, &pitConfig);
PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, 
    USEC_TO_COUNT(500000U, CLOCK_GetFreq(kCLOCK_BusClk)));
PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);
EnableIRQ(PIT0_IRQn);
```

---

## Slide 5: Button Setup Process

**Enabling Button Interrupts - Complete Process**

**Buttons:** SW2 (PTD11) for water, SW3 (PTA10) for washroom

**Setup Process:**

1. **Enable Clock** - `CLOCK_EnableClock()` for Port D and Port A
2. **Pin Configuration** - Set pull-up resistor, GPIO mode, fast slew rate
3. **Interrupt Type** - Configure falling-edge detection (button press = LOW)
4. **GPIO Init** - Set pin as digital input
5. **NVIC Enable** - Enable interrupt in interrupt controller

**Why Interrupts:** CPU doesn't need to poll - responds instantly when button pressed

**Code Location:** `SEH500_Project.c` - `setup_button_interrupts()` function

**Key Code:**

```c
CLOCK_EnableClock(kCLOCK_PortD);
PORT_SetPinConfig(PORTD, WATER_BUTTON_PIN, &portConfig);
PORT_SetPinInterruptConfig(PORTD, WATER_BUTTON_PIN, 
    kPORT_InterruptFallingEdge);
GPIO_PinInit(WATER_BUTTON_PORT, WATER_BUTTON_PIN, &gpioConfig);
EnableIRQ(PORTD_IRQn);
```

---

## Slide 6: Interrupt Service Routines

**Interrupt Handlers - How They Work**

**Four Interrupt Handlers:**

1. **PORTD_IRQHandler** - Handles SW2 (water button)
2. **PORTA_IRQHandler** - Handles SW3 (washroom button)
3. **UART0_RX_TX_IRQHandler** - Handles keyboard input
4. **PIT0_IRQHandler** - Handles 500ms timer for LED flashing

**Handler Process:**

- Clear interrupt flag first (prevents re-triggering)
- Process the event (read character, check button state, toggle LED)
- Call appropriate alert function (for buttons/keyboard)
- Return from interrupt

**Flag Clearing:** Critical - prevents infinite interrupt loops

**Code Location:** `SEH500_Project.c` - interrupt handlers section

**Example Handlers:**

```c
void PORTD_IRQHandler(void) {
    GPIO_PortClearInterruptFlags(WATER_BUTTON_PORT, 
        1U << WATER_BUTTON_PIN);
    handle_water_alert();
}

void UART0_RX_TX_IRQHandler(void) {
    uint8_t ch = UART_ReadByte(uartBase);
    if (ch == 'W' || ch == 'w') {
        handle_water_alert();
    }
}
```

---

## Slide 7: State Machine Logic

**System State Management**

**Three States:**

- `STATE_IDLE` - No active alert
- `STATE_WATER_ALERT` - Water request active (green LED flashing)
- `STATE_WASHROOM_ALERT` - Washroom request active (red LED flashing)

**State Transitions:**

- From IDLE: Press button → Enter alert state, start LED flashing
- From Alert: Press same button → Return to IDLE, stop LED
- Switch Alert: Press different button → Cancel current, start new alert

**Mutual Exclusivity:** Only one alert active at a time

**Code Location:** `SEH500_Project.c` - `handle_water_alert()` and `handle_washroom_alert()`

**State Diagram:**

```
IDLE ←→ WATER_ALERT
  ↕
WASHROOM_ALERT
```

---

## Slide 8: UART Serial Communication

**Bidirectional UART Communication**

**Purpose:** Allow keyboard control and debug output

**Configuration:** 115200 baud rate

**Features:**

- **Transmission:** Debug messages sent to computer
- **Reception:** Keyboard input ('W' for water, 'T' for washroom)

**How It Works:**

- UART hardware generates interrupt when character arrives
- Interrupt handler reads character and processes command
- CPU can sleep between keyboard inputs (no polling needed)

**Why Interrupts:** Polling would mean CPU constantly scanning for new characters, wasting CPU cycles. Interrupts allow CPU to sleep until data arrives.

**Code Location:** `SEH500_Project.c` - `UART0_RX_TX_IRQHandler()` and `setup_uart_interrupts()`

**Key Code:**

```c
void UART0_RX_TX_IRQHandler(void) {
    if (statusFlags & kUART_RxDataRegFullFlag) {
        uint8_t ch = UART_ReadByte(uartBase);
        if (ch == 'W' || ch == 'w') {
            handle_water_alert();
        }
    }
}
```

---

## Slide 9: System Architecture Overview

**Complete System Integration**

**Hardware Components:**

- GPIO: Buttons (input) and LEDs (output)
- PIT: Timer for LED flashing
- UART: Serial communication

**Software Components:**

- C code: Main application, state machine, interrupt handlers
- Assembly code: Direct LED control (100+ lines)

**Data Flow:**

1. Button press → GPIO interrupt → Handler → State change
2. Keyboard input → UART interrupt → Handler → State change
3. State change → LED control → Assembly functions
4. PIT interrupt → LED toggle → Visual feedback

**Key Design:** Fully interrupt-driven (no polling), modular code structure

**Block Diagram:**

```
Buttons → GPIO Interrupts → State Machine → LED Control (Assembly)
                                    ↓
                            PIT Timer (500ms)
                                    ↓
                            LED Flashing
```

---

## Slide 10: Conclusion

**Project Summary and Achievements**

**What We Built:** Functional proof of concept for assistive communication device

**Key Features Demonstrated:**

- GPIO configuration and control
- Interrupt-driven input handling
- Assembly language programming (100+ lines)
- State machine design
- Bidirectional UART communication

**Course Requirements Met:**

- GPIO interrupt handlers
- Bidirectional UART
- State machine logic
- Extensive assembly code

**Future Potential:** Scalable to portable, battery-powered device with customizable buttons

**Code Available:** GitHub repository with detailed comments

---

## Presentation Script

### Slide 1: Introduction

"Good [morning/afternoon]. Today we're presenting our SEH500 project: an Assistive Communication Device. This is a proof of concept designed for non-verbal patients to signal basic needs like water or washroom assistance. We built this using the NXP FRDM-K66F microcontroller board. The system uses two buttons - SW2 for water requests and SW3 for washroom requests - with color-coded LEDs providing visual feedback. We also implemented serial communication for keyboard control. The purpose of this project is to demonstrate embedded systems concepts we learned in class: GPIO configuration, interrupts, and assembly programming. While this is a prototype, we envision it could be scaled into a battery-powered device similar to a TV remote, with customizable buttons for each patient's specific needs."

### Slide 2: LED Setup Process

"Let's start with how we enabled the LEDs. We use three LEDs: green on pin PTE6 and red on pin PTC9. The setup process involves four main steps. First, we enable the clock for each port by setting specific bits in the SIM_SCGC5 register - bit 13 for Port E and bit 11 for Port C. Second, we configure each pin to GPIO mode by writing to the PORT_PCR register. Third, we set each pin as an output by configuring the GPIO_PDDR register. Finally, we initialize the LEDs to the OFF state. We implemented this in assembly language for direct register access, which provides efficient hardware control. The code is in the `gpio_led.s` file, specifically the `setup_green()` and `setup_red()` functions."

### Slide 3: Assembly LED Control Functions

"Here are the key assembly functions we wrote for LED control. We have setup functions, and then on/off functions for each LED. These functions work by reading the current GPIO port output register value, modifying only the specific bit we need using BIC to clear or ORR to set, then writing the value back. This read-modify-write approach ensures we don't affect other pins on the same port. The LEDs use active-low logic, meaning writing a 0 turns the LED on and writing a 1 turns it off. This is a common design in embedded systems. All these functions follow ARM calling conventions, using registers R0-R3 and properly saving the link register."

### Slide 4: PIT Timer Setup

"To make the LEDs flash during an active alert, we use the Periodic Interrupt Timer, or PIT. We configured it to generate interrupts every 500 milliseconds. The setup involves getting the default PIT configuration, initializing the peripheral, setting the timer period to 500,000 microseconds, enabling timer interrupts, and finally enabling the interrupt in the NVIC. When the timer counts down to zero, it generates an interrupt, which triggers our handler to toggle the LED state. This allows the CPU to do other work between LED toggles, rather than being stuck in a delay loop."

### Slide 5: Button Setup Process

"Now let's look at how we enabled the buttons. We have SW2 on pin PTD11 for water requests and SW3 on pin PTA10 for washroom requests. The setup process involves five steps. First, we enable the clock for each port. Second, we configure the pin settings including pull-up resistors and GPIO mode. Third, we set the interrupt type to falling-edge detection, which means the interrupt triggers when the button is pressed and the signal goes from high to low. Fourth, we initialize the GPIO pin as a digital input. Finally, we enable the interrupt in the NVIC. Using interrupts instead of polling means the CPU doesn't waste cycles checking button states - it responds instantly when a button is pressed."

### Slide 6: Interrupt Service Routines

"We have four interrupt service routines in our system. The PORTD handler responds to SW2 button presses, the PORTA handler responds to SW3 button presses, the UART0 handler responds to keyboard input, and the PIT0 handler manages the LED flashing timer. Each handler follows the same pattern: first, clear the interrupt flag to prevent re-triggering, then perform the necessary action - either reading a character, calling an alert function, or toggling the LED. Clearing the interrupt flag is critical - if we forget this step, the system can get stuck in an infinite interrupt loop. The button and keyboard handlers call shared functions that manage the state machine, while the PIT handler directly toggles the LED state based on the current system state."

### Slide 7: State Machine Logic

"Our system uses a state machine to manage three states: IDLE when no alert is active, WATER_ALERT when a water request is active, and WASHROOM_ALERT when a washroom request is active. The state transitions are straightforward: from IDLE, pressing a button moves to the corresponding alert state and starts LED flashing. From an alert state, pressing the same button again returns to IDLE and stops the LED. If a different button is pressed while an alert is active, the current alert is cancelled and the new one starts. This ensures only one alert is active at a time. The state machine logic is implemented in the `handle_water_alert()` and `handle_washroom_alert()` functions, which are called from both button interrupts and keyboard input."

### Slide 8: UART Serial Communication

"We implemented bidirectional UART communication at 115200 baud. This serves two purposes: sending debug messages to the computer and receiving keyboard commands. We use interrupts for keyboard input, just like we do for the buttons. When a character arrives at the UART, the hardware generates an interrupt, which triggers our UART interrupt handler. The handler reads the character and processes it. If the character is 'W' or 'w', we trigger a water alert. If it's 'T' or 't', we trigger a washroom alert. We could have used polling for keyboard input, but that would mean the CPU was constantly scanning for new characters, which wastes CPU cycles. Using interrupts allows the CPU to sleep until data actually arrives, making the system more efficient. This demonstrates bidirectional communication - we can both send information out and receive commands in. The UART allows us to control the system from a computer terminal, which is useful for testing and demonstrates the flexibility of our design."

"Let me give you an overview of how all these components work together. On the hardware side, we have GPIO for buttons and LEDs, the PIT timer, and UART for communication. On the software side, we have C code handling the main application, state machine, and interrupt handlers, plus over 100 lines of assembly code for direct LED control. The data flow works like this: when a button is pressed, a GPIO interrupt triggers, which calls a handler that changes the system state. When keyboard input arrives, a UART interrupt triggers, which also calls a handler that changes the system state. The state change then controls the LEDs through our assembly functions. Meanwhile, the PIT timer generates interrupts every 500 milliseconds to toggle the LED states, creating the flashing effect. The key design principle here is that everything is fully interrupt-driven - we don't use polling for any input, which makes the system efficient and responsive. The main loop simply puts the CPU to sleep, and it wakes up only when an interrupt occurs."

### Slide 10: Conclusion

"In conclusion, we successfully built a functional proof of concept for an assistive communication device. We demonstrated GPIO configuration and control, interrupt-driven input handling, assembly language programming with over 100 lines of code, state machine design, and bidirectional UART communication. We met all course requirements including GPIO interrupt handlers, bidirectional UART, state machine logic, and extensive assembly programming. This project shows how embedded systems concepts can be applied to build functional devices. The code is available on our GitHub repository with detailed comments. Thank you for your attention, and we're happy to answer any questions."
