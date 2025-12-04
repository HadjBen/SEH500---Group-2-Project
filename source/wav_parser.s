// WAV File Header Parser Assembly Functions
// reading and parsing WAV file headers from SD card
.syntax unified
.cpu cortex-m4
.thumb
.text

// Structure offsets for WAV file header (44 bytes total)
.equ WAV_RIFF_OFFSET, 0
.equ WAV_FILE_SIZE_OFFSET, 4
.equ WAV_WAVE_OFFSET, 8
.equ WAV_FMT_OFFSET, 12
.equ WAV_FMT_SIZE_OFFSET, 16
.equ WAV_AUDIO_FORMAT_OFFSET, 20
.equ WAV_NUM_CHANNELS_OFFSET, 22
.equ WAV_SAMPLE_RATE_OFFSET, 24
.equ WAV_BYTE_RATE_OFFSET, 28
.equ WAV_BLOCK_ALIGN_OFFSET, 32
.equ WAV_BITS_PER_SAMPLE_OFFSET, 34
.equ WAV_DATA_OFFSET, 36
.equ WAV_DATA_SIZE_OFFSET, 40

// parse_wav_header(buffer_addr, wav_info_addr)
// r0 = buffer address (SD card read buffer)
// r1 = wav_info structure address (output)
// Returns: r0 = 0 if valid, -1 if invalid
.global parse_wav_header
.type parse_wav_header, %function
parse_wav_header:
    push {r4-r7, lr}
    
    // Save parameters
    mov r4, r0              // r4 = buffer address
    mov r5, r1              // r5 = wav_info structure address
    
    // Check "RIFF" signature (bytes 0-3)
    ldr r0, [r4, #WAV_RIFF_OFFSET]
    ldr r1, =0x46464952     // "RIFF" in little-endian
    cmp r0, r1
    bne parse_wav_invalid
    
    // Check "WAVE" signature (bytes 8-11)
    ldr r0, [r4, #WAV_WAVE_OFFSET]
    ldr r1, =0x45564157     // "WAVE" in little-endian
    cmp r0, r1
    bne parse_wav_invalid
    
    // Check "fmt " signature (bytes 12-15)
    ldr r0, [r4, #WAV_FMT_OFFSET]
    ldr r1, =0x20746D66     // "fmt " in little-endian
    cmp r0, r1
    bne parse_wav_invalid
    
    // Check "data" signature (bytes 36-39)
    ldr r0, [r4, #WAV_DATA_OFFSET]
    ldr r1, =0x61746164     // "data" in little-endian
    cmp r0, r1
    bne parse_wav_invalid
    
    // Extract audio format (16-bit, offset 20)
    ldrh r0, [r4, #WAV_AUDIO_FORMAT_OFFSET]
    strh r0, [r5, #0]       // Store in wav_info.audioFormat
    
    // Extract number of channels (16-bit, offset 22)
    ldrh r0, [r4, #WAV_NUM_CHANNELS_OFFSET]
    strh r0, [r5, #2]       // Store in wav_info.numChannels
    
    // Extract sample rate (32-bit, offset 24)
    ldr r0, [r4, #WAV_SAMPLE_RATE_OFFSET]
    str r0, [r5, #4]        // Store in wav_info.sampleRate
    
    // Extract byte rate (32-bit, offset 28)
    ldr r0, [r4, #WAV_BYTE_RATE_OFFSET]
    str r0, [r5, #8]        // Store in wav_info.byteRate
    
    // Extract block align (16-bit, offset 32)
    ldrh r0, [r4, #WAV_BLOCK_ALIGN_OFFSET]
    strh r0, [r5, #12]      // Store in wav_info.blockAlign
    
    // Extract bits per sample (16-bit, offset 34)
    ldrh r0, [r4, #WAV_BITS_PER_SAMPLE_OFFSET]
    strh r0, [r5, #14]      // Store in wav_info.bitsPerSample
    
    // Extract data size (32-bit, offset 40)
    ldr r0, [r4, #WAV_DATA_SIZE_OFFSET]
    str r0, [r5, #16]       // Store in wav_info.dataSize
    
    // Calculate data start offset (always 44 for standard WAV)
    mov r0, #44
    str r0, [r5, #20]       // Store in wav_info.dataOffset
    
    // Validate audio format (should be 1 for PCM)
    ldrh r0, [r5, #0]
    cmp r0, #1
    bne parse_wav_invalid
    
    // Validate channels (should be 1 or 2)
    ldrh r0, [r5, #2]
    cmp r0, #0
    beq parse_wav_invalid
    cmp r0, #3
    bgt parse_wav_invalid
    
    // Validate sample rate (should be > 0)
    ldr r0, [r5, #4]
    cmp r0, #0
    beq parse_wav_invalid
    
    // Validate bits per sample (should be 8, 16, or 24)
    ldrh r0, [r5, #14]
    cmp r0, #8
    beq parse_wav_valid
    cmp r0, #16
    beq parse_wav_valid
    cmp r0, #24
    beq parse_wav_valid
    b parse_wav_invalid
    
parse_wav_valid:
    mov r0, #0              // Return 0 = success
    pop {r4-r7, pc}
    
parse_wav_invalid:
    mov r0, #-1             // Return -1 = invalid
    pop {r4-r7, pc}

// calculate_audio_duration(wav_info_addr)
// r0 = wav_info structure address
// Returns: r0 = duration in milliseconds
.global calculate_audio_duration
.type calculate_audio_duration, %function
calculate_audio_duration:
    push {r4-r5, lr}
    
    mov r4, r0              // r4 = wav_info address
    
    // Load dataSize and byteRate
    ldr r0, [r4, #16]       // dataSize
    ldr r1, [r4, #8]        // byteRate
    
    // Duration (ms) = (dataSize * 1000) / byteRate
    // Multiply dataSize by 1000
    mov r2, #1000
    mul r0, r0, r2
    
    // Divide by byteRate
    cmp r1, #0
    beq calc_duration_zero
    udiv r0, r0, r1
    
    pop {r4-r5, pc}
    
calc_duration_zero:
    mov r0, #0
    pop {r4-r5, pc}

// validate_wav_format(wav_info_addr, required_sample_rate, required_channels)
// r0 = wav_info structure address
// r1 = required sample rate
// r2 = required channels
// Returns: r0 = 0 if match, -1 if no match
.global validate_wav_format
.type validate_wav_format, %function
validate_wav_format:
    push {r4-r5, lr}
    
    mov r4, r0              // r4 = wav_info address
    mov r5, r1              // r5 = required sample rate
    
    // Check sample rate
    ldr r0, [r4, #4]        // wav_info.sampleRate
    cmp r0, r5
    bne validate_fail
    
    // Check channels
    ldrh r0, [r4, #2]       // wav_info.numChannels
    cmp r0, r2
    bne validate_fail
    
    // Check bits per sample (should be 16)
    ldrh r0, [r4, #14]      // wav_info.bitsPerSample
    cmp r0, #16
    bne validate_fail
    
    mov r0, #0              // Success
    pop {r4-r5, pc}
    
validate_fail:
    mov r0, #-1             // Fail
    pop {r4-r5, pc}

