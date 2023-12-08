#include "input.hh"
#include "core/events.hh"
#include "core/qlogger.hh"

// State for the input handler
struct InputState {
    bool initialized = false;
    uint32_t mousex;
    uint32_t mousey;

    KeyboardState keyboardCurrent;
    KeyboardState keyboardPrev;
    MouseState mouseCurrent;
    MouseState mousePrev;
};

static InputState input_state = {};

void
InputHandler::Startup() {
    input_state.initialized = true;

    qlogger::Info("Input state initialized.");
}

void
InputHandler::Shutdown() {
    input_state.initialized = false;
}


void
InputHandler::Update(double deltaTime) {
    (void)deltaTime;
    if (!input_state.initialized)
        return;

    // Copy current state to the previous states
    input_state.keyboardPrev = input_state.keyboardCurrent;
    input_state.mousePrev = input_state.mouseCurrent;
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
        qlogger::Info("LALT\n");
    } else if (key == KEY_RALT) {
        qlogger::Info("RALT\n");
    } else if (key == KEY_LCONTROL) {
        qlogger::Info("LCONTROL\n");
    } else if (key == KEY_RCONTROL) {
        qlogger::Info("RCONTROL\n");
    } else if (key == KEY_LSHIFT) {
        qlogger::Info("LSHIFT\n");
    } else if (key == KEY_RSHIFT) {
        qlogger::Info("RSHIFT\n");
    }
    // Only do anything if the state has changed
    if (input_state.keyboardCurrent.keys[key] != pressed) {
        input_state.keyboardCurrent.keys[key] = pressed;



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
    if (input_state.mouseCurrent.x != x || input_state.mouseCurrent.y != y) {
        input_state.mouseCurrent.x = x;
        input_state.mouseCurrent.y = y;

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
    x = input_state.mouseCurrent.x;
    y = input_state.mouseCurrent.y;
}

//
// PRIVATE METHODS
//

bool
InputHandler::_isKeyDown(Keys key) {
    if (!input_state.initialized)
        return false;

    return input_state.keyboardCurrent.keys[key] == true;;
}

bool
InputHandler::_isKeyUp(Keys key) {
    if (!input_state.initialized)
        return false;

    return input_state.keyboardCurrent.keys[key] == false;
}
bool
InputHandler::_wasKeyDown(Keys key) {
    if (!input_state.initialized)
        return false;

    return input_state.keyboardPrev.keys[key] == true;
}
bool
InputHandler::_wasKeyUp(Keys key) {
    if (!input_state.initialized)
        return false;

    return input_state.keyboardPrev.keys[key] == false;
}

bool
InputHandler::_isButtonDown(Buttons button) {
    if (!input_state.initialized)
        return false;

    return input_state.mouseCurrent.buttons[button] == true;
}

bool
InputHandler::_isButtonUp(Buttons button) {
    if (!input_state.initialized)
        return false;

    return input_state.mouseCurrent.buttons[button] == false;
}
bool
InputHandler::_wasButtonDown(Buttons button) {
    if (!input_state.initialized)
        return false;

    return input_state.mousePrev.buttons[button] == true;

}
bool
InputHandler::_wasButtonUp(Buttons button) {
    if (!input_state.initialized)
        return false;

    return input_state.mousePrev.buttons[button] == false;
}


