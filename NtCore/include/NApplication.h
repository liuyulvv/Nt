#pragma once

/**
 * @file NApplication.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-05-24
 */

#include "NPlatform.h"

class NEventLoop;

class BDllExport NApplication {
public:
    NApplication();
    ~NApplication() = default;
    NApplication(const NApplication& application) = delete;
    NApplication(NApplication&& application) = delete;
    NApplication& operator=(const NApplication& application) = delete;
    NApplication& operator=(NApplication&& application) = delete;

public:
    int Exec();

private:
    NEventLoop* event_loop_{};
};