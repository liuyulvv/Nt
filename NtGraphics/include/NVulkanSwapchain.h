#pragma once

/**
 * @file NVulkanSwapchain.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-06-01
 */

#include <vector>

#include "NVulkanHeader.h"

class BDllExport NVulkanSwapchain {
public:
    NVulkanSwapchain(HWND hwnd, uint32_t width, uint32_t height);
    NVulkanSwapchain() = delete;
    ~NVulkanSwapchain() = default;
    NVulkanSwapchain(const NVulkanSwapchain& swapchain) = delete;
    NVulkanSwapchain(NVulkanSwapchain&& swapchain) = delete;
    NVulkanSwapchain& operator=(const NVulkanSwapchain& swapchain) = delete;
    NVulkanSwapchain& operator=(NVulkanSwapchain&& swapchain) = delete;

public:
    size_t GetImageCount() const;

public:
    static constexpr int MAX_FRAMES_IN_FLIGHT{2};

private:
    void CreateSwapchain();
    void CreateRenderPass();
    void CreateDepthResources();
    void CreateFramebuffers();
    void CreateSyncObjects();

private:
    vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& available_formats);
    vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& available_present_modes);
    vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
    vk::Format FindDepthFormat() const;

private:
    vk::SurfaceKHR surface_{};
    vk::Extent2D window_extent_{};
    vk::Format swapchain_image_format_{};
    vk::Extent2D swapchain_extent_{};
    vk::SwapchainKHR swapchain_{};
    std::vector<vk::Image> swapchain_images_{};
    std::vector<vk::ImageView> swapchain_image_views_{};
    vk::RenderPass render_pass_{};
    std::vector<vk::Image> depth_images_{};
    std::vector<vk::DeviceMemory> depth_image_memories_{};
    std::vector<vk::ImageView> depth_image_views_{};
    std::vector<vk::Framebuffer> swapchain_framebuffers_{};
    std::vector<vk::Semaphore> image_available_semaphores_{};
    std::vector<vk::Semaphore> render_finished_semaphores_{};
    std::vector<vk::Fence> in_flight_fences_{};
    std::vector<vk::Fence> images_in_flight_{};
    size_t current_frame_{0};
};