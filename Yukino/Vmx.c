#include "Vmx.h"

BOOLEAN RoutineSaveCrInfo() {

	VIRTUAL_CPU* VirtualCpu = &g_VirtualCpu[KeGetCurrentProcessorNumber()];

	VirtualCpu->VmxCr0Fixed_0 = __readmsr(IA32_VMX_CR0_FIXED0);

	VirtualCpu->VmxCr0Fixed_1 = __readmsr(IA32_VMX_CR0_FIXED1);

	VirtualCpu->VmxCr4Fixed_0 = __readmsr(IA32_VMX_CR4_FIXED0);

	VirtualCpu->VmxCr4Fixed_1 = __readmsr(IA32_VMX_CR4_FIXED1);

	return TRUE;
}

BOOLEAN RoutinEnableVmx() {

	VIRTUAL_CPU* VirtualCpu = &g_VirtualCpu[KeGetCurrentProcessorNumber()];

	UINT64 Cr0 = __readcr0();

	Cr0 |= VirtualCpu->VmxCr0Fixed_0;

	Cr0 &= VirtualCpu->VmxCr0Fixed_1;

	__writecr0(Cr0);

	UINT64 Cr4 = __readcr4();

	Cr4 |= CR4_VMX_ENABLE_FLAG;

	Cr4 |= VirtualCpu->VmxCr4Fixed_0;

	Cr4 &= VirtualCpu->VmxCr4Fixed_1;

	__writecr4(Cr4);

	return TRUE;
}

BOOLEAN RoutinLoadVmCs() {

	VIRTUAL_CPU* VirtualCpu = &g_VirtualCpu[KeGetCurrentProcessorNumber()];

	UINT64 VmxOnPhysical = MmGetPhysicalAddress(&VirtualCpu->VmxOn).QuadPart;

	UINT64 VmCsPhysical = MmGetPhysicalAddress(&VirtualCpu->VmCs).QuadPart;

	IA32_VMX_BASIC_REGISTER VmxBaseMsr = { .AsUInt = __readmsr(IA32_VMX_BASIC) };

	VirtualCpu->VmxOn.RevisionId = VmxBaseMsr.VmcsRevisionId;

	VirtualCpu->VmCs.RevisionId = VmxBaseMsr.VmcsRevisionId;

	UINT8 status = __vmx_on((UINT64*)&VmxOnPhysical);

	if (status) {

		DbgPrint("[Yukino] Fail to execute __vmx_on error:%d, Processor:%d\n", status, KeGetCurrentProcessorNumber());

		return FALSE;
	}
	status = __vmx_vmclear((UINT64*)&VmCsPhysical);

	if (status) {

		__vmx_off();

		DbgPrint("[Yukino] Fail to execute __vmx_vmclear error:%d, Processor:%d\n", status, KeGetCurrentProcessorNumber());

		return FALSE;
	}
	status = __vmx_vmptrld((UINT64*)&VmCsPhysical);

	if (status) {

		__vmx_off();

		DbgPrint("[Yukino] Fail to execute __vmx_vmptrld error:%d, Processor:%d\n", status, KeGetCurrentProcessorNumber());

		return FALSE;
	}
	return TRUE;
}

BOOLEAN RoutineVirtualCpu() {

	VIRTUAL_CPU* VirtualCpu = &g_VirtualCpu[KeGetCurrentProcessorNumber()];

	SetupVmCs(VirtualCpu);

	if (!vmx_launch()) {

		DbgPrint("[Yukino] Fail to execute __vmx_vmlaunch, ErrorCode:%d\n", vmx_vmread(VMCS_VM_INSTRUCTION_ERROR));

		__vmx_off();

		return FALSE;
	}
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	VMM_CALL call = { 0 };

	call.CallNumber = Test;

	__try { Status = VmxVmcall(&call); } __except (1) { return FALSE; }

	if (!NT_SUCCESS(Status)) return FALSE;

	VirtualCpu->LaunchExecuted = TRUE;

	VirtualCpu->VmxoffExecuted = FALSE;

	return TRUE;
}

BOOLEAN RoutineRemoveVirtual() {

	if (!g_VirtualCpu || !g_VirtualCpu[KeGetCurrentProcessorNumber()].LaunchExecuted) return FALSE;

	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	VMM_CALL call = { 0 };

	call.CallNumber = Vmxoff;

	Status = VmxVmcall(&call);

	return NT_SUCCESS(Status);
}

