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

vk::ShaderModule NVulkanDevice::CreateShaderModule(const vk::ShaderModuleCreateInfo& info) {
    return device_.createShaderModule(info);
}

vk::Pipeline NVulkanDevice::CreateGraphicsPipeline(const vk::GraphicsPipelineCreateInfo& info) {
    return device_.createGraphicsPipeline(nullptr, info).value;
}

void NVulkanDevice::DestroyShaderModule(const vk::ShaderModule& shader_module) {
    device_.destroyShaderModule(shader_module);
}

void NVulkanDevice::DestroyPipeline(const vk::Pipeline& pipeline) {
    device_.destroyPipeline(pipeline);
}

void NVulkanDevice::DestroyBuffer(const vk::Buffer& buffer) {
    device_.destroyBuffer(buffer);
}

void NVulkanDevice::FreeMemory(const vk::DeviceMemory& memory) {
    device_.freeMemory(memory);
}

void NVulkanDevice::CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& memory) {
    vk::BufferCreateInfo buffer_info{};
    buffer_info
        .setFlags(vk::BufferCreateFlags())
        .setSize(size)
        .setUsage(usage)
        .setSharingMode(vk::SharingMode::eExclusive);
    buffer = device_.createBuffer(buffer_info);
    auto memory_requirements = device_.getBufferMemoryRequirements(buffer);
    vk::MemoryAllocateInfo allocate_info;
    allocate_info
        .setAllocationSize(memory_requirements.size)
        .setMemoryTypeIndex(FindMemoryType(memory_requirements.memoryTypeBits, properties));
    memory = device_.allocateMemory(allocate_info);
    device_.bindBufferMemory(buffer, memory, 0);
}

void NVulkanDevice::CopyBuffer(const vk::Buffer& src, vk::Buffer& dst, vk::DeviceSize size) {
    auto command_buffer = BeginSingleTimeCommands();
    vk::BufferCopy copy_region;
    copy_region.setSize(size);
    command_buffer.copyBuffer(src, dst, 1, &copy_region);
    EndSingleTimeCommands(command_buffer);
}

void* NVulkanDevice::MapMemory(const vk::DeviceMemory& memory, uint64_t offset, uint64_t size) {
    return device_.mapMemory(memory, offset, size);
}

void NVulkanDevice::UnMapMemory(const vk::DeviceMemory& memory) {
    device_.unmapMemory(memory);
}

vk::PipelineLayout NVulkanDevice::CreatePipelineLayout(const vk::PipelineLayoutCreateInfo& info) {
    return device_.createPipelineLayout(info);
}

void NVulkanDevice::WaitForFences(const vk::Fence& fence, bool wait_all) {
    [[maybe_unused]] auto res = device_.waitForFences(fence, wait_all, (std::numeric_limits<uint64_t>::max)());
}

void NVulkanDevice::ResetFences(const vk::Fence& fence) {
    device_.resetFences(fence);
}

uint32_t NVulkanDevice::AcquireNextImage(const vk::SwapchainKHR& swapchain, const vk::Semaphore& semaphore, const vk::Fence& fence) {
    return device_.acquireNextImageKHR(swapchain, (std::numeric_limits<uint64_t>::max)(), semaphore, fence).value;
}

const vk::Queue& NVulkanDevice::GraphicsQueue() const {
    return graphics_queue_;
}

const vk::Queue& NVulkanDevice::PresentQueue() const {
    return present_queue_;
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

vk::CommandBuffer NVulkanDevice::BeginSingleTimeCommands() {
    vk::CommandBufferAllocateInfo allocate_info;
    allocate_info
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandPool(command_pool_)
        .setCommandBufferCount(1);
    auto command_buffer = device_.allocateCommandBuffers(allocate_info);
    vk::CommandBufferBeginInfo begin_info;
    begin_info.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    command_buffer.at(0).begin(begin_info);
    return command_buffer.at(0);
}

void NVulkanDevice::EndSingleTimeCommands(const vk::CommandBuffer& command_buffer) {
    command_buffer.end();
    vk::SubmitInfo submit_info{};
    submit_info
        .setCommandBufferCount(1)
        .setCommandBuffers(command_buffer);
    graphics_queue_.submit(submit_info);
    graphics_queue_.waitIdle();
    device_.freeCommandBuffers(command_pool_, command_buffer);
}
