#include "vulkan_backend.hh"
#include "core/qlogger.hh"

bool 
VulkanBackend::create_renderpass(
    VKRenderpass& out_renderpass,
    float x, float y, float w, float h,
    float r, float g, float b, float a,
    float depth,
    uint32_t stencil
) {
    out_renderpass.x = x;
    out_renderpass.y = y;
    out_renderpass.w = w;
    out_renderpass.h = h;

    out_renderpass.r = r;
    out_renderpass.g = g;
    out_renderpass.b = b;
    out_renderpass.a = a;

    out_renderpass.depth = depth;
    out_renderpass.stencil = stencil;

    // Main subpass
    VkSubpassDescription subpass {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    // Attachments TODO: Make configurable
    uint32_t attachment_description_count = 2;
    // VkAttachmentDescription attachment_descriptions[attachment_description_count];
    VkAttachmentDescription attachment_descriptions[32];

    // Color attachment
    VkAttachmentDescription color_attachment {};
    color_attachment.format  = m_context.swapchain.image_format.format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    color_attachment.flags = 0;

    attachment_descriptions[0] = color_attachment;

    // Color attachment reference
    VkAttachmentReference color_attachment_reference;
    color_attachment_reference.attachment = 0;
    color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_reference;

    // Depth attachment
    VkAttachmentDescription depth_attachment {};
    depth_attachment.format = m_context.device.depth_format;
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    attachment_descriptions[1] = depth_attachment;

    // Depth attachment reference
    VkAttachmentReference depth_attachment_reference;
    depth_attachment_reference.attachment = 1;
    depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // TODO: Other attachment types: (input, resolve, preserve)

    // Depth stencil data
    subpass.pDepthStencilAttachment = &depth_attachment_reference;

    // Input from a shader
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = nullptr;

    // Attachments used for multisapling color attachments
    subpass.pResolveAttachments = nullptr;

    // Attachments not used in this subpass, but must be preserved for the next
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = nullptr;

    // Renderpass dependencies. TODO: Make configurable
    VkSubpassDependency dependency {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dependencyFlags = 0;


    // Renderpass create info
    VkRenderPassCreateInfo rp_info {};
    rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rp_info.attachmentCount = attachment_description_count;
    rp_info.pAttachments = attachment_descriptions;
    rp_info.subpassCount = 1;
    rp_info.pSubpasses = &subpass;
    rp_info.dependencyCount = 1;
    rp_info.pDependencies = &dependency;
    rp_info.pNext = nullptr;
    rp_info.flags = 0;

    VK_CHECK(vkCreateRenderPass(
        m_context.device.logical_device,
        &rp_info,
        m_context.allocator,
        &out_renderpass.handle
    ));

    qlogger::Info("Renderpass created...");
    return true;
}

void
VulkanBackend::destroy_renderpass(VKRenderpass& renderpass) {
    qlogger::Info("Destroying renderpass... ");
    if (renderpass.handle) {
        vkDestroyRenderPass(m_context.device.logical_device, renderpass.handle, m_context.allocator);
        renderpass.handle = nullptr;
    }
    qlogger::Info("Destroyed.");
}

void
VKRenderpass::begin(VKCommandBuffer& command_buffer, VKFramebuffer& framebuffer) {
    VkRenderPassBeginInfo begin_info {};
    begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    begin_info.renderPass = this->handle;
    begin_info.framebuffer = framebuffer.handle;
    begin_info.renderArea.offset.x = this->x;
    begin_info.renderArea.offset.y = this->y;
    begin_info.renderArea.extent.width = this->w;
    begin_info.renderArea.extent.height = this->h;
    
    VkClearValue clear_values[2];
    clear_values[0].color.float32[0] = this->r;
    clear_values[0].color.float32[1] = this->g;
    clear_values[0].color.float32[2] = this->b;
    clear_values[0].color.float32[3] = this->a;
    clear_values[1].depthStencil.depth = this->depth;
    clear_values[1].depthStencil.stencil = this->stencil;

    begin_info.clearValueCount = 2;
    begin_info.pClearValues = clear_values;

    vkCmdBeginRenderPass(
        command_buffer.handle,
        &begin_info,
        VK_SUBPASS_CONTENTS_INLINE
    );
    command_buffer.state = COMMAND_BUFFER_STATE_IN_RENDER_PASS;
}

void
VKRenderpass::end(VKCommandBuffer& command_buffer) {
    vkCmdEndRenderPass(command_buffer.handle);
    command_buffer.state = COMMAND_BUFFER_STATE_RECORDING;
}