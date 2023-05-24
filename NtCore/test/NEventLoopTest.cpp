/**
 * @file NEventLoopTest.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-05-24
 */

#include <iostream>

#include "NEventLoop.h"

int main() {
    auto event = new NEventLoop();
    event->Exec();
    return 0;
}