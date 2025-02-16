#include "MsrBitMap.h"

BOOLEAN SetMsrBitmap(VIRTUAL_CPU* VirtualCpu, UINT32 Msr, CHAR bit, BOOLEAN WriteDetection) {

	if (Msr <= MSR_ID_LOW_MAX) {

		if (!WriteDetection) VirtualCpu->MsrBitMap.RdmsrLow[Msr / 8] = bit << (Msr & 3);

		else VirtualCpu->MsrBitMap.WrmsrLow[Msr / 8] = bit << (Msr & 3);

		return TRUE;
	}
	else if ((MSR_ID_HIGH_MIN <= Msr) && (Msr <= MSR_ID_HIGH_MAX)) {

		if (!WriteDetection) VirtualCpu->MsrBitMap.RdmsrHigh[(Msr - MSR_ID_HIGH_MIN) / 8] = bit << (Msr & 3);

		else VirtualCpu->MsrBitMap.WrmsrHigh[(Msr - MSR_ID_HIGH_MIN) / 8] = bit << (Msr & 3);

		return TRUE;
	}
	else return FALSE;
}

BOOLEAN BuildMsrBitmap(VIRTUAL_CPU* VirtualCpu) {

	IA32_MTRR_CAPABILITIES_REGISTER MtrrCapabilities = { 0 };

	MtrrCapabilities.AsUInt = __readmsr(IA32_MTRR_CAPABILITIES);

	SetMsrBitmap(VirtualCpu, IA32_FEATURE_CONTROL, 1, 0);

	SetMsrBitmap(VirtualCpu, IA32_MTRR_DEF_TYPE, 1, 1);

	if (MtrrCapabilities.FixedRangeSupported) {

		SetMsrBitmap(VirtualCpu, IA32_MTRR_FIX64K_00000, 1, 1);

		SetMsrBitmap(VirtualCpu, IA32_MTRR_FIX16K_80000, 1, 1);

		SetMsrBitmap(VirtualCpu, IA32_MTRR_FIX16K_A0000, 1, 1);

		for (int i = 0; i < 8; i++) SetMsrBitmap(VirtualCpu, IA32_MTRR_FIX4K_C0000 + i, 1, 1);
	}
	for (int i = 0; i < MtrrCapabilities.VariableRangeCount; i++) {

		SetMsrBitmap(VirtualCpu, IA32_MTRR_PHYSBASE0 + i * 2, 1, 1);

		SetMsrBitmap(VirtualCpu, IA32_MTRR_PHYSMASK0 + i * 2, 1, 1);
	}
	return TRUE;
}