/**
 * @file NVulkanRender.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-06-01
 */

#include "NVulkanRender.h"

#include "NVulkanDevice.h"
#include "NVulkanSwapchain.h"

NVulkanRender::NVulkanRender(HWND hwnd, uint32_t width, uint32_t height) {
    RecreateSwapchain(hwnd, width, height);
    CreateCommandBuffers();
}

void NVulkanRender::RecreateSwapchain(HWND hwnd, uint32_t width, uint32_t height) {
    NVulkanDevice::Singleton().WaitIdle();
    swapchain_.reset(nullptr);
    swapchain_ = std::make_unique<NVulkanSwapchain>(hwnd, width, height);
}

void NVulkanRender::CreateCommandBuffers() {
    command_buffers_.resize(swapchain_->GetImageCount());
    vk::CommandBufferAllocateInfo alloc_info{};
    alloc_info
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandPool(NVulkanDevice::Singleton().CommandPool())
        .setCommandBufferCount(static_cast<uint32_t>(command_buffers_.size()));
    command_buffers_ = NVulkanDevice::Singleton().AllocateCommandBuffers(alloc_info);
}
