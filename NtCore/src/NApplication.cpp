/**
 * @file NApplication.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-05-31
 */

#include "NApplication.h"

#include "NEventLoop.h"

NApplication::NApplication() : event_loop_(new NEventLoop()) {
}

int NApplication::Exec() {
    return event_loop_->Exec();
}
