// GPIO and LED Control Assembly Functions
// Using onboard RGB LEDs on FRDM-K66F board
.syntax unified
.cpu cortex-m4
.thumb
.text

// setup_green() - Configures onboard Green LED pin PTE6
.global setup_green
.type setup_green, %function
setup_green:
    push {lr}
    // Enable clock for Port E
    ldr r1, =0x40048038     // SIM_SCGC5
    ldr r0, [r1]
    orr r0, r0, #(1<<13)    // Port E bit 13
    str r0, [r1]
    // Set PTE6 to GPIO mode
    ldr r1, =0x4004D018     // PORTE_PCR6
    mov r0, #0x0100
    str r0, [r1]
    // Set PTE6 to output mode
    ldr r1, =0x400FF114     // GPIOE_PDDR
    ldr r0, [r1]
    orr r0, r0, #(1<<6)
    str r0, [r1]
    // Set PTE6 output HIGH (LED OFF initially)
    ldr r1, =0x400FF100     // GPIOE_PDOR
    ldr r0, [r1]
    orr r0, r0, #(1<<6)
    str r0, [r1]
    pop {pc}

// setup_red() - Configures onboard Red LED pin PTC9
.global setup_red
.type setup_red, %function
setup_red:
    push {lr}
    // Enable clock for Port C
    ldr r1, =0x40048038     // SIM_SCGC5
    ldr r0, [r1]
    orr r0, r0, #(1<<11)    // Port C bit 11
    str r0, [r1]
    // Set PTC9 to GPIO mode
    ldr r1, =0x4004B024     // PORTC_PCR9
    mov r0, #0x0100
    str r0, [r1]
    // Set PTC9 to output mode
    ldr r1, =0x400FF094     // GPIOC_PDDR
    ldr r0, [r1]
    orr r0, r0, #(1<<9)
    str r0, [r1]
    // Set PTC9 output HIGH (LED OFF initially)
    ldr r1, =0x400FF080     // GPIOC_PDOR
    ldr r0, [r1]
    orr r0, r0, #(1<<9)
    str r0, [r1]
    pop {pc}

// func_green_led_on() - Turn on green LED
.global func_green_led_on
.type func_green_led_on, %function
func_green_led_on:
    ldr r1, =0x400FF100     // GPIOE_PDOR
    ldr r0, [r1]
    bic r0, r0, #(1<<6)     // Clear bit (LOW = LED ON)
    str r0, [r1]
    bx  lr

// func_green_led_off() - Turn off green LED
.global func_green_led_off
.type func_green_led_off, %function
func_green_led_off:
    ldr r1, =0x400FF100     // GPIOE_PDOR
    ldr r0, [r1]
    orr r0, r0, #(1<<6)     // Set bit (HIGH = LED OFF)
    str r0, [r1]
    bx  lr

// func_red_led_on() - Turn on red LED
.global func_red_led_on
.type func_red_led_on, %function
func_red_led_on:
    ldr r1, =0x400FF080     // GPIOC_PDOR
    ldr r0, [r1]
    bic r0, r0, #(1<<9)     // Clear bit (LOW = LED ON)
    str r0, [r1]
    bx  lr

// func_red_led_off() - Turn off red LED
.global func_red_led_off
.type func_red_led_off, %function
func_red_led_off:
    ldr r1, =0x400FF080     // GPIOC_PDOR
    ldr r0, [r1]
    orr r0, r0, #(1<<9)     // Set bit (HIGH = LED OFF)
    str r0, [r1]
    bx  lr

// setup_leds() - Master function to setup all LEDs
.global setup_leds
.type setup_leds, %function
setup_leds:
    push {lr}
    bl setup_green
    bl setup_red
    pop {pc}

