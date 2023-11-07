# Yet Another Engine
The purpose of this project is to create a generic rendering engine template that can be cloned and used to create a basic cross-platform (Windows & Linux) application with a basic event handler. It will open a window with a vulkan context ready to render on, so I don't have to create a new one (a lot of work) every time that I want to make a new application.

## Architecture
The components & subsystems currently implimented:
- Platform
    - Handles platform-specific behavior like windowing, printing to console, and potentially more in the future depending on required/desired functionality
    - Currently supported platforms:
        - Linux
        - Windows
        - (Mac planned for the future if I can get one to test on)
- Event Subsystem
    - Other subsystems and components can register for an event using a callback function
    - When an event is triggered, the event handler will notify all registered components for that event using their callback functions
- Input Subsystem
    - Handles forms of input captured from the platform layer
    - Keyboard, mouse, window resize, ec
    - Uses the event handler subsystem to fire an event for any listeners for that type of input
- Renderer Subsystem
    - Handles rendering to the window
    - Vulkan (OpenGL, DirectX potentially in the future)

## Usage:
Building on Linux:
The linux build uses GNU Make to build the source.
Run `make` or `make all` to build
You can then run `bin/testbed` to build the current testbed version. You can also run `make run` to do the same command
