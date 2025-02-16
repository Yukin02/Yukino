#include "VmCall.h"

UINT64 VmCallHandler(VIRTUAL_CPU* VirtualCpu, VMM_CALL* vmcall) {

	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	GUEST_REGS* GuestRegs = VirtualCpu->GuestRegs;

	switch (vmcall->CallNumber) {
		case Test: {

			Status = STATUS_SUCCESS;

			ExecuteNextInstruction();

			break;
		}
		case Vmxoff: {

			SEGMENT_DESCRIPTOR_REGISTER_64 Gdtr = { .BaseAddress = vmx_vmread(VMCS_GUEST_GDTR_BASE), .Limit = (UINT16)vmx_vmread(VMCS_GUEST_GDTR_LIMIT) };
			
			SEGMENT_DESCRIPTOR_REGISTER_64 Idtr = { .BaseAddress = vmx_vmread(VMCS_GUEST_IDTR_BASE), .Limit = (UINT16)vmx_vmread(VMCS_GUEST_IDTR_LIMIT) };

			SetGdt(&Gdtr);

			SetIdt(&Idtr);

			__writecr3(vmx_vmread(VMCS_GUEST_CR3));

			WriteTss(Gdtr, (UINT16)vmx_vmread(VMCS_GUEST_TR_SELECTOR));

			WriteTr((UINT16)vmx_vmread(VMCS_GUEST_TR_SELECTOR));

			WriteEs((UINT16)vmx_vmread(VMCS_GUEST_ES_SELECTOR));

			WriteDs((UINT16)vmx_vmread(VMCS_GUEST_DS_SELECTOR));

			WriteFs((UINT16)vmx_vmread(VMCS_GUEST_FS_SELECTOR));

			WriteGs((UINT16)vmx_vmread(VMCS_GUEST_GS_SELECTOR));

			WriteLdtr((UINT16)vmx_vmread(VMCS_GUEST_LDTR_SELECTOR));

			__writemsr(IA32_FS_BASE, vmx_vmread(VMCS_GUEST_FS_BASE));

			__writemsr(IA32_GS_BASE, vmx_vmread(VMCS_GUEST_GS_BASE));

			VirtualCpu->VmxoffExecuted = TRUE;

			VirtualCpu->LaunchExecuted = FALSE;
			 
			VirtualCpu->GuestRip = vmx_vmread(VMCS_GUEST_RIP) + vmx_vmread(VMCS_VMEXIT_INSTRUCTION_LENGTH);

			VirtualCpu->GuestCsSelector = vmx_vmread(VMCS_GUEST_CS_SELECTOR);

			VirtualCpu->GuestRflags = vmx_vmread(VMCS_GUEST_RFLAGS);

			VirtualCpu->GuestRsp = vmx_vmread(VMCS_GUEST_RSP);

			VirtualCpu->GuestSsSelector = vmx_vmread(VMCS_GUEST_SS_SELECTOR);

			VirtualCpu->Cr0 = vmx_vmread(VMCS_CTRL_CR0_READ_SHADOW);

			VirtualCpu->Cr4 = vmx_vmread(VMCS_CTRL_CR4_READ_SHADOW);

			Status = STATUS_SUCCESS;

			ExecuteNextInstruction();

			break;
		}
		default: { InjectInterruption(HardwareException, InvalidOpcode, 0); break; }
	}
	return Status;
}