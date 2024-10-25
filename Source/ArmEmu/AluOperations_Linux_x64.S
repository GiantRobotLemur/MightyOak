; AluOperations_Linux_x64.asm
; author GiantRobotLemur@na-se.co.uk
; date 2023
; copyright This file is part of the Mighty Oak project which is released
; under LGPL 3 license. See LICENSE file at the repository root or go to
; https://github.com/GiantRobotLemur/MightyOak for full license details.

; **** NOT REFACTORED FOR LINUX CALLING CONVENTION ****

.code
;             BA9876543210
; x64 RFlags: VDITSZ?A?P?C
; ARMv2 PSR:  ????????NZCV

; Reads the Carry, oVerflow, Negative and Sign flags from RFlags and
; orders them in the bottom nibble of the DL register.
; Uses: AL, AH, DH and DL
MOVE_PSR_TO_DL macro
    seto %dl                ; Capture the V flag
    setc %dh                ; Capture the C flag
    setz %h                 ; Capture the Z flag
    sets %al                ; Capture the N flag
    shl 1,%dh               ; Move the C flag to the right position
    shl 2,%ah               ; Move the Z flag to the right position
    shl 3,%al               ; Move the N flag to the right position

    orb %dh,%dl             ; Merge the C and V flags.
    orb %ah,%al             ; Merge Z and N flags.
    orb %al,%dl             ; Merge all flags into DL.
endm

; As above, but using CH/CL instead of DH/DL
MOVE_PSR_TO_CL macro
    seto %cl                ; Capture the V flag
    setc %ch                ; Capture the C flag
    setz %ah                ; Capture the Z flag
    sets %al                ; Capture the N flag
    shl 1,%ch               ; Move the C flag to the right position
    shl 2,%ah               ; Move the Z flag to the right position
    shl 3,%al               ; Move the N flag to the right position

    or %ch,%cl              ; Merge the C and V flags.
    or %ah,%al              ; Merge Z and N flags.
    or %al,%cl              ; Merge all flags into DL.
endm

CAPTURE_AND_MERGE_LOGIC_PSR macro flagAddrReg
    setz %r11b              ; Capture the Z flag
    sets %r10b              ; Capture the N flag
    shl 2,%r11b             ; Move the Z flag to the right position
    shl 3,%r10b             ; Move the N flag to the right position
    or %r10b,%r11b          ; Merge the Z and N flags.

    movb [flagAddrReg],%r10b,   ; Load ARM flags to inherit C and V
    and 3,%r10b             ; Mask out old N and Z flags
    or %r10b,%r11b         ; Merge old and new flags.
    movb %r11b,[flagAddrReg]    ; Store the ARM-compatible flags
endm

UPDATE_LOGIC_PSR macro flagReg
    setz %r11b              ; Capture the Z flag
    sets %r10b              ; Capture the N flag
    shl 2,%r11b             ; Move the Z flag to the right position
    shl 3,%r10b             ; Move the N flag to the right position
    or %r10b,%r11b          ; Merge the Z and N flags.

    and 3,flagReg           ; Mask out old N and Z flags
    or %r11b,flagReg        ; Merge old and new flags.
endm

; uint32_t ALU_Add(uint32_t op1, uint32_t op2,
;                     StatusFlags[uint8_t] &flags)
ALU_Add proc
    add %edx,%ecx
    MOVE_PSR_TO_DL
    movb %dl,[r8]           ; Store the ARM-compatible flags
    mov %ecx,%eax           ; Return the operation result
    ret
ALU_Add endp

; uint32_t ALU_Sub(uint32_t op1, uint32_t op2,
;                     StatusFlags[uint8_t] &statusFlags)
ALU_Sub proc
    sub %edx,%ecx
    MOVE_PSR_TO_DL
    movb %dl,[r8]           ; Store the ARM-compatible flags
    mov %ecx,%eax           ; Return the operation result
    ret
ALU_Sub endp

; uint32_t ALU_Adc(uint32_t op1, uint32_t op2,
;                     StatusFlags[uint8_t] &statusFlags)
ALU_Adc proc
    movb [r8],%al           ; Load ARM status flags
    shl 7,%al               ; Shift bit 2 out to bit 8,
                            ; which will end up in RFlags.Carry
    adc %edx,%ecx           ; Perform the operation being emulated.
    MOVE_PSR_TO_DL
    movb %dl,[r8]           ; Store the ARM-compatible flags
    mov %ecx,%eax           ; Return the operation result
    ret
ALU_Adc endp

; uint32_t ALU_Sbc(uint32_t op1, uint32_t op2,
;                     StatusFlags[uint8_t] &statusFlags)
; result = op1 - (op2 + Carry)
ALU_Sbc proc
    movb [r8],%al           ; Load ARM status flags
    shl 7,%al               ; Shift bit 2 out to bit 8,
                            ; which will end up in RFlags.Carry
    sbb %edx,%ecx           ; Perform the operation being emulated.
    MOVE_PSR_TO_DL
    movb %dl,[r8]           ; Store the ARM-compatible flags
    mov %ecx,%eax           ; Return the operation result
    ret
ALU_Sbc endp

; uint32_t ALU_Rsc(uint32_t op1, uint32_t op2,
;                     StatusFlags[uint8_t] &statusFlags)
; result = op2 - (op1 + Carry)
ALU_Rsc proc
    movb [r8],%al           ; Load ARM status flags
    shl 7,%al               ; Shift bit 2 out to bit 8,
                            ; which will end up in RFlags.Carry
    sbb %ecx,%edx           ; Perform the operation being emulated.
    MOVE_PSR_TO_CL
    movb %cl,[r8]           ; Store the ARM-compatible flags
    mov %edx,%eax           ; Return the operation result
    ret
ALU_Rsc endp

