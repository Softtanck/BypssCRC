// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <malloc.h>
#include "MemAccess.h"
#include <tchar.h> 
#include <stdio.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

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
UINT32 Memorycrc_check1 = 0x12596CB;
UINT32 Memorycrc_retn1 = 0x12597CC;
UINT32 Memorycrc_check2 = 0x1033692;
UINT32 Memorycrc_retn2 = Memorycrc_check2 + 0x10;


bool _isLoaded;

#define NAKED __declspec(naked) void __stdcall 

NAKED crc_check2()
{
	__asm {

		mov esi, [esi];
		cmp esi, image_end;
		jg nobypass;
		cmp esi, image_base;
		jl nobypass;
		sub esi, image_base;
		add esi, image_copy;
		

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
		mov esi, [esi];
		mov edx, ebp;
		sub ax, 0x2048;
		add edx, 0x0000000C;
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
		/*
	image_base = reinterpret_cast<unsigned char*>(GetModuleHandle(L"KartRider.exe")) + 0xD00000;
	//IMAGE_NT_HEADERS* nt_header = PIMAGE_NT_HEADERS(image_base + PIMAGE_DOS_HEADER(image_base)->e_lfanew);
	image_copy = reinterpret_cast<unsigned char*>(malloc(0x200000));
	memcpy(image_copy, image_base, 0x200000);
	image_end = image_base + 0x200000;
	*/

	/*
	image_base = reinterpret_cast<unsigned char*>(GetModuleHandle(L"KartRider.exe")) +Memorycrc_check1;
	//IMAGE_NT_HEADERS* nt_header = PIMAGE_NT_HEADERS(image_base + PIMAGE_DOS_HEADER(image_base)->e_lfanew);
	image_copy = reinterpret_cast<unsigned char*>(malloc(0x200000));
	memcpy(image_copy, image_base, 0x200000);
	image_end = image_base + 0x200000;
	*/

	// 弹出调试信息
	//TCHAR  sz[256];
	//_stprintf(sz, L"CRC 拷贝地址:%x,CRC2作弊地址:%x", image_copy, (DWORD)crc_check2);
	//_stprintf(sz, L"CRC 拷贝地址:%x,CRC拷贝结束地址:%x", image_base, image_end);
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
		char cProcessName[MAX_PATH] = { 0 };
		if (!GetModuleFileNameA(GetModuleHandleA(NULL), cProcessName, MAX_PATH))
		{
			::MessageBox(NULL, L"GetModuleFileNameA failed", L"Notice", 0);
			return FALSE;
		}

		if (StrStrIA(cProcessName, "AdBalloonExt.exe"))
		{
			exit(1);
			return TRUE;
		}
		_isLoaded = true;
		Initialize_bypasscrc_t();
		DisableThreadLibraryCalls(hModule);
	}

	return TRUE;

}
