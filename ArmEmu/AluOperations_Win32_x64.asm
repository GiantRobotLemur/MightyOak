; AluOperations_x64.asm
; author GiantRobotLemur@na-se.co.uk
; date 2023
; copyright This file is part of the Mighty Oak project which is released
; under LGPL 3 license. See LICENSE file at the repository root or go to
; https://github.com/GiantRobotLemur/MightyOak for full license details.

.code
;             BA9876543210
; x64 RFlags: VDITSZ?A?P?C
; ARMv2 PSR:  ????????NZCV

; Reads the Carry, oVerflow, Negative and Sign flags from RFlags and
; orders them in the bottom nibble of the DL register.
; Uses: AL, AH, DH and DL
MOVE_PSR_TO_DL macro
    seto dl                 ; Capture the V flag
    setc dh                 ; Capture the C flag
    setz ah                 ; Capture the Z flag
    sets al                 ; Capture the N flag
    shl dh,1                ; Move the C flag to the right position
    shl ah,2                ; Move the Z flag to the right position
    shl al,3                ; Move the N flag to the right position

    or dl,dh                ; Merge the C and V flags.
    or al,ah                ; Merge Z and N flags.
    or dl,al                ; Merge all flags into DL.
endm

; As above, but using CH/CL instead of DH/DL
MOVE_PSR_TO_CL macro
    seto cl                 ; Capture the V flag
    setc ch                 ; Capture the C flag
    setz ah                 ; Capture the Z flag
    sets al                 ; Capture the N flag
    shl ch,1                ; Move the C flag to the right position
    shl ah,2                ; Move the Z flag to the right position
    shl al,3                ; Move the N flag to the right position

    or cl,ch                ; Merge the C and V flags.
    or al,ah                ; Merge Z and N flags.
    or cl,al                ; Merge all flags into DL.
endm

CAPTURE_AND_MERGE_LOGIC_PSR macro flagAddrReg
    setz r11b               ; Capture the Z flag
    sets r10b               ; Capture the N flag
    shl r11b,2              ; Move the Z flag to the right position
    shl r10b,3              ; Move the N flag to the right position
    or r11b,r10b            ; Merge the Z and N flags.

    mov r10b, byte ptr [flagAddrReg]   ; Load ARM flags to inherit C and V
    and r10b,3              ; Mask out old N and Z flags
    or r11b, r10b           ; Merge old and new flags.
    mov byte ptr [flagAddrReg],r11b    ; Store the ARM-compatible flags
endm

UPDATE_LOGIC_PSR macro flagReg
    setz r11b               ; Capture the Z flag
    sets r10b               ; Capture the N flag
    shl r11b,2              ; Move the Z flag to the right position
    shl r10b,3              ; Move the N flag to the right position
    or r11b,r10b            ; Merge the Z and N flags.

    and flagReg,3           ; Mask out old N and Z flags
    or flagReg,r11b         ; Merge old and new flags.
endm

; uint32_t ALU_Add(uint32_t op1, uint32_t op2,
;                     StatusFlags[uint8_t] &flags)
ALU_Add proc
    add ecx,edx
    MOVE_PSR_TO_DL
    mov byte ptr [r8],dl    ; Store the ARM-compatible flags
    mov eax,ecx             ; Return the operation result
    ret
ALU_Add endp

; uint32_t ALU_Sub(uint32_t op1, uint32_t op2,
;                     StatusFlags[uint8_t] &statusFlags)
ALU_Sub proc
    sub ecx,edx
    MOVE_PSR_TO_DL
    mov byte ptr [r8],dl    ; Store the ARM-compatible flags
    mov eax,ecx             ; Return the operation result
    ret
ALU_Sub endp

; uint32_t ALU_Adc(uint32_t op1, uint32_t op2,
;                     StatusFlags[uint8_t] &statusFlags)
ALU_Adc proc
    mov al,byte ptr [r8]    ; Load ARM status flags
    shl al,7                ; Shift bit 2 out to bit 8,
                            ; which will end up in RFlags.Carry
    adc ecx,edx             ; Perform the operation being emulated.
    MOVE_PSR_TO_DL
    mov byte ptr [r8],dl    ; Store the ARM-compatible flags
    mov eax,ecx             ; Return the operation result
    ret
ALU_Adc endp

; uint32_t ALU_Sbc(uint32_t op1, uint32_t op2,
;                     StatusFlags[uint8_t] &statusFlags)
; result = op1 - (op2 + Carry)
ALU_Sbc proc
    mov al,byte ptr [r8]    ; Load ARM status flags
    shl al,7                ; Shift bit 2 out to bit 8,
                            ; which will end up in RFlags.Carry
    sbb ecx,edx             ; Perform the operation being emulated.
    MOVE_PSR_TO_DL
    mov byte ptr [r8],dl    ; Store the ARM-compatible flags
    mov eax,ecx             ; Return the operation result
    ret
ALU_Sbc endp

; uint32_t ALU_Rsc(uint32_t op1, uint32_t op2,
;                     StatusFlags[uint8_t] &statusFlags)
; result = op2 - (op1 + Carry)
ALU_Rsc proc
    mov al,byte ptr [r8]    ; Load ARM status flags
    shl al,7                ; Shift bit 2 out to bit 8,
                            ; which will end up in RFlags.Carry
    sbb edx,ecx             ; Perform the operation being emulated.
    MOVE_PSR_TO_CL
    mov byte ptr [r8],cl    ; Store the ARM-compatible flags
    mov eax,edx             ; Return the operation result
    ret
ALU_Rsc endp

