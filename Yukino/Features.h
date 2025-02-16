#pragma once
#include "main.h"

typedef struct _SYSTEM_INFO {
    UINT64 SystemCr3;
} SYSTEM_INFO, * PSYSTEM_INFO;

typedef struct _VMM_CALL {
    UINT64 CallNumber;
    UINT64 Args_1;
    UINT64 Args_2;
    UINT64 Args_3;
} VMM_CALL, * PVMM_CALL;

enum YUKINO_CALL {
	Test = 1,
	Vmxoff
};