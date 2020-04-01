#include "pch.h"

#if _WIN32
#include <Windows.h>
#include <winrt/base.h>
void AppMain();

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    winrt::init_apartment();
    AppMain();
}
#endif
