#include "application.hh"
#include "core/events.hh"
// #include "renderer/vulkan/renderer.hh"
#include "renderer/renderer_frontend.hh"
#include "game_types.hh"
#include "containers/qvector.inl"
#include "core/qmemory.hh"
#include "core/qlogger.hh"
#include "memory/qlinear_allocator.hh"
#include <chrono>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

    qmemory::QLinearAllocator systems_allocator;

    uint64_t logging_system_memory_requirement;
    void* logging_system_state;

    uint64_t input_system_memory_requirement;
    void* input_system_state;

    uint64_t event_system_memory_requirement;
    void* event_system_state;

    uint64_t memory_system_memory_requirement;
    void* memory_system_state;
    
    uint64_t platform_system_memory_requirement;
    void* platform_system_state;
};
  
static ApplicationState* app_state;

Application::Application(Pegasus::Game& game, std::string name, uint32_t width, uint32_t height, std::string assetPath) {
}

bool
Application::Create(
    Pegasus::Game& game, 
    std::string name,   
    uint32_t width, 
    uint32_t height, 
    std::string asset_path
) {
    if (game.application_state) {
        qlogger::Error("Application::Create: called more than once");
        return false;
    }
    
    // Initialize Application State
    game.application_state = static_cast<ApplicationState*>(QAllocator::Allocate(1, sizeof(ApplicationState), MEMORY_TAG_APPLICATION));
    app_state = new (game.application_state) ApplicationState;
    app_state->game_inst = &game;
    app_state->is_running = false;
    app_state->is_suspended = false;

    // Setup the systems linear allocator
    uint64_t systems_allocator_total_size = 64 * 1024 * 1024; // 64 MB
    app_state->systems_allocator.Create(systems_allocator_total_size, nullptr);
    
    QAllocator::Initialize(app_state->memory_system_memory_requirement, nullptr);
    app_state->memory_system_state = app_state->systems_allocator.Allocate(app_state->memory_system_memory_requirement);
    QAllocator::Initialize(app_state->memory_system_memory_requirement, app_state->memory_system_state);

    app_state->name = name;
    app_state->asset_path = asset_path;

    // Startup subsystems
    qlogger::Initialize(app_state->logging_system_memory_requirement, nullptr);
    app_state->logging_system_state = app_state->systems_allocator.Allocate(app_state->logging_system_memory_requirement);
    if (!qlogger::Initialize(app_state->logging_system_memory_requirement, app_state->logging_system_state)) {
        qlogger::Error("Failed to initialize logging system.");
        return false;
    }

    InputHandler::Startup(app_state->input_system_memory_requirement, nullptr);
    app_state->input_system_state = app_state->systems_allocator.Allocate(app_state->input_system_memory_requirement);
    InputHandler::Startup(app_state->input_system_memory_requirement, app_state->input_system_state);


    EventHandler::Startup(app_state->event_system_memory_requirement, nullptr);
    app_state->event_system_state = app_state->systems_allocator.Allocate(app_state->event_system_memory_requirement);    
    if (!EventHandler::Startup(app_state->event_system_memory_requirement, app_state->event_system_state)) {
        qlogger::Error("Error: failed to initialize event handler");
        return false;
    }
    qlogger::Info("Event System created...");
    
    // Register for events
    EventHandler::Register(EVENT_CODE_APPLICATION_QUIT, nullptr, Application::OnEvent);
    EventHandler::Register(EVENT_CODE_KEY_PRESSED, nullptr, Application::OnKey);
    EventHandler::Register(EVENT_CODE_KEY_RELEASED, nullptr, Application::OnKey);
    EventHandler::Register(EVENT_CODE_RESIZED, nullptr, Application::OnResize);
    EventHandler::Register(EVENT_CODE_MOUSE_MOVED, nullptr, Application::OnMouseMove);

    Platform::Startup(app_state->platform_system_memory_requirement, nullptr, name, width, height);
    app_state->platform_system_state = app_state->systems_allocator.Allocate(app_state->platform_system_memory_requirement);
    if (!Platform::Startup(app_state->platform_system_memory_requirement, app_state->platform_system_state, name, width, height)) {
        qlogger::Error("Error: failed to initialize platform layer");
        exit(1);
    }
    qlogger::Info("Platform created.");

    if (!Renderer::Initialize(name, asset_path, width, height, {true, false})) {
        qlogger::Error("Error: failed to initialize renderer");
        exit(1);
    }
    qlogger::Info("Renderer created.");

    app_state->initialized = true;
    return true;
}

