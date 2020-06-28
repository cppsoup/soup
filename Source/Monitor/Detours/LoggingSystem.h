#pragma once

static bool s_bLog = 1;
static LONG s_nTlsIndent = -1;
static LONG s_nTlsThread = -1;
static LONG s_nThreadCnt = 0;

LONG EnterFunc()
{
	DWORD dwErr = GetLastError();

	LONG nIndent = 0;
	LONG nThread = 0;
	if (s_nTlsIndent >= 0)
	{
		nIndent = (LONG)(LONG_PTR)TlsGetValue(s_nTlsIndent);
		TlsSetValue(s_nTlsIndent, (PVOID)(LONG_PTR)(nIndent + 1));
	}

	if (s_nTlsThread >= 0)
	{
		nThread = (LONG)(LONG_PTR)TlsGetValue(s_nTlsThread);
	}

	SetLastError(dwErr);

	return nIndent;
}

void ExitFunc()
{
	DWORD dwErr = GetLastError();

	LONG nIndent = 0;
	LONG nThread = 0;
	if (s_nTlsIndent >= 0)
	{
		nIndent = (LONG)(LONG_PTR)TlsGetValue(s_nTlsIndent) - 1;
		// TODO: ASSERT(nIndent >= 0);
		TlsSetValue(s_nTlsIndent, (PVOID)(LONG_PTR)nIndent);
	}

	if (s_nTlsThread >= 0)
	{
		nThread = (LONG)(LONG_PTR)TlsGetValue(s_nTlsThread);
	}

	SetLastError(dwErr);
}

//////////////////////////////////////////////////////////////////////////////
//
// DLL module information
//
bool ThreadAttach(HMODULE hDll)
{
    (void)hDll;

    if (s_nTlsIndent >= 0) {
        TlsSetValue(s_nTlsIndent, (PVOID)0);
    }
    if (s_nTlsThread >= 0) {
        LONG nThread = InterlockedIncrement(&s_nThreadCnt);
        TlsSetValue(s_nTlsThread, (PVOID)(LONG_PTR)nThread);
    }
    return true;
}

bool ThreadDetach(HMODULE hDll)
{
    (void)hDll;

    if (s_nTlsIndent >= 0) {
        TlsSetValue(s_nTlsIndent, (PVOID)0);
    }
    if (s_nTlsThread >= 0) {
        TlsSetValue(s_nTlsThread, (PVOID)0);
    }
    return true;
}

inline void UpdateIfRoom(PWCHAR& pwzDst, PWCHAR pwzDstEnd, WCHAR c)
{
    if (pwzDst < pwzDstEnd) {
        *pwzDst++ = c;  // Write character if room in buffer.
    }
    else {
        pwzDst++;       // If no room, just advance pointer (to alloc calculation)
    }
}

static PCHAR RemoveReturns(PCHAR pszBuffer)
{
    PCHAR pszIn = pszBuffer;
    PCHAR pszOut = pszBuffer;

    while (*pszIn) {
        if (*pszIn == '\r') {
            pszIn++;
            continue;
        }
        *pszOut++ = *pszIn++;
    }
    *pszOut = '\0';

    return pszBuffer;
}

static PWCHAR RemoveReturns(PWCHAR pwzBuffer)
{
    PWCHAR pwzIn = pwzBuffer;
    PWCHAR pwzOut = pwzBuffer;

    while (*pwzIn) {
        if (*pwzIn == '\r') {
            pwzIn++;
            continue;
        }
        *pwzOut++ = *pwzIn++;
    }
    *pwzOut = '\0';

    return pwzBuffer;
}

PBYTE LoadFile(HANDLE hFile, DWORD cbFile)
{
    PBYTE pbFile = (PBYTE)GlobalAlloc(GPTR, cbFile + 3);
    if (pbFile == nullptr) {
        return nullptr;
    }

    DWORD cbRead = 0;
    Functions::Cache::SetFilePointer(hFile, 0, nullptr, FILE_BEGIN);
    Functions::Cache::ReadFile(hFile, pbFile, cbFile, &cbRead, nullptr);

    // Make sure the file is zero terminated.
    pbFile[cbRead + 0] = 0;
    pbFile[cbRead + 1] = 0;
    pbFile[cbRead + 2] = 0;

    return pbFile;
}