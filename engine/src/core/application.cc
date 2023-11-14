#include "application.hh"
#include "core/events.hh"
#include <chrono>
#include <glm/glm.hpp>

Settings Application::settings = {};

Application::Application(std::string name, uint32_t width, uint32_t height, std::string assetPath)
    : m_name(name), 
    m_assetPath(assetPath), 
    m_eventHandler{}, 
    m_platform{name, width, height, m_eventHandler},  
    m_renderer{name, m_assetPath, width, height, m_platform},
    m_timer{} {

    // TODO: set this to be configurable
    Application::settings.enableValidation = true;
    Application::settings.enableVsync = false; // disable vsync for higher fps
    m_should_quit = false;

    // Register for events
    m_eventHandler.Register(EVENT_CODE_APPLICATION_QUIT, nullptr, [&, this](uint16_t code, void* sender, void* listener, EventContext data) -> bool {
            this->OnEvent(code, sender, listener, data);
            return true;});

    m_eventHandler.Register(EVENT_CODE_KEY_PRESSED, nullptr, [&, this](uint16_t code, void* sender, void* listener, EventContext data) -> bool {
        this->OnKey(code, sender, listener, data);
        return true;});
    
    m_eventHandler.Register(EVENT_CODE_RESIZED, nullptr, [&, this](uint16_t code, void* sender, void* listener, EventContext data) -> bool {
        this->OnResize(code, sender, listener, data);
        return true;});

    m_eventHandler.Register(EVENT_CODE_MOUSE_MOVED, nullptr, [&, this](uint16_t code, void* sender, void* listener, EventContext data) -> bool {
        this->OnMouseMove(code, sender, listener, data);
        return true;});

    m_eventHandler.Register(EVENT_CODE_KEY_RELEASED, nullptr, [&, this](uint16_t code, void* sender, void* listener, EventContext data) -> bool {
        this->OnKey(code, sender, listener, data);
        return true;});


    std::cout << "Platform created" << std::endl;

    m_platform.create_window();
    std::cout << "Window created" << std::endl;

    m_renderer.OnInit();
}

Application::~Application() {
    
}

// Event loop of the application
bool
Application::run() {
    while (!m_should_quit) {
        if (m_platform.pump_messages() == true)
            m_should_quit = true;

        if (!m_should_quit && m_renderer.IsInitialized()) {
            // Update timer
            m_timer.Tick(nullptr);

            // Update FPS and framecount
            snprintf(m_lastFPS, static_cast<size_t>(32), "%u fps", m_timer.GetFPS());
            m_framecounter++;

            // Render a frame
            m_renderer.BeginFrame();
            m_renderer.EndFrame();
        }
        
        if (m_framecounter % 300 == 0) {
            m_platform.set_title(
                m_name + " - " + m_renderer.GetDeviceName() + " - " + std::string(m_lastFPS)
            );
        }
    }

    std::cout << "Shutting down application" << std::endl;
    m_renderer.OnDestroy();
    return false; 
}

// Behavior for events
// FOR NOW: handle application shutdown signal
bool 
Application::OnEvent(uint16_t code, void* sender, void* listener, EventContext context) {
    (void)context;
    (void)listener;
    (void)sender;
    switch(code) {
        case EVENT_CODE_APPLICATION_QUIT: {
            std::cout << "EVENT_CODE_APPLICATION_QUIT received. Shutting down..." << std::endl;
            m_should_quit = true;
            return true;
        }; break;
        default:
            break;
    }

    return false;
}

// TODO: register an event for when the mouse moves
//       print out the location of the mouse
bool 
Application::OnMouseMove(uint16_t code, void* sender, void* listener, EventContext context) {
    (void)context;
    (void)listener;
    (void)sender;
    switch(code) {
        case EVENT_CODE_MOUSE_MOVED: {
            printf("X: %i, Y: %i\n", context.u16[0], context.u16[1]);
            return true;
        }; break;
        default:
            break;
    }

    return false;
}

// Resize callback for resizing the window
bool 
Application::OnResize(uint16_t code, void* sender, void* listener, EventContext context) {
    (void)code;
    (void)context;
    (void)listener;
    (void)sender;

    uint32_t w = context.u32[0];
    uint32_t h = context.u32[1];

    m_renderer.WindowResize(w, h);
    return false;
}

// Behavior for keyboard press events
bool
Application::OnKey(uint16_t code, void* sender, void* listener, EventContext context) {
    (void)context;
    (void)listener;
    (void)sender;
    if (code == EVENT_CODE_KEY_PRESSED) {
        uint16_t keycode = context.u16[0];
        if (keycode == KEY_ESCAPE) {
            EventContext data = {};
            m_eventHandler.Fire(EVENT_CODE_APPLICATION_QUIT, 0, data);

            return true;
        } else {
            printf("'%c' key pressed in window\n", static_cast<char>(keycode));
        }
    } else if (code == EVENT_CODE_KEY_RELEASED) {
        uint16_t keycode = context.u16[0];
        printf("'%c' key released in window\n", keycode);
    }

    return false;
}
