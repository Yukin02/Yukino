#include "VmxRoutines.h"

void WriteVmcsCrtl(UINT32 ctrl, UINT32 ctls_0, UINT32 ctls_1, UINT64 value) {

	IA32_VMX_BASIC_REGISTER vmx_basic = { .AsUInt = __readmsr(IA32_VMX_BASIC) };

    UINT64 msr = __readmsr(vmx_basic.VmxControls ? ctls_1 : ctls_0);

	value &= msr >> 32;

	value |= msr & 0xFFFFFFFF;

	vmx_vmwrite(ctrl, value);
}

void FillGuestSelectorData(SEGMENT_DESCRIPTOR_REGISTER_64 Gdtr, UINT32 SegReg, SEGMENT_SELECTOR Selector) {

    SEGMENT_DESCRIPTOR_64* Descriptor = (SEGMENT_DESCRIPTOR_64*)(Gdtr.BaseAddress + Selector.Index * 8);

    UINT64 BaseAddress = (UINT64)Descriptor->BaseAddressLow | ((UINT64)Descriptor->BaseAddressMiddle << 16) | ((UINT64)Descriptor->BaseAddressHigh << 24);

    if (Descriptor->DescriptorType == SEGMENT_DESCRIPTOR_TYPE_SYSTEM) BaseAddress |= (UINT64)Descriptor->BaseAddressUpper << 32;

    VMX_SEGMENT_ACCESS_RIGHTS AccessRights = { 

        .Type = Descriptor->Type,

        .DescriptorType = Descriptor->DescriptorType,

        .DescriptorPrivilegeLevel = Descriptor->DescriptorPrivilegeLevel,

        .Present = Descriptor->Present,

        .AvailableBit = Descriptor->System,

        .LongMode = Descriptor->LongMode,

        .DefaultBig = Descriptor->DefaultBig,

        .Granularity = Descriptor->Granularity,

        .Unusable = Selector.Index == 0
    };
    vmx_vmwrite(VMCS_GUEST_ES_SELECTOR + SegReg * 2, Selector.AsUInt);

    vmx_vmwrite(VMCS_GUEST_ES_LIMIT + SegReg * 2, __segmentlimit(Selector.AsUInt));

    vmx_vmwrite(VMCS_GUEST_ES_ACCESS_RIGHTS + SegReg * 2, AccessRights.AsUInt);
    
    vmx_vmwrite(VMCS_GUEST_ES_BASE + SegReg * 2, BaseAddress);
}

void ExecuteNextInstruction() {

	UINT64 GuestRIP = vmx_vmread(VMCS_GUEST_RIP);

	UINT64 ExitInstructionLength = vmx_vmread(VMCS_VMEXIT_INSTRUCTION_LENGTH);

	UINT64 ResumeRIP = GuestRIP + ExitInstructionLength;

    vmx_vmwrite(VMCS_GUEST_RIP, ResumeRIP);

    VMX_INTERRUPTIBILITY_STATE InterruptibilityState = { .AsUInt = vmx_vmread(VMCS_GUEST_INTERRUPTIBILITY_STATE) };

    InterruptibilityState.BlockingByMovSs = 0;

    InterruptibilityState.BlockingBySti = 0;

    vmx_vmwrite(VMCS_GUEST_INTERRUPTIBILITY_STATE, InterruptibilityState.AsUInt);

	IA32_DEBUGCTL_REGISTER Debugctl = { .AsUInt = vmx_vmread(VMCS_GUEST_DEBUGCTL) };

    RFLAGS Rflags = { .AsUInt = vmx_vmread(VMCS_GUEST_RFLAGS) };

	if (Rflags.TrapFlag && !Debugctl.Btf) {

        VMX_PENDING_DEBUG_EXCEPTIONS DebugExceptions = { .AsUInt = vmx_vmread(VMCS_GUEST_PENDING_DEBUG_EXCEPTIONS) };

		DebugExceptions.Bs = 1;

		vmx_vmwrite(VMCS_GUEST_PENDING_DEBUG_EXCEPTIONS, DebugExceptions.AsUInt);
	}
}

UINT64 AsmGetGuestRsp() {
    return g_VirtualCpu[KeGetCurrentProcessorNumber()].GuestRsp;
}

UINT64 AsmGetGuestRip() {
    return g_VirtualCpu[KeGetCurrentProcessorNumber()].GuestRip;
}

UINT64 AsmGetGuestCsSelector() {
    return g_VirtualCpu[KeGetCurrentProcessorNumber()].GuestCsSelector;
}

UINT64 AsmGetGuestRflags() {
    return g_VirtualCpu[KeGetCurrentProcessorNumber()].GuestRflags;
}

UINT64 AsmGetGuestSsSelector() {
    return g_VirtualCpu[KeGetCurrentProcessorNumber()].GuestSsSelector;
}

UINT64 AsmGetGuestCr0() {
    return g_VirtualCpu[KeGetCurrentProcessorNumber()].Cr0;
}

UINT64 AsmGetGuestCr4() {
    return g_VirtualCpu[KeGetCurrentProcessorNumber()].Cr4;
}

CR0 VmxGetGuestCr0() {
    CR0 cr0 = { .AsUInt = (vmx_vmread(VMCS_CTRL_CR0_READ_SHADOW) & vmx_vmread(VMCS_CTRL_CR0_GUEST_HOST_MASK)) | (vmx_vmread(VMCS_GUEST_CR0) & ~vmx_vmread(VMCS_CTRL_CR0_GUEST_HOST_MASK)) };
    return cr0;
}

CR4 VmxGetGuestCr4() {
    CR4 cr4 = { .AsUInt = (vmx_vmread(VMCS_CTRL_CR4_READ_SHADOW) & vmx_vmread(VMCS_CTRL_CR4_GUEST_HOST_MASK)) | (vmx_vmread(VMCS_GUEST_CR4) & ~vmx_vmread(VMCS_CTRL_CR4_GUEST_HOST_MASK)) };
    return cr4;
}

void InjectInterruption(INTERRUPTION_TYPE InterruptionType, UINT32 Vector, UINT32 Error) {

    VMENTRY_INTERRUPT_INFORMATION VmentryInterruptInformation = { 

        .Vector = Vector,

        .InterruptionType = InterruptionType,

        .DeliverErrorCode = Error ? 1 : 0,

        .Valid = 1
    };
    if (Error) vmx_vmwrite(VMCS_CTRL_VMENTRY_EXCEPTION_ERROR_CODE, Error);

    vmx_vmwrite(VMCS_CTRL_VMENTRY_INTERRUPTION_INFORMATION_FIELD, VmentryInterruptInformation.AsUInt);
}