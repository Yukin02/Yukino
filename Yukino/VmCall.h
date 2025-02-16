#pragma once
#include "main.h"
#include "VirtualState.h"
#include "Features.h"
#include "VmxRoutines.h"
#include "AsmVmx.h"
#include "Global.h"

UINT64 VmCallHandler(VIRTUAL_CPU* VirtualCpu, VMM_CALL* vmcall);