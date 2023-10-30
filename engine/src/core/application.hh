#pragma once
#include <platform/platform.hh>

class Application {
    public:
        Application(std::string name, uint32_t width, uint32_t height);
        bool run(); // event loop
        // get_framebuffer_size(uint32_t* width, uint32_t* height);
    private:
        std::string _name;
        Platform _platform;
        bool _should_quit;
};
