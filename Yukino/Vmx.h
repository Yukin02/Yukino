#pragma once
#include "main.h"
#include "VirtualState.h"
#include "Features.h"
#include "VmxRoutines.h"
#include "AsmVmx.h"
#include "AsmVmExit.h"
#include "Global.h"
#include "Utils.h"

BOOLEAN RoutineSaveCrInfo();

BOOLEAN RoutinEnableVmx();

BOOLEAN RoutinLoadVmCs();

BOOLEAN RoutineVirtualCpu();

BOOLEAN RoutineRemoveVirtual();

void SetupVmCs(VIRTUAL_CPU* VirtualCpu);