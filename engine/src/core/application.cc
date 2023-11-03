#include "application.hh"

Settings Application::settings = {};

Application::Application(std::string name, uint32_t width, uint32_t height)
    :  m_eventHandler{}, m_platform{name, width, height, m_eventHandler},  m_renderer{name, width, height, m_platform}, m_name(name) {

    // TODO: set this to be configurable
    Application::settings.enableValidation = true;
    Application::settings.enableVsync = false; // disable vsync for higher fps
    m_should_quit = false;

    // Register to receive events for these codes
    m_eventHandler.Register<Application>(EVENT_CODE_APPLICATION_QUIT, nullptr, &Application::OnEvent);
    m_eventHandler.Register<Application>(EVENT_CODE_KEY_PRESSED, nullptr, &Application::OnKey);
    m_eventHandler.Register<Application>(EVENT_CODE_KEY_RELEASED, nullptr, &Application::OnKey);

    std::cout << "Platform created" << std::endl;

    m_platform.create_window();
    std::cout << "Window created" << std::endl;

    m_renderer.OnInit();
}

Application::~Application() {
    
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

// Callback function to handle events
bool 
Application::OnEvent(uint16_t code, void* sender, void* listener, EventContext context) {
    switch(code) {
        case EVENT_CODE_APPLICATION_QUIT: {
            m_should_quit = true;
        }; break;
        default:
            break;
    }

    return false;
}

bool
Application::OnKey(uint16_t code, void* sender, void* listener, EventContext context) {
    if (code == EVENT_CODE_KEY_PRESSED) {
        uint16_t keycode = context.u16[0];
    } else if (code == EVENT_CODE_KEY_RELEASED) {

    }

    return true;
}
