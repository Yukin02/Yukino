#include "Utils.h"

BOOLEAN CheckIsSupportVmx() {

	INT32 ctx[4] = { 0 };

	__cpuidex(ctx, 1, 0);

	return ctx[2] & (1 << 5);
}

BOOLEAN RunOnAllProcessors(PVOID Routine) {

	BOOLEAN Status = TRUE;

	for (int ProcessorNumber = 0; ProcessorNumber < KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS); ProcessorNumber++) {
		
		KIRQL Irql = 0;
		
		if (KeGetCurrentIrql() > PASSIVE_LEVEL) Irql = KeRaiseIrqlToDpcLevel();
		
		KeSetSystemAffinityThread((KAFFINITY)(1 << ProcessorNumber));

		if (!((BOOLEAN(*)())Routine)()) Status = FALSE;

		KeRevertToUserAffinityThread();

		if (Irql) KeLowerIrql(Irql);
	}
	return Status;
}