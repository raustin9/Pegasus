#include "application.hh"
#include "core/events.hh"
// #include "renderer/vulkan/renderer.hh"
#include "renderer/renderer_frontend.hh"
#include "game_types.hh"
#include <chrono>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

static Settings settings = {};

struct ApplicationState {
    Pegasus::Game* game_inst;
    uint32_t width = 0;
    uint32_t height = 0;
    bool is_running = false;
    bool is_suspended = true;
    bool initialized = false;
    float last_time;

    char last_fps[32];
    uint64_t framecounter;
    StepTimer timer;
    std::string name;
    std::string asset_path; // TODO: move this file subsystem
};

static ApplicationState app_state = {};

Application::Application(Pegasus::Game& game, std::string name, uint32_t width, uint32_t height, std::string assetPath) {
}

bool
Application::Create(Pegasus::Game& game, std::string name, uint32_t width, uint32_t height, std::string asset_path) {
    // Application Init steps
    settings = {};

    app_state.name = name;
    app_state.asset_path = asset_path;

    // app_state.width = width;
    // app_state.height = height;

    // TODO: set this to be configurable
    settings.enableValidation = true;
    settings.enableVsync = false; // disable vsync for higher fps

    // Startup subsystems
    /* TODO: Logging startup */
    InputHandler::Startup();

    app_state.is_running = true;
    app_state.is_suspended = false;

    if (!EventHandler::Startup()) {
        std::cout << "Error: failed to initialize event handler" << std::endl;
        return false;
    }
    std::cout << "Event System created..." << std::endl;

    // Register for events
    EventHandler::Register(EVENT_CODE_APPLICATION_QUIT, nullptr, [](uint16_t code, void* sender, void* listener, EventContext data) -> bool {
            Application::OnEvent(code, sender, listener, data);
            return true;});

    EventHandler::Register(EVENT_CODE_KEY_PRESSED, nullptr, [](uint16_t code, void* sender, void* listener, EventContext data) -> bool {
        Application::OnKey(code, sender, listener, data);
        return true;});
    
    EventHandler::Register(EVENT_CODE_RESIZED, nullptr, [](uint16_t code, void* sender, void* listener, EventContext data) -> bool {
        Application::OnResize(code, sender, listener, data);
        return true;});

    EventHandler::Register(EVENT_CODE_MOUSE_MOVED, nullptr, [](uint16_t code, void* sender, void* listener, EventContext data) -> bool {
        Application::OnMouseMove(code, sender, listener, data);
        return true;});

    EventHandler::Register(EVENT_CODE_KEY_RELEASED, nullptr, [](uint16_t code, void* sender, void* listener, EventContext data) -> bool {
        Application::OnKey(code, sender, listener, data);
        return true;});

    // Init the platform
    if (!Platform::Startup(name, width, height)) {
        std::cout << "Error: failed to initialize Platform Layer" << std::endl;
        exit(1);
    }
    std::cout << "Platform created" << std::endl;

    if (!Renderer::Initialize(name, asset_path, width, height, {true, false})) {
        std::cout << "Error: failed to initialize Renderer Subsystem" << std::endl;
        exit(1);
    }
    std::cout << "Renderer created" << std::endl;

    // m_renderer.OnInit();

    app_state.initialized = true;
    return true;

}

Application::~Application() {
    
}

