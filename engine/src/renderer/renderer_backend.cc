#include "renderer_backend.hh"
//TODO: add header guards around each file
#include "vulkan/vulkan_backend.hh"

// Create the backend for the renderer based on the input type
// TODO: eventually this will support multiple backend types, but
//             for now we are only supporting Vulkan
bool 
renderer_backend_create(renderer_backend_type type, RendererBackend** backend) {
    switch (type) {
        case RENDERER_BACKEND_VULKAN: {
            *backend = new VulkanBackend();
            // backend = std::make_unique<VulkanBackend>();
            printf("vk backend\n");
            return true;
        } break;
        case RENDERER_BACKEND_OPENGL:
        case RENDERER_BACKEND_DIRECTX:
        case RENDERER_BACKEND_METAL:
            std::cout << "ERRORR: Invalid backend type" << std::endl;
            return false;
    }
    // *backend = new VulkanBackend();
    return true;
}

void
renderer_backend_destroy(RendererBackend* backend) {

}