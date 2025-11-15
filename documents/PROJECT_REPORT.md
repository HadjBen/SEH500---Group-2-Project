# SEH500 Project Report: Assistive Audio-Visual Communicator

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Project Overview](#project-overview)
3. [System Architecture](#system-architecture)
4. [Hardware Configuration](#hardware-configuration)
5. [Software Implementation](#software-implementation)
6. [Development Process](#development-process)
7. [Technical Challenges and Solutions](#technical-challenges-and-solutions)
8. [Testing and Validation](#testing-and-validation)
9. [Conclusion](#conclusion)
10. [Appendices](#appendices)

---

## Executive Summary

This report documents the development of an assistive communication device designed for non-verbal patients to communicate basic needs to caregivers. The system utilizes three primary peripherals: serial communication (UART), general-purpose input/output (GPIO) for buttons and LEDs, and an SD card interface for audio playback. The implementation successfully integrates hardware interrupts, state machine logic, and audio playback functionality to create a reliable communication interface.

The project was developed on the FRDM-K66F microcontroller board using MCUXpresso IDE and the NXP SDK. Key achievements include the implementation of GPIO interrupt handlers, bidirectional UART communication, LED control via assembly language, and SD card-based audio playback with WAV file parsing. The system demonstrates proficiency in embedded systems programming, peripheral interfacing, and real-time system design.

---

## Project Overview

### Objective

The primary objective of this project is to develop an assistive communication device that enables non-verbal patients to signal two basic needs: water and washroom assistance. The device must provide both visual and auditory feedback to ensure caregivers are promptly notified of patient requests. The system operates autonomously, requiring minimal patient interaction while maintaining reliability and ease of use.

### Functional Requirements

The system implements a state machine with three primary states: idle, water alert, and washroom alert. When a patient presses a button corresponding to their need, the system enters the appropriate alert state. During an alert state, a color-coded LED flashes continuously, and a pre-recorded audio message plays from an SD card. The audio message repeats every ten seconds until a caregiver presses the same button to acknowledge and cancel the alert.

The device supports two input methods: physical buttons mounted on the board and keyboard commands sent via serial communication. This dual-input approach provides flexibility for testing and demonstrates bidirectional serial communication capabilities. The visual feedback uses onboard RGB LEDs, with green indicating water requests and red indicating washroom requests.

### Course Requirements Fulfillment

The project successfully fulfills all course requirements. Over one hundred lines of GNU assembly language code were written for WAV file header parsing and LED control functions. GPIO interrupts are implemented for button press detection, providing responsive user interaction. Three distinct peripherals are utilized: UART for serial communication, GPIO for buttons and LEDs, and SD card interface for audio file storage and playback.

---

## System Architecture

### Hardware Platform

The project is implemented on the FRDM-K66F development board, which features an NXP MK66FN2M0VMD18 microcontroller. This microcontroller is based on the ARM Cortex-M4 core, operating at 180 MHz, and provides extensive peripheral support including multiple UART interfaces, GPIO ports, timer modules, and audio interfaces.

The board includes onboard RGB LEDs, two user buttons (SW2 and SW3), and a debug UART interface connected to a USB-to-serial converter. For audio playback, the system utilizes the Serial Audio Interface (SAI) peripheral, which interfaces with an external audio codec. The SD card interface uses the SD Host Controller (SDHC) peripheral, allowing direct connection to standard SD cards.

### Software Architecture

The software architecture follows a modular design, separating hardware abstraction, application logic, and interrupt handling. The main application loop manages system initialization and provides a non-blocking interface for serial communication. Interrupt service routines handle time-critical events such as button presses and timer expiration.

The state machine implementation ensures predictable system behavior and simplifies the management of concurrent operations. LED control and audio playback operate independently, coordinated through shared state variables. This design allows the system to respond immediately to user input while maintaining continuous visual and auditory feedback.

### Peripheral Integration

Three primary peripherals form the core of the system functionality. The UART peripheral handles bidirectional serial communication, enabling keyboard input from a connected computer and debug output to a serial terminal. The GPIO peripheral manages button input detection and LED output control, with interrupt-driven button handling for immediate response. The SD card interface, implemented through the SDHC peripheral and FatFS filesystem, provides access to audio files stored on removable media.

The integration of these peripherals requires careful coordination of interrupt priorities and resource sharing. The system uses the Periodic Interrupt Timer (PIT) to generate periodic events for LED flickering and audio playback scheduling. Enhanced Direct Memory Access (eDMA) controllers facilitate efficient audio data transfer from SD card to audio interface without CPU intervention.

---

## Hardware Configuration

### Pin Assignments

The system utilizes specific pins on the microcontroller for each peripheral function. Button SW2 is connected to Port D, Pin 11 (PTD11), configured as a GPIO input with pull-up resistor and falling-edge interrupt detection. Button SW3 is connected to Port A, Pin 10 (PTA10), with identical configuration. These pins are physically located on the board and provide tactile feedback when pressed.

The onboard RGB LED system uses multiple pins: the green LED is connected to Port E, Pin 6 (PTE6), and the red LED is connected to Port C, Pin 9 (PTC9). Both LEDs are configured as GPIO outputs, with active-low logic requiring the pin to be driven low to illuminate the LED.

The debug UART interface uses UART0, with transmit and receive pins connected to the onboard USB-to-serial converter. This interface operates at 115200 baud rate, providing sufficient bandwidth for debug messages and keyboard input. The UART configuration includes standard 8-bit data, no parity, and one stop bit.

### SD Card Interface

The SD card interface utilizes the SDHC peripheral, which provides a high-speed interface compliant with the SD card specification. The interface supports standard SD cards and SDHC cards up to 32 gigabytes. Physical connection is made through a card slot on the development board, with power, ground, clock, command, and data lines properly routed.

Card detection is implemented using a GPIO pin that monitors the physical presence of the card. The system polls this pin during initialization to ensure a card is present before attempting to access the filesystem. This approach prevents errors when no card is inserted and provides graceful handling of card removal during operation.

### Audio Interface

Audio playback is accomplished through the Serial Audio Interface (SAI) peripheral, which implements the I2S protocol for digital audio transmission. The SAI interface connects to an external audio codec, which converts digital audio data to analog signals for speaker output. The codec is controlled via I2C interface, allowing configuration of volume, sample rate, and audio format.

The audio system supports standard WAV file formats with PCM encoding. Sample rates up to 48 kHz are supported, with 16-bit sample depth and stereo or mono channel configurations. The DMA controller transfers audio data from memory to the SAI peripheral, enabling continuous playback without CPU intervention for each sample.

---

## Software Implementation

### Main Application Structure

The main application function initializes all system components in a specific order to ensure proper hardware configuration. Clock systems are configured first, providing the necessary timing references for all peripherals. Board-specific pin configurations are applied next, establishing the physical connections between the microcontroller and external components.

Following hardware initialization, the application configures the debug console, enabling serial communication for development and testing. LED control functions are initialized through assembly language routines, setting up the GPIO pins for output operation. The periodic interrupt timer is configured with a 500-millisecond period, generating interrupts for LED flickering control.

Button interrupt configuration completes the initialization sequence. The system enables interrupts for both button ports and configures the interrupt controller to route these events to the appropriate handler functions. Once initialization is complete, the main function enters an infinite loop that monitors serial communication for keyboard input while interrupt handlers manage all time-critical operations.

### Interrupt Service Routines

The system implements three interrupt service routines to handle asynchronous events. The PORTD interrupt handler responds to button SW2 presses, which trigger water alert requests. When this interrupt occurs, the handler immediately clears the interrupt flag to prevent multiple triggers from a single button press. The handler then calls the water alert management function, which updates the system state and initiates visual and auditory feedback.

The PORTA interrupt handler operates identically for button SW3, triggering washroom alert requests. Both handlers include debouncing logic through hardware pull-up resistors and software flag clearing, ensuring reliable button press detection without false triggers from electrical noise or contact bounce.

The PIT interrupt handler executes every 500 milliseconds, providing timing for LED flickering and audio playback scheduling. When an alert is active, the handler toggles the appropriate LED state, creating a visible flashing pattern. Additionally, the handler maintains a counter that tracks elapsed time, triggering audio playback every ten seconds when an alert state is active.

### State Machine Implementation

The system state machine manages three distinct states: idle, water alert, and washroom alert. State transitions occur only in response to user input, ensuring predictable behavior. When the system is in the idle state, pressing either button transitions to the corresponding alert state. When an alert is active, pressing the same button again returns the system to the idle state and cancels all active alerts.

State management functions handle the complexity of state transitions, including cleanup of previous states and initialization of new states. When transitioning to an alert state, the function first checks if a different alert is currently active. If so, it cancels the previous alert before starting the new one, ensuring only one alert is active at any time.

The state machine design simplifies the implementation of cancellation logic. Since the same button that starts an alert also cancels it, the state transition function can determine the required action by examining the current state. This approach eliminates the need for separate cancel functions and reduces code complexity.

### Assembly Language Implementation

The project includes over one hundred lines of assembly language code, fulfilling the course requirement for low-level programming. The assembly code is divided into two primary modules: LED control functions and WAV file parsing functions.

The LED control module provides functions for configuring GPIO pins and controlling LED states. These functions directly manipulate hardware registers, providing precise control over pin configuration and output states. The implementation uses read-modify-write operations to ensure that changes to one pin do not affect other pins on the same port.

The WAV file parsing module implements functions for reading and interpreting WAV file headers. These functions extract critical audio parameters such as sample rate, bit depth, channel count, and data size from the file header. The parsing functions validate the file format, ensuring that only valid WAV files are processed. This validation prevents system errors from corrupted or incorrectly formatted audio files.

The assembly implementation provides direct access to memory-mapped registers, enabling efficient data manipulation without the overhead of C function calls for simple operations. This approach is particularly beneficial for time-critical operations such as GPIO control and file header parsing, where execution speed is important.

### SD Card and Filesystem Integration

The SD card interface is implemented using the FatFS filesystem, which provides a standard file access interface compatible with FAT32-formatted SD cards. The filesystem driver abstracts the low-level SD card communication, allowing the application to use standard file operations such as open, read, and close.

Initialization of the SD card interface involves multiple steps. First, the SDHC peripheral is configured with appropriate clock settings and transfer parameters. The physical card is detected through GPIO polling, confirming that a card is present before attempting communication. The card is then initialized through a sequence of commands defined in the SD card specification, establishing communication and determining card capabilities.

Once the card is initialized, the FatFS filesystem is mounted, making the card's file structure accessible to the application. The system searches for audio files in the root directory, locating files named "water.wav" and "restroom.wav". These files must be present on the card for the system to function correctly, and the application provides error messages if files are not found.

### Audio Playback Implementation

Audio playback begins when an alert state is activated. The system first opens the appropriate WAV file from the SD card and reads the file header. The header parsing function, implemented in assembly language, extracts audio format information including sample rate, bit depth, and channel configuration.

Using the parsed header information, the application configures the SAI peripheral to match the audio file's format. This configuration ensures that the audio interface operates at the correct sample rate and data format, preventing distortion or incorrect playback speed. The DMA controller is configured to transfer audio data from the SD card buffer to the SAI peripheral, enabling continuous playback.

The audio playback process operates in a streaming manner, reading data from the SD card in blocks and transferring it to the audio interface via DMA. This approach minimizes memory requirements while ensuring smooth playback. The system monitors the DMA transfer progress, reading additional data blocks as needed to maintain continuous playback.

Playback continues until the alert is cancelled or the audio file completes. When an alert is cancelled, any ongoing audio transfer is stopped, and the audio interface is reset to a quiet state. The ten-second repeat functionality is implemented through the PIT interrupt handler, which triggers a new playback sequence when the timer expires and an alert state is active.

### WAV File Parsing Process

The WAV file parsing process begins when a file is opened for playback. The system reads the first forty-four bytes of the file, which contain the standard WAV file header structure. This header includes multiple fields that must be parsed and validated to ensure proper audio playback.

The parsing function first verifies the file format by checking for the "RIFF" identifier at the beginning of the file. This identifier confirms that the file follows the Resource Interchange File Format structure. The function then checks for the "WAVE" identifier, which specifically identifies the file as a WAV audio file.

The format chunk contains critical audio parameters. The audio format field indicates the encoding type, with a value of one indicating uncompressed PCM audio. The number of channels field specifies whether the audio is mono or stereo, affecting how the data is interpreted. The sample rate determines the playback speed, and incorrect interpretation results in audio that plays too fast or too slow.

The bit depth parameter specifies the number of bits per sample, typically sixteen bits for standard audio files. This parameter affects the data interpretation and must match the SAI configuration for correct playback. The block align value indicates how samples are grouped in the data stream, important for multi-channel audio.

The data chunk header identifies where the actual audio samples begin in the file. The data size field specifies the total number of bytes of audio data, allowing the system to calculate playback duration and allocate appropriate buffer sizes. The parsing function extracts all these parameters and stores them in a structure that the playback function uses for configuration.

Validation during parsing ensures that the file format is correct and compatible with the system's capabilities. If any required field is missing or contains invalid data, the parsing function returns an error, preventing playback of corrupted files. This validation protects the system from errors that could occur from attempting to play incompatible audio formats.

---

## Development Process

### Initial Planning and Design

The development process began with a thorough analysis of the project requirements and available hardware resources. The FRDM-K66F board was selected for its comprehensive peripheral support and development tool availability. Initial design focused on identifying the required peripherals and understanding their configuration requirements.

The system architecture was designed with modularity in mind, separating hardware abstraction from application logic. This design approach facilitated incremental development, allowing each component to be implemented and tested independently before integration. The state machine design was developed early in the process, providing a clear framework for system behavior.

Component selection involved evaluating multiple approaches for each system function. For audio playback, the SAI interface was chosen over alternative methods due to its direct support for I2S audio protocols and DMA integration capabilities. The FatFS filesystem was selected for SD card access due to its widespread use and compatibility with standard file formats.

### Driver Selection and Installation

The project utilizes drivers from the NXP SDK, which provides standardized interfaces for peripheral access. Key drivers include the GPIO driver for button and LED control, the UART driver for serial communication, the SDHC driver for SD card interface, and the SAI driver for audio playback. Additional drivers for DMA and interrupt management are also required.

Driver installation is handled automatically by MCUXpresso IDE when creating a new project with the appropriate SDK components selected. The IDE manages driver dependencies, ensuring that all required components are included. For this project, drivers for GPIO, UART, PIT, SDHC, SAI, eDMA, and DMAMUX were selected during project creation.

The SDK drivers provide abstraction layers that simplify peripheral configuration and operation. However, some low-level operations require direct register access, which is accomplished through assembly language or direct register manipulation in C code. This combination of driver functions and direct register access provides both convenience and control.

### Configuration Tools Versus Manual Implementation

During development, a decision was made to implement certain functions manually rather than relying solely on MCUXpresso ConfigTools. This decision was driven by several factors, including the need for precise control over hardware behavior and the requirement for assembly language implementation.

The ConfigTools provide a graphical interface for peripheral configuration, generating initialization code automatically. While this approach simplifies initial setup, it can limit flexibility for advanced configurations. For this project, manual implementation was chosen for GPIO interrupt configuration to ensure precise control over interrupt triggering and flag management.

Manual implementation also facilitated the integration of assembly language code, which requires direct register access. The LED control functions and WAV parsing functions are implemented entirely in assembly, providing direct hardware control without the overhead of function call mechanisms. This approach meets the course requirement for assembly language programming while optimizing performance for time-critical operations.

The button interrupt configuration was implemented manually to ensure proper debouncing and flag clearing behavior. While ConfigTools can generate basic interrupt configuration, the manual approach allowed for custom interrupt handler implementation and precise control over the interrupt priority and enable settings.

### Incremental Development Approach

Development proceeded incrementally, with each phase building upon previous work. The first phase focused on basic GPIO functionality, implementing LED control and button input. This phase established the foundation for all subsequent development, providing working hardware interfaces that could be extended with additional functionality.

The second phase added interrupt handling, enabling responsive button press detection. This phase required careful attention to interrupt configuration and handler implementation to ensure reliable operation. Testing during this phase revealed the importance of proper interrupt flag management and debouncing techniques.

Serial communication was added in the third phase, providing debugging capabilities and keyboard input functionality. This phase demonstrated bidirectional UART operation and established the communication interface used throughout the remainder of development. The keyboard input feature was added to fulfill the requirement for bidirectional serial communication.

Audio playback implementation constituted the final major phase, integrating SD card access, filesystem operations, and audio interface configuration. This phase required coordination of multiple peripherals and careful management of data flow from SD card to audio output. The WAV parsing functionality was implemented during this phase, providing the required assembly language code.

### Testing Methodology

Testing was conducted at each development phase to ensure functionality before proceeding to the next phase. Initial testing focused on basic hardware operation, verifying that LEDs could be controlled and buttons could be read. This testing used simple test programs that exercised individual components in isolation.

Interrupt testing required more sophisticated test procedures, as interrupt behavior can be difficult to observe directly. Serial communication was used extensively during interrupt testing to provide visibility into interrupt handler execution. Print statements within interrupt handlers confirmed proper triggering and execution timing.

Audio playback testing involved multiple steps, beginning with SD card detection and filesystem access. Once file access was confirmed, audio playback was tested with various WAV file formats to ensure compatibility. Playback quality was verified through direct audio output observation, confirming that audio was played correctly without distortion or timing issues.

Integration testing verified that all system components functioned together correctly. This testing included scenarios such as button presses during audio playback, rapid state changes, and error conditions such as missing audio files. The state machine behavior was verified through systematic testing of all possible state transitions.

---

## Technical Challenges and Solutions

### Interrupt Configuration Challenges

One of the initial challenges encountered during development was proper configuration of GPIO interrupts. The interrupt system requires coordination between multiple configuration steps: port pin configuration, GPIO pin setup, interrupt type selection, and NVIC interrupt enable. Missing any of these steps results in non-functional interrupts.

The solution involved careful review of the SDK documentation and example code to understand the complete interrupt configuration sequence. A systematic approach was developed, ensuring that each configuration step was performed in the correct order. Debugging was facilitated through serial output, which provided visibility into interrupt handler execution.

Another challenge related to interrupt flag management. GPIO interrupt flags must be cleared within the interrupt handler to prevent repeated triggering. However, clearing flags prematurely can cause missed interrupts, while clearing them incorrectly can prevent proper operation. The solution involved clearing flags immediately upon handler entry, before any other operations that might delay flag clearing.

### SD Card Initialization Issues

SD card initialization presented several challenges related to card detection and communication establishment. The initial implementation attempted to use automatic card detection through the SDHC peripheral's built-in detection mechanism. However, this approach proved unreliable, with the system sometimes failing to detect inserted cards.

The solution involved implementing manual card detection using a GPIO pin that monitors the physical card presence. This approach provides more reliable detection and allows the system to handle card insertion and removal gracefully. The initialization sequence was modified to poll this GPIO pin before attempting card communication.

Card communication establishment required careful timing and command sequencing. The SD card specification defines a specific initialization sequence that must be followed precisely. Deviations from this sequence result in communication failures. The solution involved implementing the exact sequence specified in the SD card documentation, with appropriate delays between commands to ensure proper card response timing.

### Audio Playback Synchronization

Achieving smooth audio playback required careful coordination between SD card reading, DMA transfers, and audio interface timing. Initial implementations experienced audio dropouts and stuttering due to insufficient buffering and improper DMA configuration. The audio interface requires continuous data flow, and any interruption results in audible artifacts.

The solution involved implementing a double-buffering scheme, where one buffer is being played while the other is being filled from the SD card. This approach ensures that data is always available for playback, preventing dropouts. The DMA controller was configured to generate interrupts when transfers complete, triggering the filling of the next buffer.

Timing coordination was achieved through careful calculation of buffer sizes relative to the audio sample rate and DMA transfer capabilities. The buffer size must be large enough to prevent underruns while small enough to maintain reasonable memory usage. The final implementation uses buffers sized to hold approximately one second of audio data, providing a balance between reliability and memory efficiency.

### Assembly Language Integration

Integrating assembly language code with C code required careful attention to calling conventions and register usage. The ARM Cortex-M4 calling convention specifies which registers must be preserved across function calls, and assembly functions must adhere to these conventions to prevent corruption of C code execution.

The solution involved implementing proper function prologues and epilogues in assembly code, saving and restoring registers as required by the calling convention. The assembly functions were designed to accept parameters and return values according to standard conventions, ensuring seamless integration with C code.

Debugging assembly code presented additional challenges, as standard debugging tools provide limited visibility into assembly execution. The solution involved adding strategic breakpoints and using serial output to trace execution flow. Careful code organization and commenting helped maintain code clarity despite the complexity of assembly language programming.

### State Machine Race Conditions

The state machine implementation initially experienced race conditions when rapid button presses occurred. These conditions arose from the asynchronous nature of interrupt handlers modifying shared state variables while the main application loop might also be accessing these variables. Without proper synchronization, state transitions could be lost or incorrect states could be entered.

The solution involved using volatile qualifiers for shared state variables and ensuring that state modifications occur only within interrupt handlers or within critical sections protected by interrupt disabling. The state machine design was simplified to minimize the number of shared variables and reduce the potential for race conditions.

Careful ordering of operations within state transition functions ensures that state changes are atomic from the perspective of other code. The cancellation logic was designed to be idempotent, meaning that multiple cancellation attempts have the same effect as a single cancellation, preventing issues from rapid repeated button presses.

---

## Testing and Validation

### Functional Testing

Functional testing verified that all system requirements were met. Button press testing confirmed that both physical buttons and keyboard commands correctly trigger alert states. LED testing verified that the correct LED activates for each alert type and that the flickering behavior operates as specified. Audio playback testing confirmed that the correct audio file plays for each alert type and that playback repeats at ten-second intervals.

State machine testing systematically verified all possible state transitions. Each transition was tested individually, confirming that the system enters the correct state and that all associated actions occur. Cancellation testing verified that alerts can be properly cancelled and that the system returns to the idle state correctly.

Integration testing combined all system functions to verify that they operate together without conflicts. Tests included scenarios such as pressing buttons during audio playback, rapid state changes, and simultaneous button and keyboard input. These tests confirmed that the system handles concurrent operations correctly and maintains stable operation under various conditions.

### Performance Validation

Performance testing verified that the system meets timing requirements. Button response time was measured to ensure that alerts activate within acceptable limits following button presses. Audio playback timing was verified to confirm that the ten-second repeat interval is accurate and that playback begins promptly after alert activation.

Memory usage was analyzed to ensure that the system operates within available resources. Buffer sizes for audio playback were optimized to balance memory usage with playback reliability. The final implementation uses approximately half of the available RAM, leaving sufficient resources for system operation and future enhancements.

Power consumption was not a primary concern for this application, as the device operates from a wall power supply. However, basic power measurements confirmed that the system operates within the board's power supply capabilities and does not require special power management considerations.

### Reliability Testing

Reliability testing involved extended operation periods to identify potential issues with long-term operation. The system was operated continuously for several hours, with periodic button presses and state changes. This testing confirmed that the system maintains stable operation without memory leaks or resource exhaustion.

Error condition testing verified that the system handles exceptional situations gracefully. Tests included scenarios such as missing audio files, SD card removal during operation, and corrupted file formats. The system was designed to detect these conditions and provide appropriate error messages without entering an unrecoverable state.

Stress testing involved rapid and repeated button presses to verify that the system handles high-frequency input correctly. These tests confirmed that interrupt handlers execute reliably even under high load conditions and that state transitions remain correct regardless of input timing.

---

## Conclusion

This project successfully demonstrates the integration of multiple microcontroller peripherals to create a functional assistive communication device. The implementation combines hardware interrupts, state machine logic, serial communication, and audio playback to provide a reliable and user-friendly interface for non-verbal patients.

The development process highlighted the importance of systematic design, incremental development, and thorough testing. The decision to implement certain functions manually rather than relying solely on configuration tools provided the flexibility needed to meet specific requirements while maintaining code clarity and performance.

The project successfully fulfills all course requirements, including the implementation of over one hundred lines of assembly language code, GPIO interrupt handling, and the utilization of three distinct peripherals. The system architecture provides a solid foundation for future enhancements and demonstrates proficiency in embedded systems development.

Key achievements include reliable interrupt handling, efficient audio playback from SD card storage, and a robust state machine implementation. The bidirectional serial communication capability provides valuable debugging and testing capabilities while demonstrating advanced peripheral usage.

The system is ready for deployment and testing in a clinical environment. Future enhancements could include additional alert types, wireless communication capabilities, or integration with caregiver notification systems. The modular design facilitates such enhancements while maintaining the core functionality of the current implementation.

---

## Appendices

### Appendix A: Pin Configuration Summary

| Function | Port | Pin | Configuration |
|----------|------|-----|---------------|
| Water Button | D | 11 | GPIO Input, Pull-up, Falling Edge Interrupt |
| Washroom Button | A | 10 | GPIO Input, Pull-up, Falling Edge Interrupt |
| Green LED | E | 6 | GPIO Output, Active Low |
| Red LED | C | 9 | GPIO Output, Active Low |
| UART TX | B | 17 | UART0 Transmit |
| UART RX | B | 16 | UART0 Receive |

### Appendix B: Driver Dependencies

The following SDK drivers are required for system operation:

- fsl_gpio: GPIO peripheral control
- fsl_port: Port pin configuration
- fsl_uart: UART communication
- fsl_pit: Periodic interrupt timer
- fsl_sdhc: SD card host controller
- fsl_sai: Serial audio interface
- fsl_sai_edma: SAI with DMA support
- fsl_edma: Enhanced DMA controller
- fsl_dmamux: DMA multiplexer
- fsl_i2c: I2C communication for audio codec
- fsl_clock: Clock management
- fsl_common: Common utilities

### Appendix C: File Structure

```
SEH500_Project/
├── source/
│   ├── SEH500_Project.c        Main application
│   ├── gpio_led.s              LED control assembly
│   ├── wav_parser.s             WAV parsing assembly
│   └── ffconf.h                 FatFS configuration
├── audio/
│   ├── water.wav                Water alert audio
│   └── restroom.wav             Washroom alert audio
├── drivers/                     SDK driver files
├── fatfs/                       FatFS filesystem
├── sdmmc/                       SD card interface
└── board/                       Board configuration
```

### Appendix D: State Machine Diagram

```
        [IDLE]
           |
    +------+------+
    |             |
[SW2/'W']    [SW3/'T']
    |             |
    v             v
[WATER_ALERT] [WASHROOM_ALERT]
    |             |
    +------+------+
           |
    [Same Button]
           |
           v
        [IDLE]
```

### Appendix E: WAV File Format

The WAV file header structure (44 bytes):

| Offset | Size | Field | Description |
|--------|------|-------|-------------|
| 0x00 | 4 | RIFF | File identifier |
| 0x04 | 4 | ChunkSize | File size - 8 |
| 0x08 | 4 | WAVE | Format identifier |
| 0x0C | 4 | fmt | Format chunk identifier |
| 0x10 | 4 | fmtSize | Format chunk size |
| 0x14 | 2 | audioFormat | Audio format (1 = PCM) |
| 0x16 | 2 | numChannels | Channel count |
| 0x18 | 4 | sampleRate | Samples per second |
| 0x1C | 4 | byteRate | Bytes per second |
| 0x20 | 2 | blockAlign | Block alignment |
| 0x22 | 2 | bitsPerSample | Bits per sample |
| 0x24 | 4 | data | Data chunk identifier |
| 0x28 | 4 | dataSize | Audio data size |

---

*End of Report*

