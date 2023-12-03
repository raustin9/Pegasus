#include "vulkan_backend.hh"


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
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    // Attachments TODO: Make configurable
    uint32_t attachment_description_count = 2;
    VkAttachmentDescription attachment_descriptions[attachment_description_count];

    // Color attachment
    VkAttachmentDescription color_attachment = {};
    color_attachment.format  = m_context.swapchain.image_format.format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
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
    VkAttachmentDescription depth_attachment = {};
    depth_attachment.format = m_context.device.depth_format;
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
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
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dependencyFlags = 0;


    // Renderpass create info
    VkRenderPassCreateInfo rp_info = {};
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

    std::cout << "Renderpass created..." << std::endl;
    return true;
}

void
VulkanBackend::destroy_renderpass(VKRenderpass& renderpass) {
    if (renderpass.handle) {
        vkDestroyRenderPass(m_context.device.logical_device, renderpass.handle, m_context.allocator);
        renderpass.handle = nullptr;
    }
}