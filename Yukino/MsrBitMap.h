#pragma once
#include "main.h"
#include "VirtualState.h"

BOOLEAN SetMsrBitmap(VIRTUAL_CPU* VirtualCpu, UINT32 Msr, CHAR bit, BOOLEAN WriteDetection);

BOOLEAN BuildMsrBitmap(VIRTUAL_CPU* VirtualCpu);