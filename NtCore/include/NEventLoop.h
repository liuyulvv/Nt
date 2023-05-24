#pragma once

/**
 * @file NEventLoop.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-05-24
 */

#include "NPlatform.h"

class BDllExport NEventLoop {
public:
    NEventLoop();
    ~NEventLoop() = default;
    NEventLoop(const NEventLoop& event_loop) = delete;
    NEventLoop(NEventLoop&& event_loop) = delete;
    NEventLoop& operator=(const NEventLoop& event_loop) = delete;
    NEventLoop& operator=(NEventLoop&& event_loop) = delete;

public:
    int Exec();

#if defined(_WIN32)
private:
    static LRESULT CALLBACK EventProcess(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);
#endif
};