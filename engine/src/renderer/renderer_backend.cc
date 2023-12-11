#include "renderer_backend.hh"
//TODO: add header guards around each file
#include "vulkan/vulkan_backend.hh"
#include "core/qlogger.hh"

// Create the backend for the renderer based on the input type
// TODO: eventually this will support multiple backend types, but
//             for now we are only supporting Vulkan
bool 
renderer_backend_create(renderer_backend_type type, RendererBackend** backend) {
    switch (type) {
        case RENDERER_BACKEND_VULKAN: {
            *backend = new VulkanBackend();
            qlogger::Debug("Vulkan Backend Selected");
            return true;
        } break;
        case RENDERER_BACKEND_OPENGL:
        case RENDERER_BACKEND_DIRECTX:
        case RENDERER_BACKEND_METAL:
            qlogger::Error("Invalid backend type");
            return false;
    }
    return true;
}

void
renderer_backend_destroy(RendererBackend* backend) {

}