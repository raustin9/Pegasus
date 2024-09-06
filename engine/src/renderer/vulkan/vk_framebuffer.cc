#include "vulkan_backend.hh"
#include "vk_framebuffer.hh"

void 
VulkanBackend::create_framebuffer(
    VKRenderpass& renderpass,
    uint32_t width,
    uint32_t height,
    uint32_t attachment_count,
    std::vector<VkImageView> &attachments,
    VKFramebuffer& out_framebuffer
) {
    // Get a copy of the attachments so we don't have to worry about lifetimes
    out_framebuffer.attachments.resize(attachment_count);
    for (uint32_t i = 0; i < attachment_count; i++) {
        out_framebuffer.attachments[i] = attachments[i];
    }

    out_framebuffer.renderpass = &renderpass;
    out_framebuffer.attachment_count = attachment_count;

    VkFramebufferCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    create_info.renderPass = renderpass.handle;
    create_info.attachmentCount = attachment_count;
    create_info.pAttachments = out_framebuffer.attachments.data();
    create_info.width = width;
    create_info.height = height;
    create_info.layers = 1;

    VK_CHECK(vkCreateFramebuffer(
        m_context.device.logical_device,
        &create_info,
        m_context.allocator,
        &out_framebuffer.handle
    ));
}

void
VulkanBackend::destroy_framebuffer(VKFramebuffer& framebuffer) {
    vkDestroyFramebuffer(m_context.device.logical_device, framebuffer.handle, m_context.allocator);
    if (!framebuffer.attachments.empty()) {
        framebuffer.attachments.clear();
    }

    framebuffer.handle = nullptr;
    framebuffer.attachment_count = 0;
    framebuffer.renderpass = nullptr;
}


void 
VulkanBackend::regenerate_framebuffers(VKSwapchain& swapchain, VKRenderpass& renderpass) {
    // Need separate framebuffer for each image in the swapchain
    for (uint32_t i = 0; i < swapchain.image_count; i++) {
        uint32_t attachment_count = 2;
        std::vector<VkImageView> attachments = {
            swapchain.views[i],
            swapchain.depth_attachment.view
        };

        // Create the framebuffer for the swapchain image
        create_framebuffer(
            renderpass,
            m_context.framebuffer_width,
            m_context.framebuffer_height,
            attachment_count,
            attachments,
            m_context.swapchain.framebuffers[i]
        );
    }
}