#pragma once

/**
 * @file NVulkanPipeline.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-06-02
 */

#include <string>
#include <vector>

#include "NVulkanHeader.h"

class NVulkanPipeline {
public:
    struct PipelineConfigInfo {
        PipelineConfigInfo() = default;

        vk::PipelineViewportStateCreateInfo viewport_info_{};
        vk::PipelineInputAssemblyStateCreateInfo input_assembly_info_{};
        vk::PipelineRasterizationStateCreateInfo rasterization_info_{};
        vk::PipelineMultisampleStateCreateInfo multisample_info_{};
        vk::PipelineColorBlendAttachmentState color_blend_attachment_{};
        vk::PipelineColorBlendStateCreateInfo color_blend_info_{};
        vk::PipelineDepthStencilStateCreateInfo depth_stencil_info_{};
        std::vector<vk::DynamicState> dynamic_states_{};
        vk::PipelineDynamicStateCreateInfo dynamic_state_info_{};
        vk::PipelineLayout pipeline_layout_{nullptr};
        vk::RenderPass render_pass_{nullptr};
        uint32_t subpass_{0};
    };

public:
    NVulkanPipeline(const std::string& vert_shader_path, const std::string& frag_shader_path, const PipelineConfigInfo& config);
    ~NVulkanPipeline();
    NVulkanPipeline() = delete;
    NVulkanPipeline(const NVulkanPipeline& pipeline) = delete;
    NVulkanPipeline(NVulkanPipeline&& pipeline) = delete;
    NVulkanPipeline& operator=(const NVulkanPipeline& pipeline) = delete;
    NVulkanPipeline& operator=(NVulkanPipeline&& pipeline) = delete;

public:
    static PipelineConfigInfo DefaultPipelineConfigInfo(vk::PrimitiveTopology primitive_topology = vk::PrimitiveTopology::eTriangleList);
    void Bind(const vk::CommandBuffer& buffer);

private:
    void CreateGraphicsPipeline(const std::string& vert_shader_path, const std::string& frag_shader_path, const PipelineConfigInfo& config);
    static std::vector<char> ReadFile(const std::string& path);
    vk::ShaderModule CreateShaderModule(const std::vector<char>& code);

private:
    vk::Pipeline graphics_pipeline_{};
    vk::ShaderModule vert_shader_module_{};
    vk::ShaderModule frag_shader_module_{};
};