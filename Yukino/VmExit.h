#pragma once
#include "main.h"
#include "VirtualState.h"
#include "Features.h"
#include "VmxRoutines.h"
#include "ExitDispatch.h"
#include "Global.h"

extern BOOLEAN VmExitHandler(GUEST_REGS* GuestRegs);