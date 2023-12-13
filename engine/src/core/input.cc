#include "input.hh"
#include "core/events.hh"
#include "core/qlogger.hh"

// State for the input handler
struct InputState {
    bool initialized;
    uint32_t mousex;
    uint32_t mousey;

    KeyboardState keyboardCurrent{};
    KeyboardState keyboardPrev{};
    MouseState mouseCurrent{};
    MouseState mousePrev{};
};

// static InputState input_state = {};
static InputState* input_state_ptr;

void
InputHandler::Startup(uint64_t& memory_requirement, void* state) {
    memory_requirement = sizeof(InputState);
    if (state == nullptr) {
        return;
    }
    input_state_ptr = new ((InputState*)state) InputState;
    input_state_ptr->initialized = true;

    qlogger::Info("Input state initialized.");
    return;
}

void
InputHandler::Shutdown() {
    input_state_ptr->initialized = false;
    input_state_ptr = nullptr;
}


void
InputHandler::Update(double deltaTime) {
    (void)deltaTime;
    if (!input_state_ptr->initialized)
        return;

    // Copy current state to the previous states
    input_state_ptr->keyboardPrev = input_state_ptr->keyboardCurrent;
    input_state_ptr->mousePrev = input_state_ptr->mouseCurrent;
}

void
InputHandler::ProcessResize(uint32_t w, uint32_t h) {
    EventContext data = {};
    data.u32[0] = static_cast<uint32_t>(w);
    data.u32[1] = static_cast<uint32_t>(h);

    EventHandler::Fire(EVENT_CODE_RESIZED, nullptr, data);
}

void
InputHandler::ProcessKey(Keys key, bool pressed) {
    if (key == KEY_LALT) {
        qlogger::Debug("LALT\n");
    } else if (key == KEY_RALT) {
        qlogger::Debug("RALT\n");
    } else if (key == KEY_LCONTROL) {
        qlogger::Debug("LCONTROL\n");
    } else if (key == KEY_RCONTROL) {
        qlogger::Debug("RCONTROL\n");
    } else if (key == KEY_LSHIFT) {
        qlogger::Debug("LSHIFT\n");
    } else if (key == KEY_RSHIFT) {
        qlogger::Debug("RSHIFT\n");
    }
    // Only do anything if the state has changed
    if (input_state_ptr->keyboardCurrent.keys[key] != pressed) {
        input_state_ptr->keyboardCurrent.keys[key] = pressed;



        // TODO: Fire an event for immediate processing
        EventContext data = {};
        data.u16[0] = static_cast<uint16_t>(key);
        EventHandler::Fire(
                pressed ? EVENT_CODE_KEY_PRESSED : EVENT_CODE_KEY_RELEASED,
                0,
                data);
    }
}

void
InputHandler::ProcessMouseMove(int32_t x, int32_t y) {
    // Only do anything if the state actually changed
    if (input_state_ptr->mouseCurrent.x != x || input_state_ptr->mouseCurrent.y != y) {
        input_state_ptr->mouseCurrent.x = x;
        input_state_ptr->mouseCurrent.y = y;

        // TODO: fire an event
        EventContext data = {};
        data.u16[0] = x;
        data.u16[1] = y;
        EventHandler::Fire(EVENT_CODE_MOUSE_MOVED, nullptr, data);

    }
}

void
InputHandler::ProcessMouseWheel(int16_t zDelta) {
    (void)zDelta;
}

// Set the parameters to the current position of the mouse's x and y positions
void
InputHandler::GetMousePosition(int32_t &x, int32_t &y) {
    x = input_state_ptr->mouseCurrent.x;
    y = input_state_ptr->mouseCurrent.y;
}

//
// PRIVATE METHODS
//

bool
InputHandler::IsKeyDown(Keys key) {
    if (!input_state_ptr->initialized)
        return false;

    return input_state_ptr->keyboardCurrent.keys[key] == true;;
}

bool
InputHandler::IsKeyUp(Keys key) {
    if (!input_state_ptr->initialized) {
        qlogger::Warn("Input::IsKeyUp called when uninitialized");
        return false;
    }

    return input_state_ptr->keyboardCurrent.keys[key] == false;
}
bool
InputHandler::WasKeyDown(Keys key) {
    if (!input_state_ptr->initialized)
        return false;

    return input_state_ptr->keyboardPrev.keys[key] == true;
}
bool
InputHandler::WasKeyUp(Keys key) {
    if (!input_state_ptr->initialized)
        return false;

    return input_state_ptr->keyboardPrev.keys[key] == false;
}

bool
InputHandler::IsButtonDown(Buttons button) {
    if (!input_state_ptr->initialized)
        return false;

    return input_state_ptr->mouseCurrent.buttons[button] == true;
}

bool
InputHandler::IsButtonUp(Buttons button) {
    if (!input_state_ptr->initialized)
        return false;

    return input_state_ptr->mouseCurrent.buttons[button] == false;
}
bool
InputHandler::WasButtonDown(Buttons button) {
    if (!input_state_ptr->initialized)
        return false;

    return input_state_ptr->mousePrev.buttons[button] == true;

}
bool
InputHandler::WasButtonUp(Buttons button) {
    if (!input_state_ptr->initialized)
        return false;

    return input_state_ptr->mousePrev.buttons[button] == false;
}


