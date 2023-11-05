#include "input.hh"
#include "core/events.hh"

InputHandler::InputHandler(EventHandler &eh) 
    : m_eventHandler(eh),
      m_keyboardCurrent{},
      m_keyboardPrev{},
      m_mouseCurrent{},
      m_mousePrev{}
{
    m_initialized = true;
    std::cout << "Input subsystem initialized..." << std::endl;
}

InputHandler::~InputHandler() {
    m_initialized = false;
}

void
InputHandler::Update(double deltaTime) {
    if (!m_initialized)
        return;

    // Copy current state to the previous states
    m_keyboardPrev = m_keyboardCurrent;
    m_mousePrev = m_mouseCurrent;
}

void
InputHandler::ProcessKey(Keys key, bool pressed) {
    // Only do anything if the state has changed
    if (m_keyboardCurrent.keys[key] != pressed) {
        m_keyboardCurrent.keys[key] = pressed;

        // TODO: Fire an event for immediate processing
        EventContext data = {};
        data.u16[0] = static_cast<uint16_t>(key);
        m_eventHandler.Fire(
                pressed ? EVENT_CODE_KEY_PRESSED : EVENT_CODE_KEY_RELEASED,
                0,
                data);
    }
}

void
InputHandler::ProcessMouseMove(int32_t x, int32_t y) {
    // Only do anything if the state actually changed
    if (m_mouseCurrent.x != x || m_mouseCurrent.y != y) {
        m_mouseCurrent.x = x;
        m_mouseCurrent.y = y;

        // TODO: fire an event
        EventContext data = {};
        data.u16[0] = x;
        data.u16[1] = y;
        m_eventHandler.Fire(EVENT_CODE_MOUSE_MOVED, nullptr, data);

    }
}

void
InputHandler::ProcessMouseWheel(int16_t zDelta) {

}

// Set the parameters to the current position of the mouse's x and y positions
void
InputHandler::GetMousePosition(int32_t &x, int32_t &y) {
    x = m_mouseCurrent.x;
    y = m_mouseCurrent.y;
}

//
// PRIVATE METHODS
//

bool
InputHandler::_isKeyDown(Keys key) {
    if (!m_initialized)
        return false;

    return m_keyboardCurrent.keys[key] == true;;
}

bool
InputHandler::_isKeyUp(Keys key) {
    if (!m_initialized)
        return false;

    return m_keyboardCurrent.keys[key] == false;
}
bool
InputHandler::_wasKeyDown(Keys key) {
    if (!m_initialized)
        return false;

    return m_keyboardPrev.keys[key] == true;
}
bool
InputHandler::_wasKeyUp(Keys key) {
    if (!m_initialized)
        return false;

    return m_keyboardPrev.keys[key] == false;
}

bool
InputHandler::_isButtonDown(Buttons button) {
    if (!m_initialized)
        return false;

    return m_mouseCurrent.buttons[button] == true;
}

bool
InputHandler::_isButtonUp(Buttons button) {
    if (!m_initialized)
        return false;

    return m_mouseCurrent.buttons[button] == false;
}
bool
InputHandler::_wasButtonDown(Buttons button) {
    if (!m_initialized)
        return false;

    return m_mousePrev.buttons[button] == true;

}
bool
InputHandler::_wasButtonUp(Buttons button) {
    if (!m_initialized)
        return false;

    return m_mousePrev.buttons[button] == false;
}


