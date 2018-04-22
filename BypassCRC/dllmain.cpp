// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <malloc.h>
#include "MemAccess.h"
#include <tchar.h> 
#include <stdio.h>

PUCHAR image_base = nullptr;
PUCHAR image_end = nullptr;
PUCHAR image_copy = nullptr;

/*
//台服
UINT32 Memorycrc_check1 = 0x0121A4A1;
UINT32 Memorycrc_retn1 = 0x0121A5B8;
UINT32 Memorycrc_check2 = 0xFD0594;
UINT32 Memorycrc_retn2 = Memorycrc_check2 + 0x8;
*/

#ifndef LIB_H
 #define LIB_H
 extern "C" void __declspec(dllexport) Initialize_bypasscrc_t();
 #endif

 
 
 //国服
UINT32 Memorycrc_check1 = 0x01224176;
UINT32 Memorycrc_retn1 = 0x0122426C;
UINT32 Memorycrc_check2 = 0x0102E7BA;
UINT32 Memorycrc_retn2 = Memorycrc_check2 + 0x7;


bool _isLoaded;

#define NAKED __declspec(naked) void __stdcall 

NAKED crc_check2()
{
	__asm {

		mov eax, [eax];
		cmp eax, image_end;
		jg nobypass;
		cmp eax, image_base;
		jl nobypass;
		sub eax, image_base;
		add eax, image_copy;
		

		/*
		老的
		cmp ecx, image_end;
		jg nobypass;
		cmp ecx, image_base;
		jl nobypass;
		sub ecx, image_base;
		add ecx, image_copy;
		*/
	nobypass:
		/**
		mov eax,[eax]
		add [ebx],eax

		mov ecx, dword ptr[ecx];
		add dword ptr [edi], ecx;
		mov esi, ebp;
		jmp[Memorycrc_retn2];
		*/

		//测试
		//mov esi, 0x00000200;
		//mov edi, 0x00000001;
		//sub edi, 0x00000080;
		//add ebx, ebp;
		//国服不需要这一句
		//xor edx, 0x00000080;
		jmp[Memorycrc_retn2];

		/*
		新的
		mov eax, [eax];
		add[ebx], eax;
		jmp[Memorycrc_retn2];
		*/


		/*
		老的
		mov ecx, dword ptr[ecx];
		add dword ptr[edi], ecx;
		mov esi, ebp;
		jmp[Memorycrc_retn2];
		*/

	}
}


void Initialize_bypasscrc_t()
{
	image_base = reinterpret_cast<unsigned char*>(GetModuleHandle(L"KartRider.exe"));
	IMAGE_NT_HEADERS* nt_header = PIMAGE_NT_HEADERS(image_base + PIMAGE_DOS_HEADER(image_base)->e_lfanew);
	image_copy = reinterpret_cast<unsigned char*>(malloc(nt_header->OptionalHeader.SizeOfImage));
	memcpy(image_copy, image_base, nt_header->OptionalHeader.SizeOfImage);
	image_end = image_base + nt_header->OptionalHeader.SizeOfImage;

	// 弹出调试信息
	//TCHAR  sz[256];
	//_stprintf(sz, L"CRC 拷贝地址:%x,CRC2作弊地址:%x", image_copy, (DWORD)crc_check2);
	//MessageBox(NULL, sz, TEXT("标题"), MB_OK);

	WriteJump((void *)Memorycrc_check1, (void *)Memorycrc_retn1);
	WriteJump((void *)Memorycrc_check2, (void *)(DWORD)crc_check2);
}

/*
long KsCreateAllocator(long var1, long var2, long var3)
{
	return 0;
}

long KsCreateClock(long var1, long var2, long var3)
{
	return 0;
}

long KsCreatePin(long var1, long var2, long var3, long var4)
{
	return 0;
}

long KsCreateTopologyNode(long var1, long var2, long var3, long var4)
{
	return 0;
}*/

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{


	if (_isLoaded == false && ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		_isLoaded = true;
		Initialize_bypasscrc_t();
		DisableThreadLibraryCalls(hModule);
	}

	return TRUE;

}
