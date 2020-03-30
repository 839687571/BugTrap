//if you want to use this class separately,
//you should comment log sentences in "CreateMiniDump" and all handlers,
//and replace CSelfPath with your exe path getting function in "PrepareDumpFilePath"
#include "stdafx.h"

#include <string>
#include <new.h>
#include <signal.h>
#include "ExceptionCatcher.h"
#include "bugTrap/BugTrap.h"

#define InfoLog printf

//copy from invarg.c of VC9.0
#if defined (_AMD64_)

PRUNTIME_FUNCTION
RtlLookupFunctionEntry (
                        IN ULONG64 ControlPc,
                        OUT PULONG64 ImageBase,
                        IN OUT PVOID HistoryTable OPTIONAL
                        );

PVOID
RtlVirtualUnwind (
                  IN ULONG HandlerType,
                  IN ULONG64 ImageBase,
                  IN ULONG64 ControlPc,
                  IN PRUNTIME_FUNCTION FunctionEntry,
                  IN OUT PCONTEXT ContextRecord,
                  OUT PVOID *HandlerData,
                  OUT PULONG64 EstablisherFrame,
                  IN OUT PVOID ContextPointers OPTIONAL
                  );

#endif

namespace
{
#ifndef _M_IX86
#error "The following code only works for x86!"
#endif
	LPTOP_LEVEL_EXCEPTION_FILTER WINAPI MyDummySetUnhandledExceptionFilter(
		LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
	{
		return NULL;
	}

	BOOL PreventSetUnhandledExceptionFilter()
	{
		HMODULE hKernel32 = LoadLibrary(L"kernel32.dll");
		if (hKernel32 == NULL) return FALSE;
		void *pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
		if (pOrgEntry == NULL) return FALSE;
		unsigned char newJump[100];
		DWORD dwOrgEntryAddr = (DWORD)pOrgEntry;
		dwOrgEntryAddr += 5; // add 5 for 5 op-codes for jmp far
		void *pNewFunc = &MyDummySetUnhandledExceptionFilter;
		DWORD dwNewEntryAddr = (DWORD)pNewFunc;
		DWORD dwRelativeAddr = dwNewEntryAddr-dwOrgEntryAddr;

		newJump[0] = 0xE9;  // JMP absolute
		memcpy(&newJump[1], &dwRelativeAddr, sizeof(pNewFunc));
		SIZE_T bytesWritten;
		BOOL bRet = WriteProcessMemory(GetCurrentProcess(),
			pOrgEntry, newJump, sizeof(pNewFunc) + 1, &bytesWritten);
		return bRet;
	}

	LONG WINAPI MyUnhandledExceptionFilter(
	struct _EXCEPTION_POINTERS *lpTopLevelExceptionFilter)
	{
		// TODO: MiniDumpWriteDump
		FatalAppExit(0, L"Unhandled Exception occured!");
		return EXCEPTION_CONTINUE_SEARCH;
	}

    bool create_minidump(EXCEPTION_POINTERS* pExceptionPtrs)
    {
		BT_SehFilter(pExceptionPtrs);
        return true;
    }

    EXCEPTION_POINTERS* get_exception_pointers(const DWORD dwExceptionCode)
    {
        //copy from function _invoke_watson in invarg.c of VC9.0
        EXCEPTION_RECORD   ExceptionRecord = {0};
        CONTEXT ContextRecord;

#ifdef _X86_

        __asm {
            mov dword ptr [ContextRecord.Eax], eax
                mov dword ptr [ContextRecord.Ecx], ecx
                mov dword ptr [ContextRecord.Edx], edx
                mov dword ptr [ContextRecord.Ebx], ebx
                mov dword ptr [ContextRecord.Esi], esi
                mov dword ptr [ContextRecord.Edi], edi
                mov word ptr [ContextRecord.SegSs], ss
                mov word ptr [ContextRecord.SegCs], cs
                mov word ptr [ContextRecord.SegDs], ds
                mov word ptr [ContextRecord.SegEs], es
                mov word ptr [ContextRecord.SegFs], fs
                mov word ptr [ContextRecord.SegGs], gs
                pushfd
                pop [ContextRecord.EFlags]
        }

        ContextRecord.ContextFlags = CONTEXT_CONTROL;
#pragma warning(push)
#pragma warning(disable:4311)
        ContextRecord.Eip = (ULONG)_ReturnAddress();
        ContextRecord.Esp = (ULONG)_AddressOfReturnAddress();
#pragma warning(pop)
        ContextRecord.Ebp = *((ULONG *)_AddressOfReturnAddress()-1);

#elif defined (_AMD64_)

        ULONG64 ControlPc;
        ULONG64 EstablisherFrame;
        PRUNTIME_FUNCTION FunctionEntry;
        PVOID HandlerData;
        ULONG64 ImageBase;

        RtlCaptureContext(&ContextRecord);
        ControlPc = ContextRecord.Rip;
        FunctionEntry = RtlLookupFunctionEntry(ControlPc, &ImageBase, NULL);
        if (FunctionEntry != NULL) {
            RtlVirtualUnwind(/*UNW_FLAG_NHANDLER*/0x00,
                ImageBase,
                ControlPc,
                FunctionEntry,
                &ContextRecord,
                &HandlerData,
                &EstablisherFrame,
                NULL);
        } else {
            ContextRecord.Rip = (ULONGLONG) _ReturnAddress();
            ContextRecord.Rsp = (ULONGLONG) _AddressOfReturnAddress();
        }

#elif defined (_IA64_)

        /* Need to fill up the Context in IA64. */
        RtlCaptureContext(&ContextRecord);

#else  /* defined (_IA64_) */

        ZeroMemory(&ContextRecord, sizeof(ContextRecord));

#endif  /* defined (_IA64_) */


        ExceptionRecord.ExceptionCode = dwExceptionCode;
        ExceptionRecord.ExceptionFlags    = EXCEPTION_NONCONTINUABLE;
        ExceptionRecord.ExceptionAddress = _ReturnAddress();

        //end copy

        EXCEPTION_RECORD* pExceptionRecord = new EXCEPTION_RECORD;
        memcpy(pExceptionRecord, &ExceptionRecord, sizeof(EXCEPTION_RECORD));
        CONTEXT* pContextRecord = new CONTEXT;
        memcpy(pContextRecord, &ContextRecord, sizeof(CONTEXT));

        EXCEPTION_POINTERS* pExceptionPointers = new EXCEPTION_POINTERS;
        pExceptionPointers->ContextRecord = pContextRecord;
        pExceptionPointers->ExceptionRecord = pExceptionRecord;

        return pExceptionPointers;
    }


    //handlers
    LONG WINAPI catched_seh_handler(EXCEPTION_POINTERS* pExceptionPtrs)
    {
        InfoLog("SEH exception occurs");
        create_minidump(pExceptionPtrs);
        return EXCEPTION_CONTINUE_SEARCH;
    }

    void __cdecl catched_terminate_handler()
    {
        InfoLog("terminate call occurs");
		create_minidump(get_exception_pointers(0));
    }

    void __cdecl catched_unexpected_handler()
    {
        InfoLog("unexpected call occurs");
        create_minidump(get_exception_pointers(0));
    }

    void __cdecl catched_pure_call_handler()
    {
        InfoLog("pure virtual function call occurs");
        create_minidump(get_exception_pointers(0));
    }

    void __cdecl catched_invalid_parameter_handler(const wchar_t* expression,
        const wchar_t* function,
        const wchar_t* file,
        unsigned int line,
        uintptr_t pReserved)
    {
        pReserved;
        InfoLog("invalid parameter call occurs, expression[%s], function[%s], file[%s], line[%d]",
            expression, function, file, line);
        create_minidump(get_exception_pointers(0));
    }

    int __cdecl catched_new_handler(size_t)
    {
        InfoLog("new operator memory allocation exception occurs");
        create_minidump(get_exception_pointers(0));
        return 0;
    }

    void catched_sigabrt_handler(int)
    {
        InfoLog("signal SIGABRT occurs");
        create_minidump(get_exception_pointers(0));
    }

    void catched_sigfpe_handler(int code, int subcode)
    {
        InfoLog("signal SIGFPE occurs, code[%d], subcode[%d]", code, subcode);
        create_minidump(reinterpret_cast<EXCEPTION_POINTERS *>(_pxcptinfoptrs));
    }

    void catched_sigint_handler(int)
    {
        InfoLog("signal SIGINT occurs");
        create_minidump(get_exception_pointers(0));
    }

    void catched_sigill_handler(int)
    {
        InfoLog("signal SIGILL occurs");
        create_minidump(get_exception_pointers(0));
    }

    void catched_sigsegv_handler(int)
    {
        InfoLog("signal SIGSEGV occurs");
        create_minidump(reinterpret_cast<EXCEPTION_POINTERS *>(_pxcptinfoptrs));
    }

    void catched_sigterm_handler(int)
    {
        InfoLog("signal SIGTERM occurs");
        create_minidump(get_exception_pointers(0));
    }
    //end handlers
}

//http://blog.kalmbachnet.de/?postid=75
void ExceptionCatcher::SetProcessExceptionHandlers()
{
    // Install top-level SEH handler
//	SetUnhandledExceptionFilter(catched_seh_handler);
//	BOOL bRet = PreventSetUnhandledExceptionFilter();

	//AddVectoredExceptionHandler()
    // Catch pure virtual function calls.
    // Because there is one _purecall_handler for the whole process,
    // calling this function immediately impacts all threads. The last
    // caller on any thread sets the handler.
    // http://msdn.microsoft.com/en-us/library/t296ys27.aspx
    _set_purecall_handler(catched_pure_call_handler);

    // Catch new operator memory allocation exceptions
    _set_new_handler(catched_new_handler);

    // Catch invalid parameter exceptions.
    _set_invalid_parameter_handler(catched_invalid_parameter_handler);

    // Set up C++ signal handlers

    _set_abort_behavior(_CALL_REPORTFAULT, _CALL_REPORTFAULT);

    // Catch an abnormal program termination
    signal(SIGABRT, catched_sigabrt_handler);

    // Catch illegal instruction handler
    signal(SIGINT, catched_sigint_handler);

    // Catch a termination request
    signal(SIGTERM, catched_sigterm_handler);
}

void ExceptionCatcher::SetThreadExceptionHandlers()
{
	// 在调试模式下,不生成dump
	if (::IsDebuggerPresent())
		return;
    // Catch terminate() calls.
    // In a multithreaded environment, terminate functions are maintained
    // separately for each thread. Each new thread needs to install its own
    // terminate function. Thus, each thread is in charge of its own termination handling
    // http://msdn.microsoft.com/en-us/library/t6fk7h29.aspx
    set_terminate(catched_terminate_handler);

    // Catch unexpected() calls.
    // In a multithreaded environment, unexpected functions are maintained
    // separately for each thread. Each new thread needs to install its own
    // unexpected function. Thus, each thread is in charge of its own unexpected handling.
    // http://msdn.microsoft.com/en-us/library/h46t5b69.aspx
    set_unexpected(catched_unexpected_handler);

    // Catch a floating point error
    typedef void (*sigh)(int);
    signal(SIGFPE, (sigh)catched_sigfpe_handler);

    // Catch an illegal instruction
    signal(SIGILL, catched_sigill_handler);

    // Catch illegal storage access errors
#ifdef ISRELEASE
	signal(SIGSEGV, catched_sigsegv_handler);
#endif

}


