#pragma once
#include "main.h"
#include "AsmVmx.h"
#include "Global.h"

void WriteVmcsCrtl(UINT32 ctrl, UINT32 ctls_0, UINT32 ctls_1, UINT64 value);

void FillGuestSelectorData(SEGMENT_DESCRIPTOR_REGISTER_64 Gdtr, UINT32 SegReg, SEGMENT_SELECTOR Selector);

void ExecuteNextInstruction();

UINT64 AsmGetGuestRsp();

UINT64 AsmGetGuestRip();

UINT64 AsmGetGuestCsSelector();

UINT64 AsmGetGuestRflags();

UINT64 AsmGetGuestSsSelector();

UINT64 AsmGetGuestCr0();

UINT64 AsmGetGuestCr4();

CR0 VmxGetGuestCr0();

CR4 VmxGetGuestCr4();

void InjectInterruption(INTERRUPTION_TYPE InterruptionType, UINT32 Vector, UINT32 Error);