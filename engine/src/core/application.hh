#pragma once
#include "renderer/renderer.hh"
#include "platform/platform.hh"

struct Settings {
    bool enableValidation = false;
};

class Application {
    public:
        Application(std::string name, uint32_t width, uint32_t height);
        bool run(); // event loop

        static Settings settings;
    private:
        std::string m_name;
        Platform m_platform;
        Renderer m_renderer;
        bool m_should_quit;
};

