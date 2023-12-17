#include "stdafx.h"

#include <GLFW/glfw3.h>
#include <tchar.h>
#pragma warning(disable:4996)
extern int main(int, char**);
//extern int wmain(int, wchar_t**);


inline char* WideCharToMutiBytes(const wchar_t* wstr)
{
    std::mbstate_t mbstate = std::mbstate_t();
    const size_t len = 1 + std::wcsrtombs(nullptr, &wstr, 0, &mbstate);

    char* ret = new char[len];
    wcsrtombs(ret, &wstr, len, &mbstate);

    return ret;
}
inline void WideCharToMutiBytesFree(const char* str)
{
    delete[] str;
}
int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd
)
{
    int argc;
    LPWSTR* szArglist = CommandLineToArgvW(GetCommandLineW(), &argc);

    char** argv = new char* [argc + 1];

    for (int i = 0; i < argc; ++i)
    {
        argv[i] = WideCharToMutiBytes(szArglist[i]);
    }
    argv[argc] = nullptr;

    const int codeExit = main(argc, argv);

    for (int i = 0; i < argc; ++i)
    {
        WideCharToMutiBytesFree(argv[i]);
    }
    delete[] argv;

    LocalFree(szArglist);

    return codeExit;
}

//int WINAPI wWinMain(
//    _In_ HINSTANCE hInstance,
//    _In_opt_ HINSTANCE hPrevInstance,
//    _In_ LPWSTR lpCmdLine,
//    _In_ int nShowCmd
//)
//{
//    int argc;
//    LPWSTR* szArglist = CommandLineToArgvW(GetCommandLineW(), &argc);
//
//    wchar_t** argv = new wchar_t* [argc + 1];
//
//    for (int i = 0; i < argc; ++i)
//    {
//        argv[i] = szArglist[i];
//    }
//    argv[argc] = nullptr;
//
//    const int codeExit = wmain(argc, argv);
//
//    for (int i = 0; i < argc; ++i)
//    {
//        delete[] argv[i];
//    }
//    delete[] argv;
//
//    LocalFree(szArglist);
//
//    return codeExit;
//}
//