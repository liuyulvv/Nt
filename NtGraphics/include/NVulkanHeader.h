#pragma once

/**
 * @file NVulkanHeader.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-05-31
 */

#if defined(_WIN32)

#if !defined(UNICODE)
#define UNICODE
#endif  // UNICODE

#if !defined(BDllExport)
#define BDllExport __declspec(dllexport)
#else
#define BDllExport __attribute__((visibility("default")))
#endif

#if !defined(VK_USE_PLATFORM_WIN32_KHR)
#define VK_USE_PLATFORM_WIN32_KHR
#endif  // VK_USE_PLATFORM_WIN32_KHR

#endif

#include "vulkan/vulkan.hpp"