#include "HyperVisor.h"

BOOLEAN CreateHypervisor() {

	if (!CheckIsSupportVmx()) {

		DbgPrint("[Yukino] This processor does not support virtualization\n");

		return FALSE;
	}
	UINT32 ProcessorsCount = KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);

	g_VirtualCpu = (VIRTUAL_CPU*)ExAllocatePoolWithTag(NonPagedPool, ProcessorsCount * sizeof(VIRTUAL_CPU), POOL_TAG);

	if (!g_VirtualCpu) {

		DbgPrint("[Yukino] Fail to allocate memory\n");

		return FALSE;
	}
	RtlSecureZeroMemory(g_VirtualCpu, ProcessorsCount * sizeof(VIRTUAL_CPU));

	SystemInfo.SystemCr3 = *(UINT64*)((UINT64)PsInitialSystemProcess + 0x28);

	if (!RunOnAllProcessors(RoutineSaveCrInfo)) {

		DbgPrint("[Yukino] Fail to save cr info\n");

		goto UnSuccessFul;
	}
	if (!RunOnAllProcessors(RoutinEnableVmx)) {

		DbgPrint("[Yukino] Fail to enable vmx\n");

		goto UnSuccessFul;
	}
	if (!RunOnAllProcessors(RoutinLoadVmCs)) {

		DbgPrint("[Yukino] Fail to load vmcs\n");

		goto UnSuccessFul;
	}
	for (int ProcessorsIndex = 0; ProcessorsIndex < ProcessorsCount; ProcessorsIndex++) {

		if (!BuildMsrBitmap(&g_VirtualCpu[ProcessorsIndex])) {

			DbgPrint("[Yukino] Fail to build MsrBitmap, Processor:%d\n", ProcessorsIndex);

			goto UnSuccessFul;
		}
	}
	if (!RunOnAllProcessors(RoutineVirtualCpu)) goto UnSuccessFul;

	return TRUE;

UnSuccessFul:

	if (g_VirtualCpu) {

		ExFreePoolWithTag(g_VirtualCpu, POOL_TAG);
	}
	return FALSE;
}

BOOLEAN RemoveHypervisor() {

	if (!g_VirtualCpu) return FALSE;

	if (RunOnAllProcessors(RoutineRemoveVirtual)) {

		ExFreePoolWithTag(g_VirtualCpu, POOL_TAG);

		g_VirtualCpu = 0;

		return TRUE;
	}
	else return FALSE;
}