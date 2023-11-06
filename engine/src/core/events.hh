#pragma once
/*
 *  This file holds the structure and interface for the event handler
 *
 *  The event handler is responsible for sending messages for events across different parts of the application
 *  This is done through "registering" something to listen for an event, and any part of the application that
 *  is registered for a given event will receive a message when an message for that event is fired
 */

#include "stdafx.hh"

#define MAX_MESSAGE_CODES 16384

struct EventContext {
    union {
        int64_t  i64[2];
        uint64_t u64[2];
        double   f64[2];

        int32_t  i32[4];
        uint32_t u32[4];
        float    f32[4];

        int16_t  i16[8];
        uint16_t u16[8];
        
        int8_t  i8[16];
        uint8_t u8[16];
    };
};

enum EventCodes {
    EVENT_CODE_APPLICATION_QUIT = 0x01,

    // Usage: uint16_t key_code = data.u16[0];
    EVENT_CODE_KEY_PRESSED = 0x02,

    // Usage: uint16_t key_code = data.u16[0];
    EVENT_CODE_KEY_RELEASED = 0x03,

    // Usage: uint16_t button = data.u16[0];
    EVENT_CODE_BUTTON_PRESSED = 0x04,

    // Usage: uint16_t button = data.u16[0];
    EVENT_CODE_BUTTON_RELEASED = 0x05,

    // Usage:
    //        uint32_t mouse_x = data.u32[0];
    //        uint32_t mouse_y = data.u32[1];
    EVENT_CODE_MOUSE_MOVED = 0x06,

    // Usage: uint8_t z_delta = data.u8[0];
    EVENT_CODE_MOUSE_WHEEL = 0x07,

    // Usage: 
    //        uint16_t width = data.u16[0];
    //        uint16_t height = data.u16[1];
    EVENT_CODE_RESIZED = 0x08,
    
    MAX_EVENT_CODE = 0xFF
};

using  CallbackFunc = std::function <bool (uint16_t code, void* sender, void* listener, EventContext data)>;

// Holds a pointer to the listener and the callback
// function to be used to communicate to them
struct RegisteredEvent {
    void* listener;

    CallbackFunc callback;
    // bool (T::*callback)(uint16_t code, void* sender, void* listener, EventContext data);
};

// Holds a vector of registered events
struct EventCodeEntry {
    
    std::vector <RegisteredEvent>  events;
};

// Holds an array of events
// One for each event code
struct EventState {
    EventCodeEntry registered[MAX_MESSAGE_CODES];
};

class EventHandler {
    public:
        EventHandler() :m_state{} {
            // Likely uneccessary but ensures empty lists
            for (size_t i = 0; i < MAX_MESSAGE_CODES; i++)
                m_state.registered[i].events.clear();
            m_initialized = true;

        }
        ~EventHandler() {
            // Free all events in the array
            for (size_t i = 0; i < MAX_MESSAGE_CODES; i++) {
                m_state.registered[i].events.clear();
            }

            m_initialized = false;
        }

        // Registers to listen to events that are sent with the specified code
        // listener/callback combos won't be registered twice, and will return 'false' if that is attempted
        bool Register(
                uint16_t code,
                void* listener,
                CallbackFunc callback
        ) {
            // Check if the listener has already been registered
            size_t registered_count = m_state.registered[code].events.size();
            for (size_t i = 0; i < registered_count; i++) {
                if (m_state.registered[code].events[i].listener == listener)
                    return false;
            }

            // Proceed with registration since it has not been registered yet
            RegisteredEvent event = {};
            event.listener = listener;
            event.callback = callback;
            m_state.registered[code].events.push_back(event);
            return true;
        }

        // Unregister an event with the specified code from the listener
        // bool Unregister(uint16_t code, void* listener, PFN_on_event on_event);
        // bool Unregister(uint16_t code, void* listener, bool (T::*)(uint16_t, void*, void*, EventContext));
        bool Unregister(
                uint16_t code,
                void* listener
        ) {
            // Check that the listener is actually registered for the event
            if (m_state.registered[code].events.size() == 0)
                return false;

            // Iterate through the list of listeners registered for the event
            // and check if both the listener and the specified callback function
            // match. If they match, remove the listener from the vector
            // Removing element from vector at an index is obviously not efficient, but for
            // the sake of prototyping this is fine for now. Also there are not too many elements
            // that would be registered for an event, so even in worst case it is not that bad
            size_t registered_count = m_state.registered[code].events.size();
            for (size_t i = 0; i < registered_count; i++) {
                RegisteredEvent ev = m_state.registered[code].events[i];
                if (ev.listener == listener) {
                    m_state.registered[code].events.erase(m_state.registered[code].events.begin() + i);
                    return true;
                }
            }

            return false;
        }

        // Fire an event with the input code
        // If the handler returns true, the event is considered handled
        // If not, the handler passes on to any more listeners
        bool Fire(uint16_t code, void* sender, EventContext context) {
            if (m_state.registered[code].events.size() == 0)
                return false;

            for (size_t i = 0; i < m_state.registered[code].events.size(); i++) {
                RegisteredEvent ev = m_state.registered[code].events[i];
                auto func = ev.callback;
                if (ev.callback(code, sender, ev.listener, context)) {
                    // message was handled if callback returned true
                    return true;
                }
            }

            return false;
        }

        // Accessors
        bool GetInitialized() const { return m_initialized; }
    private:
        bool m_initialized = false;
        EventState m_state;

};
