EXTERN VmExitHandler:PROC
EXTERN AsmGetGuestRsp:PROC
EXTERN AsmGetGuestRip:PROC
EXTERN AsmGetGuestCsSelector:PROC
EXTERN AsmGetGuestRflags:PROC
EXTERN AsmGetGuestSsSelector:PROC
EXTERN AsmGetGuestCr0:PROC
EXTERN AsmGetGuestCr4:PROC

.code

AsmVmExitHandler PROC
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rdi
    push rsi
    push rbp
    push rbp
    push rbx
    push rdx
    push rcx
    push rax

    mov rcx, rsp

    sub rsp, 20h
    call VmExitHandler
    add rsp, 20h

    cmp	al, 1
    je AsmVmxoffHandler
Resume:
    pop rax
    pop rcx
    pop rdx
    pop rbx
    pop rbp
    pop rbp
    pop rsi
    pop rdi 
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
    vmresume
AsmVmExitHandler ENDP

AsmVmxoffHandler PROC
    pop rax
    pop rcx
    pop rdx
    pop rbx
    pop rbp
    pop rbp
    pop rsi
    pop rdi 
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15

    push rax

    sub rsp, 20h
    call AsmGetGuestRip
    add rsp, 20h
    mov [rsp + 08h], rax

    sub rsp, 20h
    call AsmGetGuestCsSelector
    add rsp, 20h
    mov [rsp + 10h], rax

    sub rsp, 20h
    call AsmGetGuestRflags
    add rsp, 20h
    mov [rsp + 18h], rax

    sub rsp, 20h
    call AsmGetGuestRsp
    add rsp, 20h
    mov [rsp + 20h], rax

    sub rsp, 20h
    call AsmGetGuestSsSelector
    add rsp, 20h
    mov [rsp + 28h], rax

    vmxoff

    sub rsp, 20h
    call AsmGetGuestCr0
    add rsp, 20h
    mov cr0, rax

    sub rsp, 20h
    call AsmGetGuestCr4
    add rsp, 20h
    mov cr4, rax

    pop rax

    iretq
AsmVmxoffHandler ENDP

END