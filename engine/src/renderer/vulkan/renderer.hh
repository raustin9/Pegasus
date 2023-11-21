#pragma once

#include "stdafx.hh"
#include "vkcommon.hh"
#include "platform/platform.hh"
#include "renderer/render_types.inl"
#include "vkmodel.hh"
#include "vkpipeline.hh"

#include <cstdint>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

// Structure for Uniform Buffer Object
struct UBO {
    alignas (16) glm::mat4 projectionView{1.f};
    // glm::vec3 lightDirection = glm::normalize(glm::vec3(1.f, -3.f, -1.f));
};

// Structure for a render packet
// This is sent from the application to the renderer
// The renderer uses information in this as data to render
struct RenderPacket {
    UBO GlobalUBO;
};

class Renderer {
    public:
        Renderer();

        void Initialize(std::string title, std::string assetPath,  uint32_t width, uint32_t height);
        void OnInit();
        void OnUpdate();
        void OnRender();
        void OnDestroy();

        const std::string GetDeviceName();

        void WindowResize(uint32_t width, uint32_t height);

        void OnKeyDown(uint8_t) {}
        void OnKeyUp(uint8_t) {}

        void RenderFrame();

        // Accessors
        uint32_t GetWidth() const { return m_width; }
        uint32_t GetHeight() const { return m_height; }
        std::string GetAssetsPath() const { return m_assetPath; }
        const char* GetTitle() const { return m_title.c_str(); }

        bool IsInitialized() const { return m_initialized; }

        // // Mutators
        void SetWidth(uint32_t width) { m_width = width; }
        void SetHeight(uint32_t height) { m_height = height; }

        // Public Interface
        void BeginFrame();
        void EndFrame();

        // Static members
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
        static uint32_t GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags props, VkPhysicalDeviceMemoryProperties deviceMemoryProperties);
        static VkShaderModule LoadShader(VKCommonParameters& vkparams, std::string filename);
        static VkCommandBuffer BeginSingleTimeCommands(VKCommonParameters& params);
        static void EndSingleTimeCommands(VKCommonParameters& params, VkCommandBuffer commandBuffer);

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
        void CreateDescriptorSetLayout();
        void CreateDescriptorSets();
        void CreateDescriptorPool();

        void PopulateCommandBuffer(uint64_t bufferIndex, uint64_t imgIndex);
        void SubmitCommandBuffer(uint64_t index);
        void PresentImage(uint32_t index);
        void UpdateUniformBuffer(uint32_t currentImage);

        void DestroyInstance();
        void DestroySurface();

        void CreateVertexBuffer();
        void CreateUniformBuffer();
        void CreatePipelineLayout();
        void CreatePipelineObjects();

        VkResult AcquireNextImage(uint32_t* imageIndex);

        std::string m_title;
        std::unique_ptr<VKModel> m_model;
        std::unique_ptr<VKPipeline> m_pipeline;


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


        std::string m_assetPath;
        uint32_t m_width;
        uint32_t m_height;
        float m_aspect_ratio;
        // Platform &m_platform;

        bool m_initialized;
        uint32_t m_current_frame_index = 0;
        uint32_t m_command_buffer_index = 0;
        uint32_t m_command_buffer_count = 0;

        std::vector <std::unique_ptr<VKBuffer> > m_uboBuffers;

        VkPhysicalDeviceProperties m_deviceProperties;
        VkPhysicalDeviceFeatures m_deviceFeatures;
        VkPhysicalDeviceMemoryProperties m_deviceMemoryProperties;

        VKCommonParameters m_vkparams;
};
