#pragma once

#include "stdafx.hh"
#include "vkcommon.hh"
#include "platform/platform.hh"
#include "renderer/vkmodel.hh"

#include <cstdint>
#include <vulkan/vulkan_core.h>


class Renderer {
    public:
        Renderer(std::string name, std::string assetPath,  uint32_t width, uint32_t height, Platform& platform);

        void OnInit();
        void OnUpdate();
        void OnRender();
        void OnDestroy();

        const std::string GetWindowTitle();

        void WindowResize(uint32_t width, uint32_t height);

        void OnKeyDown(uint8_t) {}
        void OnKeyUp(uint8_t) {}

        // Accessors
        uint32_t GetWidth() const { return m_width; }
        uint32_t GetHeight() const { return m_height; }
        uint64_t GetFrameCounter() const { return m_framecounter; }
        const char* GetTitle() const { return m_title.c_str(); }
        const std::string GetStringTitle() const { return m_title; }
        std::string GetAssetsPath() { return m_assetPath; }
        bool IsInitialized() const { return m_initialized; }

        // Mutators
        void SetWidth(uint32_t width) { m_width = width; }
        void SetHeight(uint32_t height) { m_height = height; }
        
        static uint32_t GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags props, VkPhysicalDeviceMemoryProperties deviceMemoryProperties);

    private:
        void InitVulkan();
        void SetupPipeline();

        void CreateInstance();
        void CreateSurface();
        void CreateDevice();
        void CreateSwapchain(uint32_t *w, uint32_t *h, bool vsync);
        void CreateRenderPass();
        void CreateFrameBuffers();
        void AllocateCommandBuffers();
        void CreateSyncObjects();

        void PopulateCommandBuffer(uint64_t bufferIndex, uint64_t imgIndex);
        void SubmitCommandBuffer(uint64_t index);
        void PresentImage(uint32_t index);

        void DestroyInstance();
        void DestroySurface();

        void CreateVertexBuffer();
        void CreatePipelineLayout();
        void CreatePipelineObjects();

        std::unique_ptr<VKModel> m_model;

        VkShaderModule LoadShader(std::string filename);

        // Vertex layout
        struct Vertex {
            float position[3];
            float color[4];
        };

        // Vertex buffer
        struct {
            VkDeviceMemory memory; // handle to the device memory backing the vertex buffer
            VkBuffer buffer;       // handle to the vulkan buffer object that the memory is bound to
        } m_vertices;


        std::string m_title;
        std::string m_assetPath;
        uint32_t m_width;
        uint32_t m_height;
        float m_aspect_ratio;
        Platform &m_platform;
        StepTimer m_timer;

        bool m_initialized;

        uint64_t m_framecounter;
        uint32_t m_command_buffer_index = 0;
        uint32_t m_command_buffer_count = 0;

        VkPhysicalDeviceProperties m_deviceProperties;
        VkPhysicalDeviceFeatures m_deviceFeatures;
        VkPhysicalDeviceMemoryProperties m_deviceMemoryProperties;

        VKCommonParameters m_vkparams;
        VKGraphicsParameters m_graphics;

        char m_lastFPS[32]; // string to hold frames per second
};
