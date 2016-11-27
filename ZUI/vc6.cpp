#include <stdio.h>
#include <ZUI.h>
#if (defined NDEBUG) && (defined AGG)
void __cdecl operator delete(void* p, unsigned int)
{
	free(p);
}

void __cdecl operator delete[](void * p, unsigned int)
{
	free(p);
}

#endif

#if (defined NDEBUG)
extern "C" int __security_cookie = 0;     //比错误提示的名称少一个下划线

extern "C" __declspec(naked) __int64 _ftol2_sse(double v)
{
	__asm {
		push        ebp
			mov         ebp, esp
			sub         esp, 20h
			and         esp, 0FFFFFFF0h
			fld         st(0)
			fst         dword ptr[esp + 18h]
			fistp       qword ptr[esp + 10h]
			fild        qword ptr[esp + 10h]
			mov         edx, dword ptr[esp + 18h]
			mov         eax, dword ptr[esp + 10h]
			test        eax, eax
			je          integer_QnaN_or_zero
			arg_is_not_integer_QnaN :
		fsubp       st(1), st
			test        edx, edx
			jns         positive
			fstp        dword ptr[esp]
			mov         ecx, dword ptr[esp]
			xor ecx, 80000000h
			add         ecx, 7FFFFFFFh
			adc         eax, 0
			mov         edx, dword ptr[esp + 14h]
			adc         edx, 0
			jmp         localexit

			positive :
		fstp        dword ptr[esp]
			mov         ecx, dword ptr[esp]
			add         ecx, 7FFFFFFFh
			sbb         eax, 0
			mov         edx, dword ptr[esp + 14h]
			sbb         edx, 0
			jmp         localexit
			integer_QnaN_or_zero :
		mov         edx, dword ptr[esp + 14h]
			test        edx, 7FFFFFFFh
			jne         arg_is_not_integer_QnaN
			fstp        dword ptr[esp + 18h]
			fstp        dword ptr[esp + 18h]
			localexit :
			leave
			ret
	}
}
#endif