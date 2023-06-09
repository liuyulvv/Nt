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

#if !defined(BDllExport)
#define BDllExport __declspec(dllexport)
#else
#define BDllExport __attribute__((visibility("default")))
#endif

using NCanvasID = HWND;
static const wchar_t* N_CLASS_NAME{L"NWindows"};

#endif