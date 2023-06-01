/**
 * @file NVulkanSwapchainTest.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-06-01
 */

#if defined(_WIN32)

#if !defined(UNICODE)
#define UNICODE
#endif  // UNICODE

#include <Windows.h>

#endif

#include "NVulkanSwapchain.h"

static const wchar_t* B_CLASS_NAME{L"Bt"};
static const wchar_t* TITLE{L"NVulkanSwapChainTest"};

static LRESULT CALLBACK EventProcess(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
    switch (msg) {
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, w_param, l_param);
}

int main() {
    auto instance = GetModuleHandle(nullptr);
    WNDCLASS window_class{};
    window_class.lpfnWndProc = EventProcess;
    window_class.hInstance = instance;
    window_class.lpszClassName = B_CLASS_NAME;
    RegisterClass(&window_class);

    auto hwnd = CreateWindowEx(
        0,
        B_CLASS_NAME,
        TITLE,
        WS_OVERLAPPEDWINDOW,
        760,
        390,
        400,
        300,
        nullptr,
        nullptr,
        nullptr,
        nullptr);

    ShowWindow(hwnd, 5);

    NVulkanSwapchain swapchain(hwnd, 400, 300);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}