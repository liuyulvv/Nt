/**
 * @file NVulkanRender.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-06-01
 */

#include "NVulkanRender.h"

#include "NVulkanDevice.h"
#include "NVulkanPipeline.h"
#include "NVulkanSwapchain.h"

NVulkanRender::NVulkanRender(HWND hwnd, uint32_t width, uint32_t height) {
    CreateSwapchain(hwnd, width, height);
    CreateCommandBuffers();
    CreatePipelineLayout();
    pipeline_ = CreatePipeline("shaders/shader.vert.spv", "shaders/shader.frag.spv", vk::PrimitiveTopology::eTriangleList, swapchain_->RenderPass());
}

void NVulkanRender::CreateSwapchain(HWND hwnd, uint32_t width, uint32_t height) {
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

void NVulkanRender::CreatePipelineLayout() {
    vk::PipelineLayoutCreateInfo pipeline_info{};
    pipeline_info
        .setSetLayoutCount(0)
        .setPSetLayouts(nullptr)
        .setPushConstantRangeCount(0)
        .setPushConstantRanges(nullptr);
    pipeline_layout_ = NVulkanDevice::Singleton().CreatePipelineLayout(pipeline_info);
}

std::unique_ptr<NVulkanPipeline> NVulkanRender::CreatePipeline(const std::string& vert_shader_path, const std::string& frag_shader_path, vk::PrimitiveTopology primitive_topology, const vk::RenderPass& render_pass) {
    auto pipeline_config = NVulkanPipeline::DefaultPipelineConfigInfo(primitive_topology);
    pipeline_config.render_pass_ = render_pass;
    pipeline_config.pipeline_layout_ = pipeline_layout_;
    return std::make_unique<NVulkanPipeline>(vert_shader_path, frag_shader_path, pipeline_config);
}

vk::CommandBuffer NVulkanRender::GetCurrentCommandBuffer() const {
    return command_buffers_[current_image_index_];
}

vk::CommandBuffer NVulkanRender::BeginFrame() {
    // try {
    current_image_index_ = swapchain_->AcquireNextImage();
    is_frame_started_ = true;
    auto command_buffer = GetCurrentCommandBuffer();
    vk::CommandBufferBeginInfo begin_info{};
    command_buffer.begin(begin_info);
    return command_buffer;
    // } catch ([[maybe_unused]] const vk::OutOfDateKHRError& e) {
    //     // CreateSwapchain();
    //     return nullptr;
    // }
}

void NVulkanRender::EndFrame() {
    // try {
    auto command_buffer = GetCurrentCommandBuffer();
    command_buffer.end();
    swapchain_->SubmitCommandBuffers(command_buffer, current_image_index_);
    is_frame_started_ = false;
    // } catch ([[maybe_unused]] const vk::OutOfDateKHRError& e) {
    //     // RecreateSwapchain();
    // }
}

void NVulkanRender::BeginSwapchainRenderPass(vk::CommandBuffer command_buffer) {
    vk::RenderPassBeginInfo render_pass_info{};
    render_pass_info
        .setRenderPass(swapchain_->RenderPass())
        .setFramebuffer(swapchain_->FrameBuffer(current_image_index_));
    render_pass_info.renderArea
        .setOffset({0, 0})
        .setExtent(swapchain_->SwapchainExtent());
    std::array<vk::ClearValue, 2> clear_values{};
    clear_values[0].setColor({0.17F, 0.17F, 0.17F, 1.0F});
    clear_values[1].setDepthStencil({1.0F, 0});
    render_pass_info
        .setClearValueCount(static_cast<uint32_t>(clear_values.size()))
        .setClearValues(clear_values);
    command_buffer.beginRenderPass(render_pass_info, vk::SubpassContents::eInline);
    vk::Viewport viewport{};
    viewport
        .setX(0.0F)
        .setY(0.0F)
        .setWidth(static_cast<float>(swapchain_->SwapchainExtent().width))
        .setHeight(static_cast<float>(swapchain_->SwapchainExtent().height))
        .setMinDepth(0.0F)
        .setMaxDepth(1.0F);
    vk::Rect2D scissor{{0, 0}, swapchain_->SwapchainExtent()};
    command_buffer.setViewport(0, viewport);
    command_buffer.setScissor(0, scissor);
}
