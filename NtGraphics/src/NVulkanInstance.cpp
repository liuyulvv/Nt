/**
 * @file NVulkanInstance.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-05-31
 */

#include "NVulkanInstance.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_set>

NVulkanInstance::NVulkanInstance() {
    CheckValidationLayerSupport();
    CheckExtensionsSupport();
    vk::ApplicationInfo app_info{};
    auto extensions = GetRequiredExtensions();
    vk::InstanceCreateInfo create_info{};
    create_info
        .setPApplicationInfo(&app_info)
        .setFlags(vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR)
        .setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()))
        .setPEnabledExtensionNames(extensions);
    if (enable_validation_layers_) {
        vk::DebugUtilsMessengerCreateInfoEXT debug_create_info{};
        PopulateDebugMessengerCreateInfo(debug_create_info);
        create_info
            .setEnabledLayerCount(static_cast<uint32_t>(validation_layers_.size()))
            .setPEnabledLayerNames(validation_layers_)
            .setPNext(&debug_create_info);
        instance_ = vk::createInstance(create_info);
        debug_utils_messenger_ = instance_.createDebugUtilsMessengerEXT(debug_create_info, nullptr, vk::DispatchLoaderDynamic(instance_, reinterpret_cast<PFN_vkGetInstanceProcAddr>(instance_.getProcAddr("vkGetInstanceProcAddr"))));
    } else {
        create_info
            .setEnabledLayerCount(0)
            .setPNext(nullptr);
        instance_ = vk::createInstance(create_info);
    }
}

std::vector<vk::PhysicalDevice> NVulkanInstance::EnumeratePhysicalDevices() {
    return instance_.enumeratePhysicalDevices();
}

#if defined(_WIN32)
vk::SurfaceKHR NVulkanInstance::CreateSurface(const vk::Win32SurfaceCreateInfoKHR& info) {
    return instance_.createWin32SurfaceKHR(info);
}
#endif

void NVulkanInstance::CheckValidationLayerSupport() const {
    if (enable_validation_layers_) {
        auto available_layers = vk::enumerateInstanceLayerProperties();
        for (const auto& layer_name : validation_layers_) {
            bool layer_found = false;
            for (const vk::LayerProperties& layer_properties : available_layers) {
                if (std::string(layer_properties.layerName.data()) == layer_name) {
                    layer_found = true;
                    break;
                }
            }
            if (!layer_found)
                throw std::runtime_error(std::string(layer_name) + "layer requested, but not available.");
        }
    }
}

void NVulkanInstance::CheckExtensionsSupport() const {
    auto extensions = vk::enumerateInstanceExtensionProperties();
    std::unordered_set<std::string> available;
    for (const auto& extension : extensions) {
        available.insert(extension.extensionName);
    }
    auto required_extensions = GetRequiredExtensions();
    for (const auto& required : required_extensions) {
        if (!available.contains(required)) {
            throw std::runtime_error("Missing required extension: " + std::string(required) + ".");
        }
    }
}

std::vector<const char*> NVulkanInstance::GetRequiredExtensions() const {
#if defined(_WIN32)
    std::vector<const char*> extensions{"VK_KHR_surface", "VK_KHR_win32_surface"};
#endif
    if (enable_validation_layers_) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    return extensions;
}

void NVulkanInstance::PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& create_info) {
    create_info
        .setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                            vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo)
        .setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding |
                        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation)
        .setPfnUserCallback(&DebugCallback)
        .setPUserData(nullptr);
}

VKAPI_ATTR VkBool32 VKAPI_CALL NVulkanInstance::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, [[maybe_unused]] void* user_data) {
    std::string message(callback_data->pMessage);
    std::string type;
    std::string severity;
    if (!message.empty()) {
        switch (message_type) {
            case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: {
                type = "GENERAL";
                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: {
                type = "VALIDATION";
                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: {
                type = "PERFORMANCE";
                break;
            }
        }
        switch (message_severity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
                severity = "WARN";
                std::cerr << "[" << severity << "]"
                          << " Validation layer(" << type << "): " << message << std::endl;
                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
                severity = "ERROR";
                std::cerr << "[" << severity << "]"
                          << " Validation layer(" << type << "): " << message << std::endl;
                break;
            }
            default: {
                severity = "INFO";
                break;
            }
        }
    }
    return VK_FALSE;
}
