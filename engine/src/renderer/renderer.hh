/*

#pragma once

#include "stdafx.hh"
#include "vkcommon.hh"
#include "platform/platform.hh"

// #include <vulkan/vulkan.h>
#include <cstdint>

class Renderer {
    public:
        Renderer(std::string name, uint32_t width, uint32_t height);

        void OnInit();
        void OnUpdate();
        void OnRender();
        void OnDestroy();

        void WindowResize(uint32_t width, uint32_t height);

        void OnKeyDown(uint8_t) {}
        void OnKeyUp(uint8_t) {}

        // Accessors
        uint32_t GetWidth() const { return m_width; }
        uint32_t GetHeight() const { return m_height; }
        uint64_t GetFrameCounter() const { return m_framecounter; }
        const char* GetTitle() const { return m_title.c_str(); }
        const std::string GetStringTitle() const { return m_title; }

        // Mutators
        void SetWidth(uint32_t width) { m_width = width; }
        void SetHeight(uint32_t height) { m_height = height; }
        

    private:
        void InitVulkan();
        void SetupPipeline();

        void CreateInstance();
        void CreateSurface();
        void CreateSynchronizationObjects();
        void CreateDevice(VkQueueFlags requestedQueueTypes);
        void CreateSwapchain(
            uint32_t* width,
            uint32_t* height,
            bool vsync);
        void CreateRenderPass();
        void AllocateCommandBuffers();
        void CreateFramebuffers();

        void populate_command_buffer(uint32_t current_buffer_index, uint32_t current_image_index);
        void submit_command_buffer(uint32_t current_buffer_index);
        void present_image(uint32_t image_index);

        uint32_t m_width;
        uint32_t m_height;
        float m_aspect_ratio;

        uint64_t m_framecounter;
        std::string m_title;
        // std::string m_assetpath;
        bool m_initialized;

        uint32_t m_command_buffer_index = 0;
        uint32_t m_command_buffer_count = 0;

        Platform m_platform;

        VKCommonParameters m_vkparams;

        VkPhysicalDeviceProperties m_deviceProperties;
        VkPhysicalDeviceMemoryProperties m_DeviceMemoryProperties;
        VkPhysicalDeviceFeatures m_deviceFeatures;
};

*/
