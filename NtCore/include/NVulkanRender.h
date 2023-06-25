#pragma once

/**
 * @file NVulkanRender.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-06-01
 */

#include <memory>
#include <vector>

#include "NVulkanHeader.h"
#include "NPlatform.h"

class NVulkanSwapchain;
class NVulkanPipeline;
class NCanvas;

class NVulkanRender {
public:
    NVulkanRender(NCanvasID id, uint32_t width, uint32_t height);
    NVulkanRender() = delete;
    ~NVulkanRender() = default;
    NVulkanRender(const NVulkanRender& render) = delete;
    NVulkanRender(NVulkanRender&& render) = delete;
    NVulkanRender& operator=(const NVulkanRender& render) = delete;
    NVulkanRender& operator=(NVulkanRender&& render) = delete;

public:
    vk::CommandBuffer BeginFrame();
    void EndFrame();
    void BeginSwapchainRenderPass(vk::CommandBuffer command_buffer);
    void EndSwapchainRenderPass(vk::CommandBuffer command_buffer);
    void RecreateSwapchain(NCanvasID id, uint32_t width, uint32_t height);

private:
    void CreateSwapchain(NCanvasID id, uint32_t width, uint32_t height);
    void CreateCommandBuffers();
    void CreatePipelineLayout();

private:
    std::unique_ptr<NVulkanPipeline> CreatePipeline(const std::string& vert_shader_path, const std::string& frag_shader_path, vk::PrimitiveTopology primitive_topology, const vk::RenderPass& render_pass);
    vk::CommandBuffer GetCurrentCommandBuffer() const;

private:
    std::unique_ptr<NVulkanSwapchain> swapchain_{};
    std::vector<vk::CommandBuffer> command_buffers_{};
    vk::PipelineLayout pipeline_layout_{};
    std::unique_ptr<NVulkanPipeline> pipeline_{};
    uint32_t current_image_index_{};
    bool is_frame_started_{false};
};