#pragma once
#include "defines.hh"
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
        uint32_t m_framebuffer_width;
        uint32_t m_framebuffer_height;
        uint32_t m_framebuffer_size_generation;
        uint64_t m_framebuffer_size_last_generation;

        #if defined(P_DEBUG)
        VkDebufUtilsMessengerEXT m_debug_messenger;
        #endif



        // Vulkan Context
        VkAllocationCallbacks *m_vkallocator;
};