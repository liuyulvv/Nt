#pragma once

/**
 * @file NSize.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-05-31
 */

#include <cstdint>

#include "NPlatform.h"

struct NDllExport NSize {
    uint32_t width_{0};
    uint32_t height_{0};
};
