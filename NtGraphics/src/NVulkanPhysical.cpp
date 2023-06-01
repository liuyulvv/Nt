/**
 * @file NVulkanPhysical.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-05-31
 */

#include "NVulkanPhysical.h"

#include <string>
#include <unordered_set>

#include "NVulkanInstance.h"

NVulkanPhysical::NVulkanPhysical() {
    auto devices = NVulkanInstance::Singleton().EnumeratePhysicalDevices();
    for (const auto& device : devices) {
        if (IsPhysicalDeviceSuitable(device)) {
            physical_ = device;
            return;
        }
    }
    throw std::runtime_error("Failed to find a suitable GPU.");
}

NVulkanPhysical::QueueFamilyIndices NVulkanPhysical::QueueFamilies() const {
    return FindQueueFamilies(physical_);
}

const std::vector<const char*>& NVulkanPhysical::DeviceExtensions() const {
    return device_extensions_;
}

vk::Device NVulkanPhysical::CreateDevice(const vk::DeviceCreateInfo& info) {
    return physical_.createDevice(info);
}

NVulkanPhysical::SwapchainSupportDetails NVulkanPhysical::QuerySwapchainSupport(const vk::SurfaceKHR& surface) {
    SwapchainSupportDetails details{};
    details.capabilities_ = physical_.getSurfaceCapabilitiesKHR(surface);
    details.formats_ = physical_.getSurfaceFormatsKHR(surface);
    details.present_modes_ = physical_.getSurfacePresentModesKHR(surface);
    return details;
}

vk::FormatProperties NVulkanPhysical::GetFormatProperties(const vk::Format& format) {
    return physical_.getFormatProperties(format);
}

vk::PhysicalDeviceMemoryProperties NVulkanPhysical::GetMemoryProperties() {
    return physical_.getMemoryProperties();
}

bool NVulkanPhysical::IsPhysicalDeviceSuitable(const vk::PhysicalDevice& device) const {
    auto indices = FindQueueFamilies(device);
    auto available_extensions = device.enumerateDeviceExtensionProperties();
    std::unordered_set<std::string> required_extensions{device_extensions_.begin(), device_extensions_.end()};
    for (const auto& extension : available_extensions) {
        required_extensions.erase(extension.extensionName);
    }
    bool extensions_supported = required_extensions.empty();
    auto supported_features = device.getFeatures();
    return indices && extensions_supported && supported_features.samplerAnisotropy;
}

NVulkanPhysical::QueueFamilyIndices NVulkanPhysical::FindQueueFamilies(const vk::PhysicalDevice& device) const {
    QueueFamilyIndices indices;
    auto properties = device.getQueueFamilyProperties();
    for (size_t i = 0; i < properties.size(); ++i) {
        const auto& property = properties[i];
        if (property.queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.graphics_family_ = static_cast<uint32_t>(i);
            indices.has_graphics_family_ = true;
        }
#if defined(_WIN32)
        if (device.getWin32PresentationSupportKHR(static_cast<uint32_t>(i))) {
            indices.present_family_ = static_cast<uint32_t>(i);
            indices.has_present_family_ = true;
        }
#endif
        if (indices) {
            break;
        }
    }
    return indices;
}
