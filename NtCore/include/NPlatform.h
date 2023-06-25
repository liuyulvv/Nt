#pragma once

/**
 * @file NPlatform.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-05-24
 */

#if defined(_WIN32)

#if !defined(UNICODE)
#define UNICODE
#endif  // UNICODE

#include <Windows.h>

#if !defined(NDllExport)
#define NDllExport __declspec(dllexport)
#else
#define NDllExport __attribute__((visibility("default")))
#endif

using NCanvasID = HWND;
static const wchar_t* N_PLATFORM_NAME{L"NWindows"};

#endif