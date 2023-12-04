/**
 * This is the implementation for a Vulkan rendering backend
*/

#include "vulkan_backend.hh"
#include "vulkan_types.hh"
#include "core/application.hh"

static uint32_t cached_framebuffer_width = 0;
static uint32_t cached_framebuffer_height = 0;

// Initialize the backend for the vulkan renderer
bool
VulkanBackend::Initialize(std::string& name) {
    std::cout << "Vulkan Backend Initialized" << std::endl;
    m_context.allocator = nullptr; // TODO: eventually create custom allocator
    Application::GetFramebufferSize(
        cached_framebuffer_width,
        cached_framebuffer_height
    );

    m_context.framebuffer_width = (cached_framebuffer_width != 0) ? cached_framebuffer_width : 800;
    m_context.framebuffer_height = (cached_framebuffer_height != 0) ? cached_framebuffer_height : 600;
    m_context.framebuffer_size_generation = 0;
    m_context.framebuffer_size_last_generation = 0;
    cached_framebuffer_width = 0;
    cached_framebuffer_height = 0;


    // Initialize all vulkan properties
    if (!create_instance(name.c_str())) {
        return false;
    }

    create_debug_messenger();

    if (!create_surface()) {
        std::cout << "Error: failed to create vulkan surface" << std::endl;
        return false;
    }

    if (!create_device()) {
        std::cout << "Error: Failed to create vulkan device" << std::endl;
        return false;
    }

    if (!create_swapchain(
        m_context.framebuffer_width,
        m_context.framebuffer_height,
        m_context.swapchain
    )) {
        std::cout << "Error: Failed to create swapchain..." << std::endl;
    }

    if (!create_renderpass(
        m_context.main_renderpass,
        0, 0, m_context.framebuffer_width, m_context.framebuffer_height,
        0.0f, 0.0f, 0.2f, 1.0f,
        1.0f,
        0
    )) {
        std::cout << "Error: failed to create main renderpass." << std::endl;
        return false;
    }

    // Create swapchain framebuffers
    m_context.swapchain.framebuffers.resize(m_context.swapchain.image_count);
    regenerate_framebuffers(m_context.swapchain, m_context.main_renderpass);

    create_command_buffers();

    // Create synchronization objects
    std::cout << "Creating synch objects..." << std::endl;
    m_context.image_available_semaphores.reserve(m_context.swapchain.max_frames_in_flight);
    m_context.queue_complete_semaphores.reserve(m_context.swapchain.max_frames_in_flight);
    m_context.in_flight_fences.reserve(m_context.swapchain.max_frames_in_flight);

    for (uint32_t i = 0; i < m_context.swapchain.max_frames_in_flight; i++) {
        VkSemaphoreCreateInfo sem_info {};
        sem_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        vkCreateSemaphore(m_context.device.logical_device, &sem_info, m_context.allocator, &m_context.image_available_semaphores[i]);
        vkCreateSemaphore(m_context.device.logical_device, &sem_info, m_context.allocator, &m_context.queue_complete_semaphores[i]);

        // Create this fence in a simplified state indicating that the first frame
        // has already been 'rendrered'. This prevents the application from waiting
        // indefinitely for the first frame to rendersince it cannot be rendered
        // until a frame is "rendered before it"
        m_context.in_flight_fences[i].create(m_context, true);
    }

    m_context.images_in_flight.resize(m_context.swapchain.image_count);
    for (uint32_t i = 0; i < m_context.swapchain.image_count; i++) {
        m_context.images_in_flight[i] = nullptr;
    }

    std::cout << "Vulkan Backend initialized successfully." << std::endl; 
    return true;
}

void
VulkanBackend::Shutdown() {
    vkDeviceWaitIdle(m_context.device.logical_device);

    // Sync objects
    std::cout << "Destroying sync objects... ";
    for (uint32_t i = 0; i < m_context.swapchain.max_frames_in_flight; i++) {
        if (m_context.image_available_semaphores[i]) {
            vkDestroySemaphore(
                m_context.device.logical_device,
                m_context.image_available_semaphores[i],
                m_context.allocator
            );
        }
        if (m_context.queue_complete_semaphores[i]) {
            vkDestroySemaphore(
                m_context.device.logical_device,
                m_context.queue_complete_semaphores[i],
                m_context.allocator
            );
        }
        m_context.in_flight_fences[i].destroy(m_context);
    }
    m_context.image_available_semaphores.clear();
    m_context.queue_complete_semaphores.clear();
    m_context.in_flight_fences.clear();
    m_context.images_in_flight.clear(); // we do not destroy these fences because we do not own them
    std::cout << "Destroyed." << std::endl;

    // Command Buffers
    std::cout << "Freeing command buffers... ";
    for (uint32_t i = 0; i < m_context.swapchain.image_count; i++) {
        if (m_context.graphics_command_buffers[i].handle) {
            m_context.graphics_command_buffers[i].free(
                m_context,
                m_context.device.graphics_command_pool
            );
            m_context.graphics_command_buffers[i].handle = nullptr;
        }
    }
    m_context.graphics_command_buffers.clear();
    std::cout << "Freed." << std::endl;

    // Framebuffers
    std::cout << "Destroying framebuffers... ";
    for (uint32_t i = 0; i < m_context.swapchain.image_count; i++) {
        destroy_framebuffer(m_context.swapchain.framebuffers[i]);
    }
    std::cout << "Destroyed " << std::endl;

    destroy_renderpass(m_context.main_renderpass);

    destroy_swapchain();

    destroy_device();

    std::cout << "Destroying surface... ";
    vkDestroySurfaceKHR(m_context.instance, m_context.surface, m_context.allocator);
    std::cout << "Destroyed " << std::endl;

#if defined(P_DEBUG)
    std::cout << "Destroying debug messenger... ";
    PFN_vkDestroyDebugUtilsMessengerEXT func = 
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_context.instance, "vkDestroyDebugUtilsMessengerEXT");
    func(m_context.instance, m_context.debug_messenger, m_context.allocator);
    std::cout << "Destroyed" << std::endl;
#endif // P_DEBUG

    std::cout << "Destroying instance... ";
    vkDestroyInstance(m_context.instance, m_context.allocator);
    std::cout << "Destroyed" << std::endl;
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