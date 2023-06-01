/**
 * @file NVulkanSwapchain.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-06-01
 */

#include "NVulkanSwapchain.h"

#include "NVulkanDevice.h"
#include "NVulkanInstance.h"
#include "NVulkanPhysical.h"

NVulkanSwapchain::NVulkanSwapchain(HWND hwnd, uint32_t width, uint32_t height) {
    window_extent_.setWidth(width);
    window_extent_.setHeight(height);
    vk::Win32SurfaceCreateInfoKHR info{{}, GetModuleHandle(nullptr), hwnd};
    surface_ = NVulkanInstance::Singleton().CreateSurface(info);
    CreateSwapchain();
    CreateRenderPass();
    CreateDepthResources();
    CreateFramebuffers();
    CreateSyncObjects();
}

size_t NVulkanSwapchain::GetImageCount() const {
    return swapchain_images_.size();
}

void NVulkanSwapchain::CreateSwapchain() {
    auto swapchain_support = NVulkanPhysical::Singleton().QuerySwapchainSupport(surface_);
    auto surface_format = ChooseSwapSurfaceFormat(swapchain_support.formats_);
    swapchain_image_format_ = surface_format.format;
    auto present_mode = ChooseSwapPresentMode(swapchain_support.present_modes_);
    auto extent = ChooseSwapExtent(swapchain_support.capabilities_);
    swapchain_extent_ = extent;
    auto image_count = swapchain_support.capabilities_.minImageCount + 1;
    if (swapchain_support.capabilities_.maxImageCount > 0 && image_count > swapchain_support.capabilities_.maxImageCount) {
        image_count = swapchain_support.capabilities_.maxImageCount;
    }
    vk::SwapchainCreateInfoKHR create_info{};
    create_info
        .setSurface(surface_)
        .setMinImageCount(image_count)
        .setImageFormat(surface_format.format)
        .setImageColorSpace(surface_format.colorSpace)
        .setImageExtent(extent)
        .setImageArrayLayers(1)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
        .setPreTransform(swapchain_support.capabilities_.currentTransform)
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
        .setPresentMode(present_mode)
        .setClipped(true);
    auto indices = NVulkanPhysical::Singleton().QueueFamilies();
    if (indices.graphics_family_ != indices.present_family_) {
        std::array<uint32_t, 2> queue_family_indices{indices.graphics_family_, indices.present_family_};
        create_info
            .setImageSharingMode(vk::SharingMode::eConcurrent)
            .setQueueFamilyIndices(queue_family_indices);
    } else {
        create_info
            .setImageSharingMode(vk::SharingMode::eExclusive)
            .setQueueFamilyIndices(indices.graphics_family_);
    }
    swapchain_ = NVulkanDevice::Singleton().CreateSwapchain(create_info);
    swapchain_images_ = NVulkanDevice::Singleton().GetSwapchainImages(swapchain_);
    swapchain_image_views_.resize(swapchain_images_.size());
    for (size_t i = 0; i < swapchain_image_views_.size(); ++i) {
        swapchain_image_views_[i] = NVulkanDevice::Singleton().CreateImageView(swapchain_images_[i], swapchain_image_format_, vk::ImageAspectFlagBits::eColor);
    }
}

void NVulkanSwapchain::CreateRenderPass() {
    vk::AttachmentDescription depth_attachment{};
    depth_attachment
        .setFormat(FindDepthFormat())
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
    vk::AttachmentReference depth_attachment_reference;
    depth_attachment_reference
        .setAttachment(1)
        .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::AttachmentDescription color_attachment;
    color_attachment
        .setFormat(swapchain_image_format_)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
    vk::AttachmentReference color_attachment_reference;
    color_attachment_reference
        .setAttachment(0)
        .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

    vk::SubpassDescription subpass;
    subpass
        .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
        .setColorAttachmentCount(1)
        .setColorAttachments(color_attachment_reference)
        .setPDepthStencilAttachment(&depth_attachment_reference);

    vk::SubpassDependency dependency;
    dependency
        .setSrcSubpass(VK_SUBPASS_EXTERNAL)
        .setSrcAccessMask(vk::AccessFlagBits::eNone)
        .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
        .setDstSubpass(0)
        .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite);

    std::array<vk::AttachmentDescription, 2> attachments{color_attachment, depth_attachment};

    vk::RenderPassCreateInfo render_pass_info;
    render_pass_info
        .setAttachmentCount(static_cast<uint32_t>(attachments.size()))
        .setAttachments(attachments)
        .setSubpassCount(1)
        .setSubpasses(subpass)
        .setDependencyCount(1)
        .setDependencies(dependency);
    render_pass_ = NVulkanDevice::Singleton().CreateRenderPass(render_pass_info);
}

