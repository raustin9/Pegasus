#include "application.hh"

Settings Application::settings = {};

Application::Application(std::string name, uint32_t width, uint32_t height)
    : m_name(name), m_platform{name, width, height}, m_renderer{name, width, height, m_platform} {

    // TODO: set this to be configurable
    Application::settings.enableValidation = true;
    Application::settings.enableVsync = false; // disable vsync for higher fps
    m_should_quit = false;

    std::cout << "Platform created" << std::endl;

    m_platform.create_window();
    std::cout << "Window created" << std::endl;

    m_renderer.OnInit();
}

bool
Application::run() {
    while (!m_should_quit) {
        m_should_quit = m_platform.pump_messages();

        if (!m_should_quit && m_renderer.IsInitialized()) {
            m_renderer.OnUpdate();
            m_renderer.OnRender();
            // update and render
        }
    }

    std::cout << "Shutting down application" << std::endl;
    m_renderer.OnDestroy();
    return false; 
}
