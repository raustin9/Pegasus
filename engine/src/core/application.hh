#pragma once
#include "renderer/renderer.hh"
#include "platform/platform.hh"
#include "stdafx.hh"
#include "events.hh"

struct Settings {
    bool enableValidation = false;
    bool enableVsync = false;
};

class Application {
    public:
        Application(std::string name, uint32_t width, uint32_t height);
        ~Application();
        bool run(); // event loop

        static Settings settings;
        bool OnEvent(uint16_t code, void* sender, void* listener, EventContext context);
        bool OnKey(uint16_t code, void* sender, void* listener, EventContext context);
        bool OnMouseMove(uint16_t code, void* sender, void* listener, EventContext context);
    private:
        EventHandler m_eventHandler;
        Platform m_platform;
        Renderer m_renderer;


        std::string m_name;
        bool m_should_quit;
};

