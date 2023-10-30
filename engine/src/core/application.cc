#include "application.hh"

Application::Application(std::string name, uint32_t width, uint32_t height)
    : _name(name), _platform{name, width, height} {
    std::cout << "Got past platform init" << std::endl;
    _should_quit = false;

    _platform.create_window();
    std::cout << "Got past window init" << std::endl;
}

bool
Application::run() {
    while (!_should_quit) {
        _should_quit = _platform.pump_messages();

        if (!_should_quit) {
            // update and render
        }
    }

    std::cout << "Shutting down application" << std::endl;
    return false; 
}
