#include <stdio.h>
#include <wchar.h>
#include <windows.h>

#ifndef COMMAND
    #error Define the command you want to compile for (i.e. pass '-DCOMMAND=L"\"program\" args"').
#endif

enum { COMMAND_LEN = sizeof COMMAND / sizeof *COMMAND - 1 };

static WCHAR gBuffer[32768];

static int prepare(void) {
    const WCHAR* args = GetCommandLineW();
    wmemcpy(gBuffer, COMMAND, COMMAND_LEN + 1);
    if (*args == L'"') {
        const WCHAR* const end = wcschr(args + 1, L'"');
        if (!end) /* WTF? */
            return 0;
        args = end + 1;
    } else {
        const WCHAR* const end = wcschr(args, L' ');
        if (!end) /* No arguments. */
            return 1;
        args = end;
    }
    wcsncpy(gBuffer + COMMAND_LEN, args, 32768 - COMMAND_LEN);
    return !gBuffer[32767];
}

#ifdef __GNUC__
__attribute__((format(printf, 1, 0)))
#endif
static int fail(const char msg[ ]) {
    fprintf(stderr, msg, (unsigned long)GetLastError());
    return 1;
}

int main(void) {
    STARTUPINFOW si = { 0 };
    PROCESS_INFORMATION pi;
    DWORD result;
    si.cb = sizeof si;
    if (!prepare())
        return fail("Too long command line (32767 characters max).\n");
    if (!CreateProcessW(NULL, gBuffer, NULL, NULL, FALSE, 0x0, NULL, NULL, &si, &pi))
        return fail("`CreateProcessW` failed with code %lu.\n");
    if (!CloseHandle(pi.hThread))
        return fail("`CloseHandle` failed with code %lu.\n");
    if (WaitForSingleObject(pi.hProcess, INFINITE) != WAIT_OBJECT_0)
        return fail("`WaitForSingleObject` failed with code %lu.\n");
    if (!GetExitCodeProcess(pi.hProcess, &result))
        return fail("`GetExitCodeProcess` failed with code %lu.\n");
    /*
    if (!CloseHandle(pi.hProcess))
        return fail("`CloseHandle` failed with code %lu.\n");
    */
    return (int)result;
}
