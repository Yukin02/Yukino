#include "Global.h"

VIRTUAL_CPU* g_VirtualCpu = 0;

UINT32 POOL_TAG = 1003848372;

SYSTEM_INFO SystemInfo = { 0 };

IA32_VMX_PINBASED_CTLS_REGISTER VmxPinbasedCtls = {
	.AsUInt = 0
};

IA32_VMX_PROCBASED_CTLS_REGISTER VmxProcbasedCtls = {
	.Cr3LoadExiting = 0,

	.UseMsrBitmaps = 1,

	.ActivateSecondaryControls = 1
};

IA32_VMX_PROCBASED_CTLS2_REGISTER VmxProcbasedCtls2 = {
	.EnableRdtscp = 1,

	.EnableInvpcid = 1,

	.EnableXsaves = 1,

	.ConcealVmxFromPt = 1
};

IA32_VMX_EXIT_CTLS_REGISTER VmxExitCtls = {
	.SaveDebugControls = 1,

	.HostAddressSpaceSize = 1,

	.ConcealVmxFromPt = 1
};

IA32_VMX_ENTRY_CTLS_REGISTER VmxEntryCtls = {
	.LoadDebugControls = 1,

	.Ia32EModeGuest = 1,

	.ConcealVmxFromPt = 1
};