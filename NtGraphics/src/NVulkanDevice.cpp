/**
 * @file NVulkanDevice.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-06-01
 */

#include "NVulkanDevice.h"

#include <vector>

#include "NVulkanInstance.h"
#include "NVulkanPhysical.h"

NVulkanDevice::NVulkanDevice() {
    CreateDevice();
    CreateCommandPool();
}

vk::SwapchainKHR NVulkanDevice::CreateSwapchain(const vk::SwapchainCreateInfoKHR& info) {
    return device_.createSwapchainKHR(info);
}

std::vector<vk::Image> NVulkanDevice::GetSwapchainImages(const vk::SwapchainKHR& swapchain) {
    return device_.getSwapchainImagesKHR(swapchain);
}

vk::ImageView NVulkanDevice::CreateImageView(const vk::Image& image, const vk::Format& format, vk::ImageAspectFlagBits flag) {
    vk::ImageViewCreateInfo view_info{};
    view_info
        .setImage(image)
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(format);
    view_info.subresourceRange
        .setAspectMask(flag)
        .setBaseMipLevel(0)
        .setLevelCount(1)
        .setBaseArrayLayer(0)
        .setLayerCount(1);
    return device_.createImageView(view_info);
}

vk::Format NVulkanDevice::FindSupportFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, const vk::FormatFeatureFlags& features) const {
    for (const auto& format : candidates) {
        auto properties = NVulkanPhysical::Singleton().GetFormatProperties(format);
        if (tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features) {
            return format;
        }
    }
    throw std::runtime_error("No supported format found.");
}

vk::RenderPass NVulkanDevice::CreateRenderPass(const vk::RenderPassCreateInfo& info) {
    return device_.createRenderPass(info);
}

void NVulkanDevice::CreateImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, const vk::ImageUsageFlags& usage, const vk::MemoryPropertyFlags& properties, vk::Image& image, vk::DeviceMemory& memory) {
    vk::ImageCreateInfo image_info{};
    image_info
        .setImageType(vk::ImageType::e2D)
        .setMipLevels(1)
        .setArrayLayers(1)
        .setFormat(format)
        .setTiling(tiling)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setUsage(usage)
        .setSharingMode(vk::SharingMode::eExclusive)
        .setSamples(vk::SampleCountFlagBits::e1);
    image_info.extent
        .setWidth(width)
        .setHeight(height)
        .setDepth(1);
    image = device_.createImage(image_info);
    auto memory_requirements = device_.getImageMemoryRequirements(image);
    vk::MemoryAllocateInfo allocate_info{};
    allocate_info
        .setAllocationSize(memory_requirements.size)
        .setMemoryTypeIndex(FindMemoryType(memory_requirements.memoryTypeBits, properties));
    memory = device_.allocateMemory(allocate_info);
    device_.bindImageMemory(image, memory, 0);
}

vk::Framebuffer NVulkanDevice::CreateFramebuffer(const vk::FramebufferCreateInfo& info) {
    return device_.createFramebuffer(info);
}

vk::Semaphore NVulkanDevice::CreateSemaphore(const vk::SemaphoreCreateInfo& info) {
    return device_.createSemaphore(info);
}

vk::Fence NVulkanDevice::CreateFence(const vk::FenceCreateInfo& info) {
    return device_.createFence(info);
}

void NVulkanDevice::WaitIdle() const {
    device_.waitIdle();
}

const vk::CommandPool& NVulkanDevice::CommandPool() const {
    return command_pool_;
}

std::vector<vk::CommandBuffer> NVulkanDevice::AllocateCommandBuffers(const vk::CommandBufferAllocateInfo& info) {
    return device_.allocateCommandBuffers(info);
}

void NVulkanDevice::CreateDevice() {
    auto indices = NVulkanPhysical::Singleton().QueueFamilies();
    auto queue_priority = 1.0F;
    std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
    if (indices.graphics_family_ == indices.present_family_) {
        vk::DeviceQueueCreateInfo queue_create_info{};
        queue_create_info
            .setQueueCount(1)
            .setQueueFamilyIndex(indices.graphics_family_)
            .setQueuePriorities(queue_priority);
        queue_create_infos.push_back(queue_create_info);
    } else {
        vk::DeviceQueueCreateInfo queue_create_info;
        queue_create_info
            .setQueueCount(1)
            .setQueueFamilyIndex(indices.graphics_family_)
            .setQueuePriorities(queue_priority);
        queue_create_infos.push_back(queue_create_info);
        queue_create_info.setQueueFamilyIndex(indices.present_family_);
        queue_create_infos.push_back(queue_create_info);
    }
    vk::PhysicalDeviceFeatures device_features{};
    device_features.setSamplerAnisotropy(true);
    vk::DeviceCreateInfo device_create_info{};
    device_create_info
        .setQueueCreateInfoCount(static_cast<uint32_t>(queue_create_infos.size()))
        .setQueueCreateInfos(queue_create_infos)
        .setEnabledExtensionCount(static_cast<uint32_t>(NVulkanPhysical::Singleton().DeviceExtensions().size()))
        .setPEnabledExtensionNames(NVulkanPhysical::Singleton().DeviceExtensions())
        .setPEnabledFeatures(&device_features);
    device_ = NVulkanPhysical::Singleton().CreateDevice(device_create_info);
    graphics_queue_ = device_.getQueue(indices.graphics_family_, 0);
    present_queue_ = device_.getQueue(indices.present_family_, 0);
}

void NVulkanDevice::CreateCommandPool() {
    auto queue_family_indices = NVulkanPhysical::Singleton().QueueFamilies();
    vk::CommandPoolCreateInfo pool_info{};
    pool_info
        .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
        .setQueueFamilyIndex(queue_family_indices.graphics_family_);
    command_pool_ = device_.createCommandPool(pool_info);
}

uint32_t NVulkanDevice::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    auto memory_properties = NVulkanPhysical::Singleton().GetMemoryProperties();
    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
        if ((typeFilter & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    return 0;
}
