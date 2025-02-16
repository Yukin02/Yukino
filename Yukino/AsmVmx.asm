.code

ReadCs PROC
  mov ax, cs
  ret
ReadCs ENDP

ReadSs PROC
  mov ax, ss
  ret
ReadSs ENDP

ReadDs PROC
  mov ax, ds
  ret
ReadDs ENDP

ReadEs PROC 
  mov ax, es
  ret
ReadEs ENDP

ReadFs PROC
  mov ax, fs
  ret
ReadFs ENDP

ReadGs PROC
  mov ax, gs
  ret
ReadGs ENDP

ReadTr PROC
  str ax
  ret
ReadTr ENDP

ReadLdtr PROC
  sldt ax
  ret
ReadLdtr ENDP

WriteDs PROC
  mov ds, cx
  ret
WriteDs ENDP

WriteEs PROC
  mov es, cx
  ret
WriteEs ENDP

WriteCs PROC
  mov cs, cx
  ret
WriteCs ENDP

WriteSs PROC
  mov ss, cx
  ret
WriteSs ENDP

WriteFs PROC
  mov fs, cx
  ret
WriteFs ENDP

WriteGs PROC
  mov gs, cx
  ret
WriteGs ENDP

WriteTr PROC
  ltr cx
  ret
WriteTr ENDP

WriteLdtr PROC
  lldt cx
  ret
WriteLdtr ENDP

WriteTss PROC
  movzx r8d, dx
  mov rdx, [rcx + 2]
  shr r8, 3 
  mov eax, [rdx + r8 * 8 + 4]
  and eax, 0FFFFF9FFh
  or eax, 900h
  mov [rdx + r8 * 8 + 4], eax
  ret
WriteTss ENDP

vmx_launch PROC
  mov rax, 681Ch
  vmwrite rax, rsp
  vmlaunch
  mov rax, 0
  ret
vmx_launch ENDP

RestoreRip PROC
  mov rax, 1
  ret
RestoreRip ENDP

VmxVmcall PROC
  push r8
  push r9
  push r10
  push r11
  mov rax, 1003848372h
  mov r8, [rcx + 00h]
  mov r9, [rcx + 08h]
  mov r10, [rcx + 10h]
  mov r11, [rcx + 18h]
  vmcall
  pop r11
  pop r10
  pop r8
  pop r9
  ret
VmxVmcall ENDP

END