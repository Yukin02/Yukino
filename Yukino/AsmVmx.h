#pragma once
#include "main.h"
#include "Features.h"

extern inline SEGMENT_SELECTOR ReadCs();

extern inline SEGMENT_SELECTOR ReadSs();

extern inline SEGMENT_SELECTOR ReadDs();

extern inline SEGMENT_SELECTOR ReadEs();

extern inline SEGMENT_SELECTOR ReadFs();

extern inline SEGMENT_SELECTOR ReadGs();

extern inline SEGMENT_SELECTOR ReadTr();

extern inline SEGMENT_SELECTOR ReadLdtr();

extern inline void WriteDs(UINT16 selector);

extern inline void WriteEs(UINT16 selector);

extern inline void WriteCs(UINT16 selector);

extern inline void WriteSs(UINT16 selector);

extern inline void WriteFs(UINT16 selector);

extern inline void WriteGs(UINT16 selector);

extern inline void WriteTr(UINT16 selector);

extern inline void WriteLdtr(UINT16 selector);

extern inline void WriteCr2(UINT64 cr2);

extern inline void _sgdt(SEGMENT_DESCRIPTOR_REGISTER_64* gdtr);

extern inline void _lgdt(SEGMENT_DESCRIPTOR_REGISTER_64* gdtr);

extern inline UINT8 vmx_launch();

extern inline UINT8 RestoreRip();

extern inline void WriteTss(SEGMENT_DESCRIPTOR_REGISTER_64 Gdtr, UINT16 GuestTr);

extern inline UINT64 VmxVmcall(VMM_CALL* vmcall);

inline SEGMENT_DESCRIPTOR_REGISTER_64 GetGdt() { SEGMENT_DESCRIPTOR_REGISTER_64 Gdtr = { 0 }; _sgdt(&Gdtr); return (SEGMENT_DESCRIPTOR_REGISTER_64) { Gdtr.Limit, Gdtr.BaseAddress }; }

inline SEGMENT_DESCRIPTOR_REGISTER_64 GetIdt() { SEGMENT_DESCRIPTOR_REGISTER_64 Idtr = { 0 }; __sidt(&Idtr); return (SEGMENT_DESCRIPTOR_REGISTER_64) { Idtr.Limit, Idtr.BaseAddress }; }

inline void SetGdt(SEGMENT_DESCRIPTOR_REGISTER_64* Gdtr) { _lgdt(Gdtr); }
inline void SetIdt(SEGMENT_DESCRIPTOR_REGISTER_64* Idtr) { __lidt(Idtr); }

inline UINT64 vmx_vmread(UINT64 field) { UINT64 value = 0; __vmx_vmread(field, &value); return value; }

inline void vmx_vmwrite(UINT64 field, UINT64 value) { __vmx_vmwrite(field, value); }