void SetupVmCs(VIRTUAL_CPU* VirtualCpu) {

	WriteVmcsCrtl(VMCS_CTRL_PIN_BASED_VM_EXECUTION_CONTROLS, IA32_VMX_PINBASED_CTLS, IA32_VMX_TRUE_PINBASED_CTLS, VmxPinbasedCtls.AsUInt);

	WriteVmcsCrtl(VMCS_CTRL_PROCESSOR_BASED_VM_EXECUTION_CONTROLS, IA32_VMX_PROCBASED_CTLS, IA32_VMX_TRUE_PROCBASED_CTLS, VmxProcbasedCtls.AsUInt);

	WriteVmcsCrtl(VMCS_CTRL_SECONDARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS, IA32_VMX_PROCBASED_CTLS2, IA32_VMX_PROCBASED_CTLS2, VmxProcbasedCtls2.AsUInt);

	WriteVmcsCrtl(VMCS_CTRL_PRIMARY_VMEXIT_CONTROLS, IA32_VMX_EXIT_CTLS, IA32_VMX_TRUE_EXIT_CTLS, VmxExitCtls.AsUInt);

	WriteVmcsCrtl(VMCS_CTRL_VMENTRY_CONTROLS, IA32_VMX_ENTRY_CTLS, IA32_VMX_TRUE_ENTRY_CTLS, VmxEntryCtls.AsUInt);
	
	vmx_vmwrite(VMCS_CTRL_MSR_BITMAP_ADDRESS, MmGetPhysicalAddress(&VirtualCpu->MsrBitMap).QuadPart);

	vmx_vmwrite(VMCS_CTRL_CR0_GUEST_HOST_MASK, VirtualCpu->VmxCr0Fixed_0 | ~VirtualCpu->VmxCr0Fixed_1 | CR0_CACHE_DISABLE_FLAG | CR0_WRITE_PROTECT_FLAG);

	vmx_vmwrite(VMCS_CTRL_CR4_GUEST_HOST_MASK, VirtualCpu->VmxCr4Fixed_0 | ~VirtualCpu->VmxCr4Fixed_1);

	vmx_vmwrite(VMCS_CTRL_CR0_READ_SHADOW, __readcr0());

	vmx_vmwrite(VMCS_CTRL_CR4_READ_SHADOW, __readcr4() & ~CR4_VMX_ENABLE_FLAG);

	vmx_vmwrite(VMCS_HOST_ES_SELECTOR, ReadEs().AsUInt & 0xF8);

	vmx_vmwrite(VMCS_HOST_CS_SELECTOR, ReadCs().AsUInt & 0xF8);

	vmx_vmwrite(VMCS_HOST_SS_SELECTOR, ReadSs().AsUInt & 0xF8);

	vmx_vmwrite(VMCS_HOST_DS_SELECTOR, ReadDs().AsUInt & 0xF8);

	vmx_vmwrite(VMCS_HOST_FS_SELECTOR, ReadFs().AsUInt & 0xF8);

	vmx_vmwrite(VMCS_HOST_GS_SELECTOR, ReadGs().AsUInt & 0xF8);

	vmx_vmwrite(VMCS_HOST_TR_SELECTOR, ReadTr().AsUInt & 0xF8);

	vmx_vmwrite(VMCS_HOST_FS_BASE, __readmsr(IA32_FS_BASE));

	vmx_vmwrite(VMCS_HOST_GS_BASE, __readmsr(IA32_GS_BASE));

	vmx_vmwrite(VMCS_HOST_CR0, __readcr0());

	vmx_vmwrite(VMCS_HOST_CR3, __readcr3());

	vmx_vmwrite(VMCS_HOST_CR4, __readcr4());

	vmx_vmwrite(VMCS_HOST_TR_BASE, 0);

	vmx_vmwrite(VMCS_HOST_GDTR_BASE, GetGdt().BaseAddress);

	vmx_vmwrite(VMCS_HOST_IDTR_BASE, GetIdt().BaseAddress);

	vmx_vmwrite(VMCS_HOST_SYSENTER_CS, __readmsr(IA32_SYSENTER_CS));

	vmx_vmwrite(VMCS_HOST_SYSENTER_ESP, __readmsr(IA32_SYSENTER_ESP));

	vmx_vmwrite(VMCS_HOST_SYSENTER_EIP, __readmsr(IA32_SYSENTER_EIP));

	vmx_vmwrite(VMCS_GUEST_CR0, __readcr0());

	vmx_vmwrite(VMCS_GUEST_CR3, __readcr3());

	vmx_vmwrite(VMCS_GUEST_CR4, __readcr4());

	vmx_vmwrite(VMCS_GUEST_DR7, __readdr(7));

	vmx_vmwrite(VMCS_HOST_RSP, (((UINT64)VirtualCpu->HostStack + VMX_STACK_SIZE)) - 8);

	vmx_vmwrite(VMCS_HOST_RIP, (UINT64)AsmVmExitHandler);

	vmx_vmwrite(VMCS_GUEST_RSP, 0);

	vmx_vmwrite(VMCS_GUEST_RIP, RestoreRip);

	vmx_vmwrite(VMCS_GUEST_RFLAGS, __readeflags());

	FillGuestSelectorData(GetGdt(), ES, ReadEs());

	FillGuestSelectorData(GetGdt(), CS, ReadCs());

	FillGuestSelectorData(GetGdt(), SS, ReadSs());

	FillGuestSelectorData(GetGdt(), DS, ReadDs());

	FillGuestSelectorData(GetGdt(), FS, ReadFs());

	FillGuestSelectorData(GetGdt(), GS, ReadGs());

	FillGuestSelectorData(GetGdt(), LDTR, ReadLdtr());

	FillGuestSelectorData(GetGdt(), TR, ReadTr());

	vmx_vmwrite(VMCS_GUEST_FS_BASE, __readmsr(IA32_FS_BASE));

	vmx_vmwrite(VMCS_GUEST_GS_BASE, __readmsr(IA32_GS_BASE));

	vmx_vmwrite(VMCS_GUEST_GDTR_BASE, GetGdt().BaseAddress);

	vmx_vmwrite(VMCS_GUEST_IDTR_BASE, GetIdt().BaseAddress);

	vmx_vmwrite(VMCS_GUEST_GDTR_LIMIT, GetGdt().Limit);

	vmx_vmwrite(VMCS_GUEST_IDTR_LIMIT, GetIdt().Limit);

	vmx_vmwrite(VMCS_GUEST_SYSENTER_CS, __readmsr(IA32_SYSENTER_CS));

	vmx_vmwrite(VMCS_GUEST_SYSENTER_ESP, __readmsr(IA32_SYSENTER_ESP));

	vmx_vmwrite(VMCS_GUEST_SYSENTER_EIP, __readmsr(IA32_SYSENTER_EIP));

	vmx_vmwrite(VMCS_GUEST_DEBUGCTL, __readmsr(IA32_DEBUGCTL));

	vmx_vmwrite(VMCS_GUEST_VMCS_LINK_POINTER, MAXULONG64);
}