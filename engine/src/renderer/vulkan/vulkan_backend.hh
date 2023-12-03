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

        void destroy_device();
        void destroy_swapchain();

        bool recreate_swapchain(uint32_t width, uint32_t height, VKSwapchain& out_swapchain);
};