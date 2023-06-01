#pragma once

/**
 * @file NVulkanRender.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-06-01
 */

#include <memory>
#include <vector>

#include "NVulkanHeader.h"

class NVulkanSwapchain;

class NVulkanRender {
public:
    NVulkanRender(HWND hwnd, uint32_t width, uint32_t height);
    NVulkanRender() = delete;
    ~NVulkanRender() = default;
    NVulkanRender(const NVulkanRender& render) = delete;
    NVulkanRender(NVulkanRender&& render) = delete;
    NVulkanRender& operator=(const NVulkanRender& render) = delete;
    NVulkanRender& operator=(NVulkanRender&& render) = delete;

private:
    void CreateSwapchain(HWND hwnd, uint32_t width, uint32_t height);
    void CreateCommandBuffers();

private:
    std::unique_ptr<NVulkanSwapchain> swapchain_{};
    std::vector<vk::CommandBuffer> command_buffers_{};
    uint32_t current_image_index_{};
    bool is_frame_started_{false};
};