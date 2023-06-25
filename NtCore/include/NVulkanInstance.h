#pragma once

/**
 * @file NVulkanInstance.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-05-31
 */

#include <vector>

#include "NVulkanHeader.h"

class NDllExport NVulkanInstance {
public:
    static NVulkanInstance& Singleton() {
        static NVulkanInstance instance;
        return instance;
    }

private:
    NVulkanInstance();

public:
    ~NVulkanInstance();
    NVulkanInstance(const NVulkanInstance& instance) = default;
    NVulkanInstance(NVulkanInstance&& instance) = delete;
    NVulkanInstance& operator=(const NVulkanInstance& instance) = default;
    NVulkanInstance& operator=(NVulkanInstance&& instance) = delete;

public:
    std::vector<vk::PhysicalDevice> EnumeratePhysicalDevices();
#if defined(_WIN32)
    vk::SurfaceKHR CreateSurface(const vk::Win32SurfaceCreateInfoKHR& info);
#endif

private:
    void CheckValidationLayerSupport() const;
    void CheckExtensionsSupport() const;
    std::vector<const char*> GetRequiredExtensions() const;
    static void PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& create_info);

private:
    vk::Instance instance_{};

private:
#if defined(NOT_DEBUG)
    bool enable_validation_layers_ = false;
#else
    bool enable_validation_layers_ = true;
    std::vector<const char*> validation_layers_{"VK_LAYER_KHRONOS_validation"};
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                        VkDebugUtilsMessageTypeFlagsEXT message_type,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                        void* user_data);
    vk::DebugUtilsMessengerEXT debug_utils_messenger_;
#endif  // NOT_DEBUG
};