Application::~Application() {
}

// Event loop of the application
bool
Application::run() {
    app_state->is_running = true;

    Vector<uint64_t> v = Vector<uint64_t>(MEMORY_TAG_APPLICATION);
    v.push(25);
    v.push(26);
    v.push(27);
    v.push(28);  

    qlogger::Debug(QAllocator::GetUsageString().c_str());

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
    while (app_state->is_running) {
        if (!Platform::pump_messages())
            app_state->is_running = false;

        if (!app_state->is_suspended) {
            // Update timer
            app_state->timer.Tick(nullptr);
            static auto startTime = std::chrono::high_resolution_clock::now();
            auto currentTime = std::chrono::high_resolution_clock::now();
            float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

            if (!app_state->game_inst->Update(time)) {
                qlogger::Fatal("Game update failed. Shutting down.");
                app_state->is_running = false;
                break;
            }

            if (!app_state->game_inst->Render(time)) {
                qlogger::Fatal("Game render failed. Shutting down.");
                app_state->is_running = false;
                break;
            }

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
                static_cast<float>(app_state->width) / static_cast<float>(app_state->height), 0.1f, 10.0f);

            ubo.projectionView = proj * view * model;

            // Update FPS and framecount
            snprintf(app_state->last_fps, static_cast<size_t>(32), "%u fps", app_state->timer.GetFPS());
            app_state->framecounter++;

            // Render a frame
            RenderPacket packet = {};
            packet.ubo = ubo;
            packet.delta_time = time;
            Renderer::DrawFrame(packet);

            InputHandler::Update(time);
        }
        
        if (app_state->framecounter % 300 == 0) {
            Platform::set_title(
                // app_state->name + " - " + std::string(m_lastFPS)
                app_state->name + " - " + std::string(app_state->last_fps)
            );
        }
    }

    app_state->is_running = false;

    qlogger::Info("Shutting down application");

    EventHandler::Unregister(EVENT_CODE_APPLICATION_QUIT, nullptr);
    EventHandler::Unregister(EVENT_CODE_KEY_PRESSED, nullptr);
    EventHandler::Unregister(EVENT_CODE_KEY_RELEASED, nullptr);
    EventHandler::Unregister(EVENT_CODE_RESIZED, nullptr);

    EventHandler::Shutdown();
    InputHandler::Shutdown();
    qlogger::Shutdown(app_state->logging_system_state);
    Renderer::Shutdown();
    Platform::Shutdown();
    QAllocator::Shutdown();
    qlogger::Info("Application shutdown successfully.");
    return true; 
}

/////////////////////////////////////////
// ---------- CALLBACKS ---------- //
/////////////////////////////////////////

// Behavior for events
// FOR NOW: handle application shutdown signal
bool 
Application::OnEvent(uint16_t code, void* sender, void* listener, EventContext context) {
    (void)context;
    (void)listener;
    (void)sender;
    switch(code) {
        case EVENT_CODE_APPLICATION_QUIT: {
            qlogger::Info("EVENT_CODE_APPLICATION_QUIT received. Shutting down...");
            app_state->is_running = false;
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
            qlogger::Debug("X: %i, Y: %i", context.u16[0], context.u16[1]);
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
        if (app_state->width != w || app_state->height != h) {
            qlogger::Trace("[%i, %i] != [%i, %i]", app_state->width, app_state->height, w, h);
            app_state->width = w;
            app_state->height = h;

            // Handle minimization
            if (w == 0 || h == 0) {
                qlogger::Trace("Application minimized");
                // TODO: add suspended states to the application 
                app_state->is_suspended = true;
                return true;
            } else {
                // TODO: check if app is suspended and only act if not
                if (app_state->is_suspended) {
                    qlogger::Trace("Window restored. Resuming application");
                    app_state->is_suspended = false;
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
            qlogger::Debug("'%c' key pressed in window", static_cast<char>(keycode));
        }
    } else if (code == EVENT_CODE_KEY_RELEASED) {
        uint16_t keycode = context.u16[0];
        qlogger::Debug("'%c' key released in window", keycode);
    }

    return false;
}


void 
Application::GetFramebufferSize(uint32_t& width, uint32_t& height) {
    width = app_state->width;
    height = app_state->height;
}