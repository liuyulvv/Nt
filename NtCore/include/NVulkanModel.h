#pragma once

/**
 * @file NVulkanModel.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-06-02
 */

#include <vector>

#include "NVulkanHeader.h"

class NVulkanModel {
public:
    struct Vertex {
        glm::vec3 position_;
        glm::vec4 color_;
        bool operator==(const Vertex& other) const;
        static std::vector<vk::VertexInputBindingDescription> GetBindingDescriptions();
        static std::vector<vk::VertexInputAttributeDescription> GetAttributeDescriptions();
    };

public:
    explicit NVulkanModel(const std::vector<Vertex>& vertices);
    ~NVulkanModel();
    NVulkanModel() = delete;
    NVulkanModel(const NVulkanModel& model) = default;
    NVulkanModel(NVulkanModel&& model) = default;
    NVulkanModel& operator=(const NVulkanModel& model) = default;
    NVulkanModel& operator=(NVulkanModel&& model) = default;

public:
    void Bind(vk::CommandBuffer& command_buffer) const;
    void Draw(vk::CommandBuffer& command_buffer) const;

private:
    void CreateVertexBuffer(const std::vector<Vertex>& vertices);

private:
    vk::Buffer vertex_buffer_{};
    vk::DeviceMemory vertex_buffer_memory_{};
    uint32_t vertex_count_{0};
};