void NVulkanSwapchain::CreateDepthResources() {
    auto depth_format = FindDepthFormat();
    depth_images_.resize(GetImageCount());
    depth_image_memories_.resize(GetImageCount());
    depth_image_views_.resize(GetImageCount());
    for (size_t i = 0; i < depth_images_.size(); ++i) {
        NVulkanDevice::Singleton().CreateImage(swapchain_extent_.width, swapchain_extent_.height, depth_format, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal, depth_images_[i], depth_image_memories_[i]);
        depth_image_views_[i] = NVulkanDevice::Singleton().CreateImageView(depth_images_[i], depth_format, vk::ImageAspectFlagBits::eDepth);
    }
}

void NVulkanSwapchain::CreateFramebuffers() {
    swapchain_framebuffers_.resize(GetImageCount());
    for (size_t i = 0; i < GetImageCount(); ++i) {
        std::array<vk::ImageView, 2> attachments{swapchain_image_views_[i], depth_image_views_[i]};
        vk::FramebufferCreateInfo framebuffer_info{};
        framebuffer_info
            .setRenderPass(render_pass_)
            .setAttachmentCount(static_cast<uint32_t>(attachments.size()))
            .setAttachments(attachments)
            .setWidth(swapchain_extent_.width)
            .setHeight(swapchain_extent_.height)
            .setLayers(1);
        swapchain_framebuffers_[i] = NVulkanDevice::Singleton().CreateFramebuffer(framebuffer_info);
    }
}

void NVulkanSwapchain::CreateSyncObjects() {
    image_available_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    render_finished_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    in_flight_fences_.resize(MAX_FRAMES_IN_FLIGHT);
    images_in_flight_.resize(GetImageCount());
    vk::SemaphoreCreateInfo semaphore_info{};
    vk::FenceCreateInfo fence_info{};
    fence_info.setFlags(vk::FenceCreateFlagBits::eSignaled);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        image_available_semaphores_[i] = NVulkanDevice::Singleton().CreateSemaphore(semaphore_info);
        render_finished_semaphores_[i] = NVulkanDevice::Singleton().CreateSemaphore(semaphore_info);
        in_flight_fences_[i] = NVulkanDevice::Singleton().CreateFence(fence_info);
    }
}

vk::SurfaceFormatKHR NVulkanSwapchain::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& available_formats) {
    for (const auto& available_format : available_formats) {
        if (available_format.format == vk::Format::eR8G8B8A8Srgb && available_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return available_format;
        }
    }
    return available_formats[0];
}

vk::PresentModeKHR NVulkanSwapchain::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& available_present_modes) {
    for (const auto& available_present_mode : available_present_modes) {
        if (available_present_mode == vk::PresentModeKHR::eMailbox) {
            return available_present_mode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D NVulkanSwapchain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)()) {
        return capabilities.currentExtent;
    }
    auto actual_extent = window_extent_;
    actual_extent.width = (std::max)(capabilities.minImageExtent.width, (std::min)(capabilities.maxImageExtent.width, actual_extent.width));
    actual_extent.height = (std::max)(capabilities.minImageExtent.height, (std::min)(capabilities.maxImageExtent.height, actual_extent.height));
    return actual_extent;
}

vk::Format NVulkanSwapchain::FindDepthFormat() const {
    return NVulkanDevice::Singleton().FindSupportFormat({vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint}, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}