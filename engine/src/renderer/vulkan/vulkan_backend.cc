/**
 * This is the implementation for a Vulkan rendering backend
*/

#include "vulkan_backend.hh"
#include "vulkan_types.inl"

// Initialize the backend for the vulkan renderer
bool
VulkanBackend::Initialize(std::string& name) {
    std::cout << "Vulkan Backend Initialized" << std::endl;
    m_vkallocator = nullptr; // TODO: eventually create custom allocator

    // TODO: Setup the vulkan instance...

    return true;
}

void
VulkanBackend::Shutdown() {

}

void
VulkanBackend::Resized(uint32_t width, uint32_t height) {

}

bool
VulkanBackend::BeginFrame(float delta_time) {
    return true;
}

bool 
VulkanBackend::EndFrame(float delta_time) {
    return true;
}