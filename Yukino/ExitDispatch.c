#include "ExitDispatch.h"

void VmExitHandleCpuid(VIRTUAL_CPU* VirtualCpu) {

	GUEST_REGS* GuestRegs = VirtualCpu->GuestRegs;

	int CpuInfo[4] = { 0 };

	__cpuidex(CpuInfo, GuestRegs->eax, GuestRegs->ecx);

	GuestRegs->rax = CpuInfo[0];

	GuestRegs->rbx = CpuInfo[1];

	GuestRegs->rcx = CpuInfo[2];

	GuestRegs->rdx = CpuInfo[3];

	ExecuteNextInstruction();
}

void VmExitHandleRdtsc(VIRTUAL_CPU* VirtualCpu) {

	GUEST_REGS* GuestRegs = VirtualCpu->GuestRegs;

	UINT64 Tsc = __rdtsc();

	GuestRegs->rax = Tsc & 0xFFFFFFFF;

	GuestRegs->rdx = (Tsc >> 32) & 0xFFFFFFFF;

	ExecuteNextInstruction();
}

void VmExitHandleVmcall(VIRTUAL_CPU* VirtualCpu) {

	GUEST_REGS* GuestRegs = VirtualCpu->GuestRegs;

	if (GuestRegs->rax == 0x1003848372) {

		VMM_CALL VmCallArgs = { 

			.CallNumber = GuestRegs->r8,

			.Args_1 = GuestRegs->r9,

			.Args_2 = GuestRegs->r10,

			.Args_3 = GuestRegs->r11,
		};
		GuestRegs->rax = VmCallHandler(VirtualCpu, &VmCallArgs);
	}
	else {
		InjectInterruption(HardwareException, InvalidOpcode, 0);
	}
}

void VmExitHandleCRAccess(VIRTUAL_CPU* VirtualCpu) {

	GUEST_REGS* GuestRegs = VirtualCpu->GuestRegs;

	VMX_EXIT_QUALIFICATION_MOV_CR CrExitQualification = { 0 };

	CrExitQualification.AsUInt = vmx_vmread(VMCS_EXIT_QUALIFICATION);

	UINT64* RegPtr = &GuestRegs->rax + CrExitQualification.GeneralPurposeRegister;

	UINT64 GuestRsp = 0;

	if (CrExitQualification.GeneralPurposeRegister == 4) {

		GuestRsp = vmx_vmread(VMCS_GUEST_RSP);

		*RegPtr = GuestRsp;
	}
	switch (CrExitQualification.AccessType) {

		case VMX_EXIT_QUALIFICATION_ACCESS_MOV_TO_CR: {

			switch (CrExitQualification.ControlRegister) {

				case VMX_EXIT_QUALIFICATION_REGISTER_CR0: {

					vmx_vmwrite(VMCS_GUEST_CR0, *RegPtr);

					vmx_vmwrite(VMCS_CTRL_CR0_READ_SHADOW, *RegPtr);

					ExecuteNextInstruction();

					break;
				}
				case VMX_EXIT_QUALIFICATION_REGISTER_CR3: {

					vmx_vmwrite(VMCS_GUEST_CR3, (*RegPtr & ~(1ULL << 63)));

					ExecuteNextInstruction();

					break;
				}
				case VMX_EXIT_QUALIFICATION_REGISTER_CR4: {

					vmx_vmwrite(VMCS_GUEST_CR4, *RegPtr);

					vmx_vmwrite(VMCS_CTRL_CR4_READ_SHADOW, *RegPtr);

					ExecuteNextInstruction();

					break;
				}
			}
			break;
		}
		case VMX_EXIT_QUALIFICATION_ACCESS_MOV_FROM_CR: {

			switch (CrExitQualification.ControlRegister) {

				case VMX_EXIT_QUALIFICATION_REGISTER_CR0: {
					__vmx_vmread(VMCS_GUEST_CR0, RegPtr);
					break;
				}
				case VMX_EXIT_QUALIFICATION_REGISTER_CR3: {
					__vmx_vmread(VMCS_GUEST_CR3, RegPtr);
					break;
				}
				case VMX_EXIT_QUALIFICATION_REGISTER_CR4: {
					__vmx_vmread(VMCS_GUEST_CR4, RegPtr);
					break;
				}
			}
			ExecuteNextInstruction();

			break;
		}
	}
}

void VmExitHandleMsrRead(VIRTUAL_CPU* VirtualCpu) {

	GUEST_REGS* GuestRegs = VirtualCpu->GuestRegs;

	UINT64 msr = __readmsr(GuestRegs->ecx);

	GuestRegs->rax = msr & 0xFFFFFFFF;

	GuestRegs->rdx = msr >> 32;

	ExecuteNextInstruction();
}

void VmExitHandleMsrWrite(VIRTUAL_CPU* VirtualCpu) {

	GUEST_REGS* GuestRegs = VirtualCpu->GuestRegs;

	UINT32 msr = GuestRegs->ecx;

	UINT64 value = (UINT64)(GuestRegs->rdx << 32) | GuestRegs->eax;

	__writemsr(GuestRegs->ecx, value);

	ExecuteNextInstruction();
}

void VmExitHandleRdtscp(VIRTUAL_CPU* VirtualCpu) {

	GUEST_REGS* GuestRegs = VirtualCpu->GuestRegs;

	UINT32 Aux = 0ui32;

	UINT64 Tsc = __rdtscp((unsigned int*)&Aux);

	GuestRegs->rax = Tsc & 0xFFFFFFFF;

	GuestRegs->rdx = (Tsc >> 32) & 0xFFFFFFFF;

	GuestRegs->rcx = Aux;

	ExecuteNextInstruction();
}

void VmExitHandleXsetbv(VIRTUAL_CPU* VirtualCpu) {

	GUEST_REGS* GuestRegs = VirtualCpu->GuestRegs;

	_xsetbv(GuestRegs->ecx, (UINT64)(GuestRegs->rdx << 32) | GuestRegs->eax);

	ExecuteNextInstruction();
}