; uint32_t ALU_And(uint32_t op1, uint32_t op2,
;                     StatusFlags[uint8_t] &statusFlags)
ALU_And proc
    and ecx,edx
    CAPTURE_AND_MERGE_LOGIC_PSR r8
    mov eax,ecx             ; Return the operation result
    ret
ALU_And endp

; uint32_t ALU_Or(uint32_t op1, uint32_t op2,
;                    StatusFlags[uint8_t] &statusFlags)
ALU_Or proc
    or ecx,edx
    CAPTURE_AND_MERGE_LOGIC_PSR r8
    mov eax,ecx             ; Return the operation result
    ret
ALU_Or endp

; uint32_t ALU_Xor(uint32_t op1, uint32_t op2,
;                     StatusFlags[uint8_t] &statusFlags)
ALU_Xor proc
    xor ecx,edx
    CAPTURE_AND_MERGE_LOGIC_PSR r8
    mov eax,ecx             ; Return the operation result
    ret
ALU_Xor endp

; uint32_t ALU_Bic(uint32_t op1, uint32_t op2,
;                     StatusFlags[uint8_t] &statusFlags)
; result = op1 & (~op2)
ALU_Bic proc
    not edx
    and ecx,edx
    CAPTURE_AND_MERGE_LOGIC_PSR r8
    mov eax,ecx             ; Return the operation result
    ret
ALU_Bic endp

; uint8_t ALU_Logic_Flags(uint32_t result, uint8_t statusFlags)
; Inherits C and V flag from statusFlags
; Sets Z and N flags based on result.
; Returns combined flags.
ALU_Logic_Flags proc
    and dl,03h              ; Mask out the Z and N flags.
    test ecx,ecx            ; Set host Z and N flags based on the value.
    setz al                 ; Capture the host Z flag state
    sets cl                 ; Capture the host N flag state
    shl al,2                ; Move the Z flag to the right position
    shl cl,3                ; Move the N flag to the right position
    or al,dl
    or al,cl
    ret
ALU_Logic_Flags endp

; uint32_t ALU_Mul(uint32_t op1, uint32_t op2,
;                     StatusFlags[uint8_t] &statusFlags)
ALU_Mul proc
    mov eax,edx     ; Move one multiplicand to the accumulator
    mul ecx         ; perform 32-bit multiply.
    CAPTURE_AND_MERGE_LOGIC_PSR r8
    ret
ALU_Mul endp

; uint32_t ALU_MulAcc(uint32_t op1, uint32_t op2, uint32_t op3,
;                        StatusFlags[uint8_t] &statusFlags)
; result = (op1 * op2) + op3
ALU_Mla proc
    mov eax,edx     ; Move one multiplicand to the accumulator
    mul ecx         ; perform 32-bit multiply.
    add eax,dword ptr [r8]  ; Accumulate
    CAPTURE_AND_MERGE_LOGIC_PSR r9
    ret
ALU_Mla endp

; uint8_t ALU_Umull(LongWord &rd, uint32_t rs, uint32_t rm, uint8_t statusFlags)
ALU_Umull proc
    mov eax,edx     ; Move one of the multiplicands into the accumulator
    mul r8          ; Perform 64-bit multiplication to produce a 128-bit result
    UPDATE_LOGIC_PSR r9b
    mov qword ptr [rcx], rax        ; Store the 64-bit result
    xor rax,rax     ; Clear RAX ready overwrite the bottom 8-bits.
    mov al,r9b      ; Return the updated ARM-compatible PSR flags.
    ret
ALU_Umull endp

; uint8_t ALU_Umlal(LongWord &rd, uint32_t rs, uint32_t rm, uint8_t statusFlags)
ALU_Umlal proc
    mov eax, edx    ; Move rs into the accumulator and zero the uppe 32-bits.
    mul r8          ; Perform 64-bit multiplication to produce a 128-bit result
    add qword ptr [rcx], rax    ; 64-bit accumulate into memory
    UPDATE_LOGIC_PSR r9b
    xor rax,rax     ; Clear RAX ready overwrite the bottom 8-bits.
    mov al,r9b      ; Return the updated ARM-compatible PSR flags.
    ret
ALU_Umlal endp

; uint8_t ALU_Smull(LongWord &rd, uint32_t rs, uint32_t rm, uint8_t statusFlags)
ALU_Smull proc
    movsxd r8,r8d   ; Sign extend rm to 64-bits
    movsxd rax,edx  ; Move one of the multiplicands into the accumulator and
                    ; sign extend it
    imul r8         ; Perform 64-bit multiplication to produce a 128-bit result
    UPDATE_LOGIC_PSR r9b
    mov qword ptr [rcx], rax        ; Store the 64-bit result
    xor rax,rax     ; Clear RAX ready overwrite the bottom 8-bits.
    mov al,r9b      ; Return the updated ARM-compatible PSR flags.
    ret
ALU_Smull endp

; uint8_t ALU_Smlal(LongWord &rd, uint32_t rs, uint32_t rm, uint8_t statusFlags)
ALU_Smlal proc
    movsxd r8,r8d   ; Sign extend rm to 64-bits
    movsxd rax,edx  ; Move one of the multiplicands into the accumulator and
                    ; sign extend it
    imul r8         ; Perform 64-bit multiplication to produce a 128-bit result
    add qword ptr [rcx], rax        ; Accumulate the 64-bit result
    UPDATE_LOGIC_PSR r9b
    xor rax,rax     ; Clear RAX ready overwrite the bottom 8-bits.
    mov al,r9b      ; Return the updated ARM-compatible PSR flags.
    ret
ALU_Smlal endp

    end