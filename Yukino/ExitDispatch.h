#pragma once
#include "main.h"
#include "VirtualState.h"
#include "Features.h"
#include "VmxRoutines.h"
#include "VmCall.h"

void VmExitHandleCpuid(VIRTUAL_CPU* VirtualCpu);

void VmExitHandleRdtsc(VIRTUAL_CPU* VirtualCpu);

void VmExitHandleVmcall(VIRTUAL_CPU* VirtualCpu);

void VmExitHandleCRAccess(VIRTUAL_CPU* VirtualCpu);

void VmExitHandleMsrRead(VIRTUAL_CPU* VirtualCpu);

void VmExitHandleMsrWrite(VIRTUAL_CPU* VirtualCpu);

void VmExitHandleRdtscp(VIRTUAL_CPU* VirtualCpu);

void VmExitHandleXsetbv(VIRTUAL_CPU* VirtualCpu);