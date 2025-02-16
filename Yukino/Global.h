#pragma once
#include "main.h"
#include "VirtualState.h"
#include "Features.h"

extern VIRTUAL_CPU* g_VirtualCpu;

extern UINT32 POOL_TAG;

extern SYSTEM_INFO SystemInfo;

extern IA32_VMX_PINBASED_CTLS_REGISTER VmxPinbasedCtls;

extern IA32_VMX_PROCBASED_CTLS_REGISTER VmxProcbasedCtls;

extern IA32_VMX_PROCBASED_CTLS2_REGISTER VmxProcbasedCtls2;

extern IA32_VMX_EXIT_CTLS_REGISTER VmxExitCtls;

extern IA32_VMX_ENTRY_CTLS_REGISTER VmxEntryCtls;