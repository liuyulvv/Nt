/**
 * @file NVulkanModel.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-06-02
 */

#include "NVulkanModel.h"

#include <array>

#include "NVulkanDevice.h"

bool NVulkanModel::Vertex::operator==(const Vertex& other) const {
    return position_ == other.position_ && color_ == other.color_;
}

std::vector<vk::VertexInputBindingDescription> NVulkanModel::Vertex::GetBindingDescriptions() {
    std::vector<vk::VertexInputBindingDescription> binding_descriptions(1);
    binding_descriptions.at(0)
        .setBinding(0)
        .setStride(sizeof(Vertex))
        .setInputRate(vk::VertexInputRate::eVertex);
    return binding_descriptions;
}

std::vector<vk::VertexInputAttributeDescription> NVulkanModel::Vertex::GetAttributeDescriptions() {
    std::vector<vk::VertexInputAttributeDescription> attribute_descriptions{};
    attribute_descriptions.push_back({0, 0, vk::Format::eR32G32B32Sfloat, static_cast<uint32_t>(offsetof(NVulkanModel::Vertex, position_))});
    attribute_descriptions.push_back({1, 0, vk::Format::eR32G32B32A32Sfloat, static_cast<uint32_t>(offsetof(NVulkanModel::Vertex, color_))});
    return attribute_descriptions;
}

NVulkanModel::NVulkanModel(const std::vector<Vertex>& vertices) {
    CreateVertexBuffer(vertices);
}

NVulkanModel::~NVulkanModel() {
    NVulkanDevice::Singleton().WaitIdle();
    NVulkanDevice::Singleton().DestroyBuffer(vertex_buffer_);
    NVulkanDevice::Singleton().FreeMemory(vertex_buffer_memory_);
}

void NVulkanModel::Bind(vk::CommandBuffer& command_buffer) const {
    std::array<vk::Buffer, 1> buffers{vertex_buffer_};
    command_buffer.bindVertexBuffers(0, buffers, {0});
}

void NVulkanModel::Draw(vk::CommandBuffer& command_buffer) const {
    command_buffer.draw(vertex_count_, 1, 0, 0);
}

void NVulkanModel::CreateVertexBuffer(const std::vector<Vertex>& vertices) {
    vertex_count_ = static_cast<uint32_t>(vertices.size());
    vk::DeviceSize buffer_size = sizeof(vertices[0]) * vertex_count_;
    vk::Buffer staging_buffer{};
    vk::DeviceMemory staging_buffer_memory{};
    NVulkanDevice::Singleton().CreateBuffer(
        buffer_size,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        staging_buffer,
        staging_buffer_memory);
    auto* data = NVulkanDevice::Singleton().MapMemory(staging_buffer_memory, 0, buffer_size);
    memcpy(data, vertices.data(), buffer_size);
    NVulkanDevice::Singleton().UnMapMemory(staging_buffer_memory);
    NVulkanDevice::Singleton().CreateBuffer(
        buffer_size,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        vertex_buffer_,
        vertex_buffer_memory_);
    NVulkanDevice::Singleton().CopyBuffer(staging_buffer, vertex_buffer_, buffer_size);
    NVulkanDevice::Singleton().DestroyBuffer(staging_buffer);
    NVulkanDevice::Singleton().FreeMemory(staging_buffer_memory);
}
