#pragma once
#include "main.h"

#define VMX_STACK_SIZE       0x6000

typedef struct _GUEST_REGS {
    union {
        UINT64 rax;
        UINT32 eax;
    };
    union {
        UINT64 rcx;
        UINT32 ecx;
    };
    union {
        UINT64 rdx;
        UINT32 edx;
    };
    union {
        UINT64 rbx;
        UINT32 ebx;
    };
    union {
        UINT64 rsp;
        UINT32 esp;
    };
    union {
        UINT64 rbp;
        UINT32 ebp;
    };
    union {
        UINT64 rsi;
        UINT32 esi;
    };
    union {
        UINT64 rdi;
        UINT32 edi;
    };
    UINT64 r8;
    UINT64 r9;
    UINT64 r10;
    UINT64 r11;
    UINT64 r12;
    UINT64 r13;
    UINT64 r14;
    UINT64 r15;
} GUEST_REGS, * PGUEST_REGS;

typedef struct _VIRTUAL_CPU {
    __declspec(align(PAGE_SIZE)) VMXON VmxOn;
    __declspec(align(PAGE_SIZE)) VMCS VmCs;
    __declspec(align(PAGE_SIZE)) VMX_MSR_BITMAP MsrBitMap;
    __declspec(align(PAGE_SIZE)) UINT8 HostStack[VMX_STACK_SIZE];

    GUEST_REGS* GuestRegs;

    UINT64 VmxCr0Fixed_0;
    UINT64 VmxCr0Fixed_1;
    UINT64 VmxCr4Fixed_0;
    UINT64 VmxCr4Fixed_1;

    UINT64 GuestRip;
    UINT64 GuestCsSelector;
    UINT64 GuestRflags;
    UINT64 GuestRsp;
    UINT64 GuestSsSelector;
    UINT64 Cr0;
    UINT64 Cr4;

    BOOLEAN LaunchExecuted;
    BOOLEAN VmxoffExecuted;
}VIRTUAL_CPU, * PVIRTUAL_CPU;