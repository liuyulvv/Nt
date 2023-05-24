/**
 * @file NEventLoop.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-05-24
 */

#include "NEventLoop.h"

NEventLoop::NEventLoop() {
#if defined(_WIN32)
    auto instance = GetModuleHandle(nullptr);
    WNDCLASS window_class{};
    window_class.lpfnWndProc = EventProcess;
    window_class.hInstance = instance;
    RegisterClass(&window_class);
#endif
}

int NEventLoop::Exec() {
#if defined(_WIN32)
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#endif
    return 0;
}

LRESULT NEventLoop::EventProcess(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
    switch (msg) {
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, w_param, l_param);
}
