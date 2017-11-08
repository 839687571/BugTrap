/*
 * This is a part of the BugTrap package.
 * Copyright (c) 2005-2009 IntelleSoft.
 * All rights reserved.
 *
 * Description: Log stream class.
 * Author: Maksim Pyatkovskiy.
 *
 * This source code is only intended as a supplement to the
 * BugTrap package reference and related electronic documentation
 * provided with the product. See these sources for detailed
 * information regarding the BugTrap package.
 */

#include "StdAfx.h"
#include "LogStream.h"
#include "TextFormat.h"

// use 20byte to store write size
#define  HEADER_BYTE_SIZE   20

/*


*/


SYSTEMTIME SystemTimeAdd(SYSTEMTIME src,long long  seconds)
{
	//SYSTEMTIME stTarget;
	//GetLocalTime(&stTarget);

	FILETIME ftTarget;
	SystemTimeToFileTime(&src, &ftTarget);

// 	WCHAR buf[256];
// 	swprintf_s(buf, L"[%02d:%02d:%02d:%d]\t\n", stTarget.wHour, stTarget.wMinute, stTarget.wSecond, stTarget.wMilliseconds);
// 	OutputDebugStringW(buf);

	ULARGE_INTEGER  ulTarget;

	ulTarget.HighPart = ftTarget.dwHighDateTime;
	ulTarget.LowPart = ftTarget.dwLowDateTime;

	ulTarget.QuadPart += seconds * 10000000i64;

	ftTarget.dwHighDateTime = ulTarget.HighPart;
	ftTarget.dwLowDateTime = ulTarget.LowPart;

	SYSTEMTIME nT;
	FileTimeToSystemTime(&ftTarget, &nT);

// 	 buf[256];
// 	swprintf_s(buf, L"[%02d:%02d:%02d:%d]\t\n", nT.wHour, nT.wMinute, nT.wSecond, nT.wMilliseconds);
// 	OutputDebugStringW(buf);
	return nT;
}


time_t SystemTimeToTimeTs(SYSTEMTIME& st)
{
	SYSTEMTIME stRef{};
	stRef.wYear = 1970;
	stRef.wMonth = 1;
	stRef.wDay = 1;
	stRef.wHour = 0;

	FILETIME ftRef;
	SystemTimeToFileTime(&stRef, &ftRef);

	FILETIME lftRef;
	FileTimeToLocalFileTime(&ftRef, &lftRef);

	// target time, convert to FILETIME
	//SYSTEMTIME stTarget;
	//GetLocalTime(&stTarget);

	FILETIME ftTarget;
	SystemTimeToFileTime(&st, &ftTarget);

	// convert both to ULARGE_INTEGER
	ULARGE_INTEGER ulRef, ulTarget;
	ulRef.HighPart = lftRef.dwHighDateTime;
	ulRef.LowPart = lftRef.dwLowDateTime;
	ulTarget.HighPart = ftTarget.dwHighDateTime;
	ulTarget.LowPart = ftTarget.dwLowDateTime;


	// subtract reference time from target time
	ulTarget.QuadPart -= ulRef.QuadPart;

	// convert to seconds (divide by 10000000)
	DWORD dwSecondsSinceRefTime = ulTarget.QuadPart / 10000000i64;

	return dwSecondsSinceRefTime;
}


DWORD CLogStream::GetLogSizeInBytes(void) const
{
	return m_SizeInBytes;
}

BOOL CLogStream::SetLogSizeInBytes(DWORD dwLogSizeInBytes)
{
	m_SizeInBytes = dwLogSizeInBytes;
	return TRUE;
}
BOOL CLogStream::SetTimestampFromInternet(long long tm)
{
	m_TimeOffset = tm;

	SYSTEMTIME st;
	GetLocalTime(&st);

	time_t t = SystemTimeToTimeTs(st);

	m_TimeOffset = tm - t;

	return TRUE;
}



/**
 * @return true if operation was completed successfully.
 */