; uint32_t ALU_And(uint32_t op1, uint32_t op2,
;                     StatusFlags[uint8_t] &statusFlags)
ALU_And proc
    and %edx,%ecx
    CAPTURE_AND_MERGE_LOGIC_PSR %r8
    mov %ecx,%eax            ; Return the operation result
    ret
ALU_And endp

; uint32_t ALU_Or(uint32_t op1, uint32_t op2,
;                    StatusFlags[uint8_t] &statusFlags)
ALU_Or proc
    or %edx,%ecx
    CAPTURE_AND_MERGE_LOGIC_PSR %r8
    mov %ecx,%eax           ; Return the operation result
    ret
ALU_Or endp

; uint32_t ALU_Xor(uint32_t op1, uint32_t op2,
;                     StatusFlags[uint8_t] &statusFlags)
ALU_Xor proc
    xor %edx,%ecx
    CAPTURE_AND_MERGE_LOGIC_PSR %r8
    mov %ecx,%eax           ; Return the operation result
    ret
ALU_Xor endp

; uint32_t ALU_Bic(uint32_t op1, uint32_t op2,
;                     StatusFlags[uint8_t] &statusFlags)
; result = op1 & (~op2)
ALU_Bic proc
    not %edx
    and %edx,%ecx
    CAPTURE_AND_MERGE_LOGIC_PSR %r8
    mov %ecx,%eax           ; Return the operation result
    ret
ALU_Bic endp

; uint8_t ALU_Logic_Flags(uint32_t result, uint8_t statusFlags)
; Inherits C and V flag from statusFlags
; Sets Z and N flags based on result.
; Returns combined flags.
ALU_Logic_Flags proc
    andb 3,%dl              ; Mask out the Z and N flags.
    test %ecx,%ecx          ; Set host Z and N flags based on the value.
    setz %al                ; Capture the host Z flag state
    sets %cl                ; Capture the host N flag state
    shl 2,%al               ; Move the Z flag to the right position
    shl 3,%cl               ; Move the N flag to the right position
    or %dl,%al
    or %cl,%al
    ret
ALU_Logic_Flags endp

; uint32_t ALU_Mul(uint32_t op1, uint32_t op2,
;                     StatusFlags[uint8_t] &statusFlags)
ALU_Mul proc
    mov %edx,%eax   ; Move one multiplicand to the accumulator
    mul %ecx        ; perform 32-bit multiply.
    CAPTURE_AND_MERGE_LOGIC_PSR %r8
    ret
ALU_Mul endp

; uint32_t ALU_MulAcc(uint32_t op1, uint32_t op2, uint32_t op3,
;                        StatusFlags[uint8_t] &statusFlags)
; result = (op1 * op2) + op3
ALU_Mla proc
    mov %edx,%eax   ; Move one multiplicand to the accumulator
    mul %ecx        ; perform 32-bit multiply.
    addl %eax,[%r8] ; Accumulate
    CAPTURE_AND_MERGE_LOGIC_PSR %r9
    ret
ALU_Mla endp

; uint8_t ALU_Umull(LongWord &rd, uint32_t rs, uint32_t rm, uint8_t statusFlags)
ALU_Umull proc
    mov %edx,%eax   ; Move one of the multiplicands into the accumulator
    mul %r8         ; Perform 64-bit multiplication to produce a 128-bit result
    UPDATE_LOGIC_PSR %r9b
    movq %rax,[%rcx]  ; Store the 64-bit result
    xor %rax,%rax   ; Clear RAX ready overwrite the bottom 8-bits.
    mov %al,%r9b    ; Return the updated ARM-compatible PSR flags.
    ret
ALU_Umull endp

; uint8_t ALU_Umlal(LongWord &rd, uint32_t rs, uint32_t rm, uint8_t statusFlags)
ALU_Umlal proc
    mov %edx,%eax   ; Move rs into the accumulator and zero the uppe 32-bits.
    mul %r8         ; Perform 64-bit multiplication to produce a 128-bit result
    addq %rax,[rcx] ; 64-bit accumulate into memory
    UPDATE_LOGIC_PSR %r9b
    xor %rax,%rax   ; Clear RAX ready overwrite the bottom 8-bits.
    mov %r9b,%al      ; Return the updated ARM-compatible PSR flags.
    ret
ALU_Umlal endp

; uint8_t ALU_Smull(LongWord &rd, uint32_t rs, uint32_t rm, uint8_t statusFlags)
ALU_Smull proc
    movsxd %r8d,r8  ; Sign extend rm to 64-bits
    movsxd %edx,%rax ; Move one of the multiplicands into the accumulator and
                    ; sign extend it
    imul %r8        ; Perform 64-bit multiplication to produce a 128-bit result
    UPDATE_LOGIC_PSR %r9b
    movq %rax,[rcx] ; Store the 64-bit result
    xor %rax,%rax   ; Clear RAX ready overwrite the bottom 8-bits.
    mov %r9b,%al    ; Return the updated ARM-compatible PSR flags.
    ret
ALU_Smull endp

; uint8_t ALU_Smlal(LongWord &rd, uint32_t rs, uint32_t rm, uint8_t statusFlags)
ALU_Smlal proc
    movsxd %r8d,%r8 ; Sign extend rm to 64-bits
    movsxd %edx,%rax ; Move one of the multiplicands into the accumulator and
                    ; sign extend it
    imul %r8        ; Perform 64-bit multiplication to produce a 128-bit result
    addq %rax,[rcx] ; Accumulate the 64-bit result
    UPDATE_LOGIC_PSR %r9b
    xor %rax,%rax   ; Clear RAX ready overwrite the bottom 8-bits.
    mov %r9b,%al    ; Return the updated ARM-compatible PSR flags.
    ret
ALU_Smlal endp

    end