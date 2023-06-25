#pragma once

/**
 * @file NVulkanDevice.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-06-01
 */

#include <cstdint>
#include <vector>

#include "NVulkanHeader.h"

class NDllExport NVulkanDevice {
public:
    static NVulkanDevice& Singleton() {
        static NVulkanDevice device;
        return device;
    }

private:
    NVulkanDevice();

public:
    ~NVulkanDevice() = default;
    NVulkanDevice(const NVulkanDevice& device) = default;
    NVulkanDevice(NVulkanDevice&& device) = delete;
    NVulkanDevice& operator=(const NVulkanDevice& device) = default;
    NVulkanDevice& operator=(NVulkanDevice&& device) = delete;

public:
    vk::SwapchainKHR CreateSwapchain(const vk::SwapchainCreateInfoKHR& info);
    std::vector<vk::Image> GetSwapchainImages(const vk::SwapchainKHR& swapchain);
    vk::ImageView CreateImageView(const vk::Image& image, const vk::Format& format, vk::ImageAspectFlagBits flag);
    vk::Format FindSupportFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, const vk::FormatFeatureFlags& features) const;
    vk::RenderPass CreateRenderPass(const vk::RenderPassCreateInfo& info);
    void CreateImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, const vk::ImageUsageFlags& usage, const vk::MemoryPropertyFlags& properties, vk::Image& image, vk::DeviceMemory& memory);
    vk::Framebuffer CreateFramebuffer(const vk::FramebufferCreateInfo& info);
    vk::Semaphore CreateSemaphore(const vk::SemaphoreCreateInfo& info);
    vk::Fence CreateFence(const vk::FenceCreateInfo& info);
    void WaitIdle() const;
    const vk::CommandPool& CommandPool() const;
    std::vector<vk::CommandBuffer> AllocateCommandBuffers(const vk::CommandBufferAllocateInfo& info);
    vk::ShaderModule CreateShaderModule(const vk::ShaderModuleCreateInfo& info);
    vk::Pipeline CreateGraphicsPipeline(const vk::GraphicsPipelineCreateInfo& info);
    void DestroyShaderModule(const vk::ShaderModule& shader_module);
    void DestroyPipeline(const vk::Pipeline& pipeline);
    void DestroyBuffer(const vk::Buffer& buffer);
    void FreeMemory(const vk::DeviceMemory& memory);
    void CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& memory);
    void CopyBuffer(const vk::Buffer& src, vk::Buffer& dst, vk::DeviceSize size);
    void* MapMemory(const vk::DeviceMemory& memory, uint64_t offset, uint64_t size);
    void UnMapMemory(const vk::DeviceMemory& memory);
    vk::PipelineLayout CreatePipelineLayout(const vk::PipelineLayoutCreateInfo& info);
    void WaitForFences(const vk::Fence& fence, bool wait_all = true);
    void ResetFences(const vk::Fence& fence);
    uint32_t AcquireNextImage(const vk::SwapchainKHR& swapchain, const vk::Semaphore& semaphore, const vk::Fence& fence = nullptr);
    const vk::Queue& GraphicsQueue() const;
    const vk::Queue& PresentQueue() const;

private:
    void CreateDevice();
    void CreateCommandPool();

private:
    uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
    vk::CommandBuffer BeginSingleTimeCommands();
    void EndSingleTimeCommands(const vk::CommandBuffer& command_buffer);

private:
    vk::Device device_{};
    vk::Queue graphics_queue_{};
    vk::Queue present_queue_{};
    vk::CommandPool command_pool_{};
};