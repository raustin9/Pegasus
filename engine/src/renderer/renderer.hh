#pragma once

#include "stdafx.hh"
#include "vkcommon.hh"
#include "platform/platform.hh"

#include <cstdint>

class Renderer {
    public:
        Renderer(std::string name, uint32_t width, uint32_t height, Platform& platform);

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
        void CreateDevice();

        void DestroyInstance();
        void DestroySurface();


        std::string m_title;
        uint32_t m_width;
        uint32_t m_height;
        float m_aspect_ratio;
        Platform &m_platform;

        bool m_initialized;

        uint64_t m_framecounter;
        uint32_t m_command_buffer_index = 0;
        uint32_t m_command_buffer_count = 0;

        VkPhysicalDeviceProperties m_deviceProperties;
        VkPhysicalDeviceFeatures m_deviceFeatures;
        VkPhysicalDeviceMemoryProperties m_deviceMemoryProperties;

        VKCommonParameters m_vkparams;

//        VkPhysicalDeviceProperties m_deviceProperties;
//        VkPhysicalDeviceMemoryProperties m_DeviceMemoryProperties;
//        VkPhysicalDeviceFeatures m_deviceFeatures;
};
