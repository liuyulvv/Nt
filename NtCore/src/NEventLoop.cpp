/**
 * @file NEventLoop.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-05-24
 */

#include "NEventLoop.h"

#include "NCanvas.h"

NEventLoop::NEventLoop() {
#if defined(_WIN32)
    auto instance = GetModuleHandle(nullptr);
    WNDCLASS window_class{};
    window_class.lpfnWndProc = EventProcess;
    window_class.hInstance = instance;
    window_class.lpszClassName = N_CLASS_NAME;
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
    auto* canvas = reinterpret_cast<NCanvas*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    switch (msg) {
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
        case WM_MOVE: {
            auto x = static_cast<int32_t>(LOWORD(l_param));
            auto y = static_cast<int32_t>(HIWORD(l_param));
            canvas->MoveEvent({x, y});
            return 0;
        }
        case WM_SIZE: {
            auto width = static_cast<uint32_t>(LOWORD(l_param));
            auto height = static_cast<uint32_t>(HIWORD(l_param));
            canvas->ResizeEvent({width, height});
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, w_param, l_param);
}
