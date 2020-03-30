//reference: http://www.codeproject.com/Articles/207464/Exception-Handling-in-Visual-Cplusplus
#pragma once
#include "ComUtil.h"
namespace ExceptionCatcher
{
    //some exception handlers work on per process, others work on per thread
    //per process: SEH exception, pure virtual function call, C++ new exception, runtime invalid parameter error, signal: SIGABRT, SIGINT, SIGTERM
    //per thread: terminate() call, unexpected() call, signal: SIGFPE, SIGILL, SIGSEGV
	COMUTIL_API void SetProcessExceptionHandlers();
	COMUTIL_API void SetThreadExceptionHandlers();
}