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

## Dependencies
- GLM: 
    - the math library used for linear algebra (might be replaced with from-scrath lib in hte future) 
    - You can find how to use GLM [here](https://github.com/g-truc/glm)
- Vulkan SDK: 
    - In order to build, you will need to have the vulkan SDK installed. 
    - If you do not have it installed, you can find good resources on how to use it [here](ihttps://vulkan-tutorial.com/Introduction)
- Make:
    - The current build system uses Make for both Windows and Linux.
    - Make likely comes installed with any distribution of Linux
        - If not, use your distro's package manager to install it
    - For Windows, you can download it [here](https://gnuwin32.sourceforge.net/packages/make.htm).
- Clang (c++17);
    - `clang` is the current compiler we are using. You can install that [here](https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=&ved=2ahUKEwiKtd_1rtiCAxUqPEQIHY1UDYcQFnoECBEQAQ&url=https%3A%2F%2Fclang.llvm.org%2Fget_started.html&usg=AOvVaw3ljm1g5TDbtBViG5dfMXra&opi=89978449).
    - **NOTE**: Make sure LLVM is up-to-date enough to compile C++17.
- STB:
    - Used for image loading (mainly for textures)
    - Header library and is already included in the project directory.
- GLSLC:
    - Compiler for `.vert` and `.frag` shader files to `.spv` IR
    - Binary in the `tooling` director in the project directory
        - `glslc.exe` for Windows
        - `glslc` for Linux
    - You can modify the *Makefile*s to point to your local copy if you have it by changing the `GLSLC` variable to the path of your copy

## Usage:
- Building on Windows:
    - Make sure all dependencies are installed and up to date.
    - Run the `build-all.bat` script to build the library.
    - Once built, run `.\bin\testbed.exe` to run the output
    - To clean the build, run `.\clean.bat` to clean out all `.o` files
- Building on Linux:
    - Make sure all dependencies are installed and up to date.
    - Run the `build-all.sh` script to build the library.
    - Once built, run `.\bin\testbed` to run the output
    - To clean the build, run `.\clean.sh` to clean out all `.o` files