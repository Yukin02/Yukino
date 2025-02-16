#include "VmExit.h"

BOOLEAN VmExitHandler(GUEST_REGS* GuestRegs) {

	VIRTUAL_CPU* VirtualCpu = &g_VirtualCpu[KeGetCurrentProcessorNumber()];

	VMX_VMEXIT_REASON ExitReason = { 0 };

	ExitReason.AsUInt = vmx_vmread(VMCS_EXIT_REASON);

	VirtualCpu->GuestRegs = GuestRegs;

	switch (ExitReason.BasicExitReason) {

		case VMX_EXIT_REASON_EXECUTE_CPUID: {
			VmExitHandleCpuid(VirtualCpu);
			break;
		}
		case VMX_EXIT_REASON_EXECUTE_RDTSC: {
			VmExitHandleRdtsc(VirtualCpu);
			break;
		}
		case VMX_EXIT_REASON_EXECUTE_VMCALL: {
			VmExitHandleVmcall(VirtualCpu);
			break;
		}
		case VMX_EXIT_REASON_MOV_CR: {
			VmExitHandleCRAccess(VirtualCpu);
			break;
		}
		case VMX_EXIT_REASON_EXECUTE_RDMSR: {
			VmExitHandleMsrRead(VirtualCpu);
			break;
		}
		case VMX_EXIT_REASON_EXECUTE_WRMSR: {
			VmExitHandleMsrWrite(VirtualCpu);
			break;
		}
		case VMX_EXIT_REASON_EXECUTE_RDTSCP: {
			VmExitHandleRdtscp(VirtualCpu);
			break;
		}
		case VMX_EXIT_REASON_EXECUTE_XSETBV: {
			VmExitHandleXsetbv(VirtualCpu);
			break;
		}
		default: {
			InjectInterruption(HardwareException, GeneralProtection, 0);
			break;
		}
	}
	VirtualCpu->GuestRegs = 0;

	return VirtualCpu->VmxoffExecuted ? TRUE : FALSE;
}