// Event loop of the application
bool
Application::run() {
    // create temporary objects using the renderer API
    // TODO: remove these and do this through UI
    Pegasus::GameObject obj = Pegasus::Game::NewGameObject();
    Pegasus::GameObject obj2 = Pegasus::Game::NewGameObject();
    
    obj.vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.87f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.51f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.43f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 0.5f, 0.0f}}
    };
    obj.indices = {
        0, 1, 2, 2, 3, 0
    };
    // obj.vertices = {
    //     { {-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 1.0f} },
    //     { {0.5f,  -0.5f,  0.5f}, {1.0f, 0.0f, 1.0f} },
    //     { {-0.5f, 0.5f,  0.5f}, {1.0f, 1.0f, 0.0f} },
    //     { {0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f} },
    //     { {-0.5f, 0.5f,  -0.5f}, {1.0f, 0.0f, 0.0f} },
    //     { {0.5f,  0.5f,  -0.5f}, {0.0f, 1.0f, 0.0f} },
    //     { {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f} },
    //     { {0.5f,  -0.5f, -0.5f}, {0.0f, 0.0f, 0.0f} },
    // };
    // obj.vertices = {
    //     { {-0.5f, -0.5f, -0.49f}, {0.5f, 0.f, 0.f} },
    //     { {0.5f, -0.5f, -0.5f}, {0.f, 0.f, 0.f} },
    //     { {0.5f,  0.5f, -0.49f}, {0.5f, 0.5f, 0.f} },
    //     { {-0.5f,  0.5f,  -0.5f}, {0.5f, 0.f, 0.5f} },
    //     { {-0.5f, -0.5f, 0.5f}, {0.f, 0.5f, 0.f} },
    //     { {0.5f, -0.5f, 0.5f}, {0.f, 0.f, 0.5f} },
    //     { {0.5f,  0.5f,  0.5f}, {0.5f, 0.5f, 0.5f} },
    //     { {-0.5f,  0.5f,  0.5f}, {0.f, 0.5f, 0.5f} },
    // };
    // obj.indices = {
    //     0, 1, 2, 2, 3, 0,
    //     4, 5, 6, 6, 7, 4,
    //     0, 4, 7, 7, 3, 0,
    //     1, 5, 6, 6, 2, 1,
    //     3, 7, 6, 6, 2, 3,
    //     0, 1, 5, 5, 4, 0,
    // };
    
    obj2.vertices = {
        {{-0.5f, -0.5f, 0.5f}, {0.3f, 0.87f, 0.0f}},
        {{0.5f, -0.5f, 0.5f}, {0.81f, 1.0f, 0.9f}},
        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.43f}},
        {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.5f, 0.0f}}
    };
    obj2.indices = {
        0, 1, 2, 3, 0, 2
    };

    Renderer::CreateModel(obj);
    Renderer::CreateModel(obj2);

    // Application Event loop
    while (app_state.is_running) {
        if (!Platform::pump_messages())
            app_state.is_running = false;

        if (!app_state.is_suspended) {
            // Update timer
            app_state.timer.Tick(nullptr);
            // m_timer.Tick(nullptr);
            static auto startTime = std::chrono::high_resolution_clock::now();
            auto currentTime = std::chrono::high_resolution_clock::now();
            float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

            UBO ubo{};
            glm::mat4 model = glm::rotate(
                glm::mat4(1.0f), 
                time * glm::radians(90.0f),
                glm::vec3(0.0f, 0.0f, 1.0f));

            glm::mat4 view = glm::lookAt(
                glm::vec3(2.0f, 2.0f, 2.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 1.0f));

            glm::mat4 proj = glm::perspective(
                // 1.f + glm::cos(0.7f * time) *
                glm::radians(45.0f),
                static_cast<float>(app_state.width) / static_cast<float>(app_state.height), 0.1f, 10.0f);

            ubo.projectionView = proj * view * model;

            // Update FPS and framecount
            // snprintf(m_lastFPS, static_cast<size_t>(32), "%u fps", m_timer.GetFPS());
            snprintf(app_state.last_fps, static_cast<size_t>(32), "%u fps", app_state.timer.GetFPS());
            app_state.framecounter++;

            // Render a frame
            RenderPacket packet = {};
            packet.ubo = ubo;
            packet.delta_time = time;
            Renderer::DrawFrame(packet);
        }
        
        if (app_state.framecounter % 300 == 0) {
            Platform::set_title(
                // app_state.name + " - " + std::string(m_lastFPS)
                app_state.name + " - " + std::string(app_state.last_fps)
            );
            // Platform::set_title(
            //     m_name + " - " + std::string(m_lastFPS)
            // );
        }
    }

    std::cout << "Shutting down application" << std::endl;

    EventHandler::Unregister(EVENT_CODE_APPLICATION_QUIT, nullptr);
    EventHandler::Unregister(EVENT_CODE_KEY_PRESSED, nullptr);
    EventHandler::Unregister(EVENT_CODE_KEY_RELEASED, nullptr);
    EventHandler::Unregister(EVENT_CODE_RESIZED, nullptr);

    EventHandler::Shutdown();
    InputHandler::Shutdown();
    Renderer::Shutdown();
    Platform::Shutdown();
    std::cout << "Application shutdown successfully" << std::endl;
    return true; 
}

/////////////////////////////////////
// ---------- CALLBACKS ---------- //
/////////////////////////////////////

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
            app_state.is_running = false;
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
   
    if (code == EVENT_CODE_RESIZED) {
        uint32_t w = context.u32[0];
        uint32_t h = context.u32[1];

        // Check if either the width or height are different
        if (app_state.width != w || app_state.height != h) {
            printf("[%i, %i] != [%i, %i]\n", app_state.width, app_state.height, w, h);
            app_state.width = w;
            app_state.height = h;

            // Handle minimization
            if (w == 0 || h == 0) {
                std::cout << "Application minimized" << std::endl;
                // TODO: add suspended states to the application 
                app_state.is_suspended = true;
                return true;
            } else {
                // TODO: check if app is suspended and only act if not
                if (app_state.is_suspended) {
                    std::cout << "Window restored. Resuming application" << std::endl;
                    app_state.is_suspended = false;
                }
                Renderer::OnResize(w, h);
            }
        }
    }

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
            EventHandler::Fire(EVENT_CODE_APPLICATION_QUIT, 0, data);

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


void 
Application::GetFramebufferSize(uint32_t& width, uint32_t& height) {
    width = app_state.width;
    height = app_state.height;
}