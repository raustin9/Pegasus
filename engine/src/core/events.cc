#include "events.hh"

static EventState event_state = {};

bool EventHandler::Startup() {
    if (event_state.initialized)
        return false;

    for (size_t i = 0; i < MAX_MESSAGE_CODES; i++)
        event_state.registered[i].events.clear();

    event_state.initialized = true;
    return true;
}

void EventHandler::Shutdown() {
    for (size_t i = 0; i < MAX_MESSAGE_CODES; i++) {
        event_state.registered[i].events.clear();
    }

    event_state.initialized = false;
}

// Registers to listen to events that are sent with the specified code
// listener/callback combos won't be registered twice, and will return 'false' if that is attempted
bool EventHandler::Register(
        uint16_t code,
        void* listener,
        CallbackFunc callback
        ) {
    // Check if the listener has already been registered
    size_t registered_count = event_state.registered[code].events.size();
    for (size_t i = 0; i < registered_count; i++) {
        if (event_state.registered[code].events[i].listener == listener)
            return false;
    }

    // Proceed with registration since it has not been registered yet
    RegisteredEvent event = {};
    event.listener = listener;
    event.callback = callback;
    event_state.registered[code].events.push_back(event);
    return true;
}

// Unregister an event with the specified code from the listener
// bool Unregister(uint16_t code, void* listener, PFN_on_event on_event);
// bool Unregister(uint16_t code, void* listener, bool (T::*)(uint16_t, void*, void*, EventContext));
bool EventHandler::Unregister(
        uint16_t code,
        void* listener
        ) {
    // Check that the listener is actually registered for the event
    if (event_state.registered[code].events.size() == 0)
        return false;

    // Iterate through the list of listeners registered for the event
    // and check if both the listener and the specified callback function
    // match. If they match, remove the listener from the vector
    // Removing element from vector at an index is obviously not efficient, but for
    // the sake of prototyping this is fine for now. Also there are not too many elements
    // that would be registered for an event, so even in worst case it is not that bad
    size_t registered_count = event_state.registered[code].events.size();
    for (size_t i = 0; i < registered_count; i++) {
        RegisteredEvent ev = event_state.registered[code].events[i];
        if (ev.listener == listener) {
            event_state.registered[code].events.erase(event_state.registered[code].events.begin() + i);
            return true;
        }
    }

    return false;
}

// Fire an event with the input code
// If the handler returns true, the event is considered handled
// If not, the handler passes on to any more listeners
bool EventHandler::Fire(uint16_t code, void* sender, EventContext context) {
    if (event_state.registered[code].events.size() == 0) {
        return false;
    }

    for (size_t i = 0; i < event_state.registered[code].events.size(); i++) {
        RegisteredEvent ev = event_state.registered[code].events[i];
        auto func = ev.callback;
        if (ev.callback(code, sender, ev.listener, context)) {
            // message was handled if callback returned true
            return true;
        }
    }

    return false;
}

// Accessors
bool EventHandler::GetInitialized() { return event_state.initialized; }