BOOL CLogStream::LoadEntries(void)
{
	_ASSERTE(m_hFile == INVALID_HANDLE_VALUE);
	if (m_hFile != INVALID_HANDLE_VALUE)
		return FALSE;
	PCTSTR pszLogFileName = GetLogFileName();
	m_hFile = CreateFile(pszLogFileName, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	DWORD dwFileSize = GetFileSize(m_hFile, NULL);
	if (dwFileSize == 0)
	{
		DWORD dwWritten;
		if (! WriteFile(m_hFile, g_arrUTF8Preamble, sizeof(g_arrUTF8Preamble), &dwWritten, NULL))
			goto error;
	}
	else
	{
		if (dwFileSize > 10 * 1024 * 1024) {// lager than 10M ,we clear data.
			if (SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
				goto error;
		}
		if (SetFilePointer(m_hFile, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER)
			goto error;
	}
	return TRUE;

error:
	Close();
	return FALSE;
}

/**
 * @return true if operation was completed successfully.
 */
BOOL CLogStream::ClearEntries(void)
{
	_ASSERTE(m_hFile != INVALID_HANDLE_VALUE);
	if (m_hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	if (SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return FALSE;
	if (! SetEndOfFile(m_hFile))
		return FALSE;
	return TRUE;
}

void CLogStream::Close(void)
{
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}


/**
 * @param eLogLevel - log level number.
 * @param eEntryMode - entry mode.
 * @param rcsConsoleAccess - provides synchronous access to the console.
 * @param pszEntry - log entry text.
 * @return true if operation was completed successfully.
 */
BOOL CLogStream::WriteLogEntry(BUGTRAP_LOGLEVEL eLogLevel, ENTRY_MODE eEntryMode, CRITICAL_SECTION& rcsConsoleAccess, PCTSTR pszEntry)
{
	_ASSERTE(m_hFile != INVALID_HANDLE_VALUE);
	if (m_hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	_ASSERTE(eEntryMode == EM_APPEND);
	if (eEntryMode != EM_APPEND)
		return FALSE;
	BUGTRAP_LOGLEVEL eLogFileLevel = GetLogLevel();
	if (eLogLevel <= eLogFileLevel)
	{
		SYSTEMTIME st;
		GetLocalTime(&st);

		SYSTEMTIME stInternet = SystemTimeAdd(st, m_TimeOffset);

		if (!WriteLogEntryToConsole(eLogLevel, &stInternet, rcsConsoleAccess, pszEntry))
			FillEntryText(eLogLevel, &stInternet, pszEntry);
		EncodeEntryText();
		const BYTE* pBuffer = m_MemStream.GetBuffer();
		if (pBuffer == NULL)
			return FALSE;
		DWORD dwLength = (DWORD)m_MemStream.GetLength();
		_ASSERTE(dwLength > 0);
		DWORD dwWritten;

		DWORD dwFilePos = SetFilePointer(m_hFile, 0, NULL, FILE_CURRENT);
		if (dwFilePos == INVALID_SET_FILE_POINTER) {
				char buf[256];
				sprintf_s(buf, 256, "SetFilePointer get error = %d\n", GetLastError());
				WriteFile(m_hFile, buf, strlen(buf), &dwWritten, NULL);

		}
		if(m_SizeInBytes > 0){
			if (dwFilePos >  m_SizeInBytes) {
				DWORD dwSet = SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);
				BYTE buf[20];
				BOOL rRet = ReadFile(m_hFile, buf, 20, &dwWritten, NULL);
				if (rRet != 0) {
				
				}
				INT32  fileWrite = atoi((char*)buf);
				if (fileWrite > m_SizeInBytes) {
					fileWrite = HEADER_BYTE_SIZE;
				}

				dwSet = SetFilePointer(m_hFile, fileWrite, NULL, FILE_BEGIN);
				 if (dwSet == INVALID_SET_FILE_POINTER)
				 {
					 char buf[256];
					 sprintf_s(buf, 256, "SetFilePointer get error = %d\n", GetLastError());
					 WriteFile(m_hFile, buf, strlen(buf), &dwWritten, NULL);
				 }
			}
			BOOL ret  = WriteFile(m_hFile, pBuffer, dwLength, &dwWritten, NULL);
			dwFilePos = SetFilePointer(m_hFile, 0, NULL, FILE_CURRENT);
			char buf[HEADER_BYTE_SIZE] = { '0' };
			sprintf_s(buf,"%018d\n", dwFilePos);
			buf[19] = '0';
			SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);
			WriteFile(m_hFile, buf, HEADER_BYTE_SIZE, &dwWritten, NULL);
			SetFilePointer(m_hFile, dwFilePos, NULL, FILE_BEGIN);  /* 在文件开头写入当前 写入的行数, 第二次启动的时候 读取,然后在 写入的位置写入.*/

		}else {
			return WriteFile(m_hFile, pBuffer, dwLength, &dwWritten, NULL);
		}

	}
	return TRUE;
}
