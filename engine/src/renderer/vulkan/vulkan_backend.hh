#pragma once
#include "defines.hh"
#include "vulkan_types.hh"
#include "vulkan_utils.hh"
#include "platform/platform.hh"
#include "renderer/render_types.hh"

#include <vulkan/vulkan.h>

class VulkanBackend : public RendererBackend {
    public:
        // VulkanBackend() {}
        // ~VulkanBackend() {}
        bool Initialize(std::string& name) override;
        void Shutdown() override;

        void Resized(uint32_t width, uint32_t height) override;

        bool BeginFrame(float delta_time) override;
        bool EndFrame(float delta_time) override;

    private:
        // Backend Members
        // uint32_t m_framebuffer_width;
        // uint32_t m_framebuffer_height;
        // uint32_t m_framebuffer_size_generation;
        // uint64_t m_framebuffer_size_last_generation;

        VKContext m_context;

        // Member Functions
        bool create_instance(const char* name);
        void create_debug_messenger();
        bool create_surface();
        bool create_device();
        bool create_swapchain(uint32_t width, uint32_t height, VKSwapchain& out_swapchain);
        bool create_renderpass(
            VKRenderpass& out_renderpass,
            float x, float y, float w, float h,
            float r, float g, float b, float a,
            float depth,
            uint32_t stencil
        );
        void create_framebuffer(
            VKRenderpass& renderpass,
            uint32_t width,
            uint32_t height,
            uint32_t attachment_count,
            std::vector<VkImageView> &attachments,
            VKFramebuffer& out_framebuffer
        );
        void create_command_buffers();
        bool create_buffers(); // create vertex and index buffers

        void destroy_device();
        void destroy_swapchain();
        void destroy_renderpass(VKRenderpass& renderpass);
        void destroy_framebuffer(VKFramebuffer& framebuffer);
        void destroy_buffers();

        bool recreate_swapchain();
        void regenerate_framebuffers(VKSwapchain& swapchain, VKRenderpass& renderpass);
        // void free_command_buffer(VkCommandPool pool, VKCommandBuffer& command_buffer);
        // void allocate_command_buffer(VkCommandPool pool, bool is_primary, VKCommandBuffer& command_buffer);
};