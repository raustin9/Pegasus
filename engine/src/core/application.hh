#pragma once
#include "renderer/vulkan/renderer.hh"
#include "platform/platform.hh"
#include "stdafx.hh"
#include "events.hh"
#include "platform/platform_timer.hh"

struct Settings {
    bool enableValidation = false;
    bool enableVsync = false;
};

class  QAPI Application {
    public:
        Application(std::string name,  uint32_t width, uint32_t height, std::string assetPath = "./assets");
        ~Application();
        bool run(); // event loop

        static Settings settings;
        bool OnEvent(uint16_t code, void* sender, void* listener, EventContext context);
        bool OnKey(uint16_t code, void* sender, void* listener, EventContext context);
        bool OnMouseMove(uint16_t code, void* sender, void* listener, EventContext context);
        bool OnResize(uint16_t code, void* sender, void* listener, EventContext context);
    private:
        std::string m_name;
        std::string m_assetPath;

        // EventHandler m_eventHandler;
        // Platform m_platform;
        // Renderer m_renderer;
        StepTimer m_timer;

        uint64_t m_framecounter;
        char m_lastFPS[32]; // string to hold frames per second
};

