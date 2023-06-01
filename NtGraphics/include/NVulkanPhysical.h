#pragma once

/**
 * @file NVulkanPhysical.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-05-31
 */

#include <vector>

#include "NVulkanHeader.h"

class BDllExport NVulkanPhysical {
public:
    struct QueueFamilyIndices {
        uint32_t graphics_family_;
        uint32_t present_family_;

        bool has_graphics_family_ = false;
        bool has_present_family_ = false;

        operator bool() {
            return has_graphics_family_ && has_present_family_;
        }
    };

    struct SwapchainSupportDetails {
        vk::SurfaceCapabilitiesKHR capabilities_;
        std::vector<vk::SurfaceFormatKHR> formats_;
        std::vector<vk::PresentModeKHR> present_modes_;
    };

public:
    static NVulkanPhysical& Singleton() {
        static NVulkanPhysical physical;
        return physical;
    }

private:
    NVulkanPhysical();

public:
    ~NVulkanPhysical() = default;
    NVulkanPhysical(const NVulkanPhysical& physical) = default;
    NVulkanPhysical(NVulkanPhysical&& physical) = delete;
    NVulkanPhysical& operator=(const NVulkanPhysical& physical) = default;
    NVulkanPhysical& operator=(NVulkanPhysical&& physical) = delete;

public:
    QueueFamilyIndices QueueFamilies() const;
    const std::vector<const char*>& DeviceExtensions() const;
    vk::Device CreateDevice(const vk::DeviceCreateInfo& info);
    SwapchainSupportDetails QuerySwapchainSupport(const vk::SurfaceKHR& surface);
    vk::FormatProperties GetFormatProperties(const vk::Format& format);
    vk::PhysicalDeviceMemoryProperties GetMemoryProperties();

private:
    bool IsPhysicalDeviceSuitable(const vk::PhysicalDevice& device) const;
    QueueFamilyIndices FindQueueFamilies(const vk::PhysicalDevice& device) const;

private:
    vk::PhysicalDevice physical_{};

private:
#if defined(_WIN32)
    std::vector<const char*> device_extensions_ = {"VK_KHR_swapchain"};
#else
    std::vector<const char*> device_extensions_ = {"VK_KHR_portability_subset", "VK_KHR_swapchain"};
#endif
};