#include "main.h"
#include "HyperVisor.h"

void DriverUnload(PDRIVER_OBJECT DriverObject) {

	if (RemoveHypervisor() == FALSE) DbgPrint("[Yukino] Fail to remove hypervisor.\n");

	else DbgPrint("[Yukino] Remove hypervisor success.\n");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject) {

	DriverObject->DriverUnload = DriverUnload;

	if (CreateHypervisor() == FALSE) {

		DbgPrint("[Yukino] Fail to create hypervisor.\n");

		return STATUS_UNSUCCESSFUL;
	}
	DbgPrint("[Yukino] Create hypervisor success.\n");

	return STATUS_SUCCESS;
}