/**
 * This is the implementation for a Vulkan rendering backend
*/

#include "vulkan_backend.hh"
#include "vulkan_types.hh"
#include "core/application.hh"
#include "vk_device.hh"
#include "qmath/qmath.hh"
#include "core/qlogger.hh"
#include "core/qmemory.hh"

static uint32_t cached_framebuffer_width = 0;
static uint32_t cached_framebuffer_height = 0;

void
upload_data_range(VKContext& context, VkCommandPool pool, VkFence fence, VkQueue queue, VKBuffer& buffer, uint64_t offset, uint64_t size, void* data) {
    VkBufferUsageFlags flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    VKBuffer staging;
    staging.Create(
        context,
        size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        flags,
        true
    );

    staging.LoadData(context, 0, size, 0, data);
    staging.CopyTo(context, pool, fence, queue, 0, buffer.handle, offset, size);
    staging.Destroy(context);
}

// Initialize the backend for the vulkan renderer
bool
VulkanBackend::Initialize(std::string& name) {
 
    qlogger::Info("Vulkan Backend Initialized");
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
        qlogger::Info("Error: failed to create vulkan surface");
        return false;
    }

    if (!create_device()) {
        qlogger::Info("Error: Failed to create vulkan device");
        return false;
    }

    if (!create_swapchain(
        m_context.framebuffer_width,
        m_context.framebuffer_height,
        m_context.swapchain
    )) {
        qlogger::Info("Error: Failed to create swapchain...");
    }

    if (!create_renderpass(
        m_context.main_renderpass,
        0.0f, 0.0f, m_context.framebuffer_width, m_context.framebuffer_height,
        0.0f, 0.0f, 0.2f, 1.0f,
        1.0f,
        0
    )) {
        qlogger::Info("Error: failed to create main renderpass.");
        return false;
    }

    // Create swapchain framebuffers
    m_context.swapchain.framebuffers.resize(m_context.swapchain.image_count);
    regenerate_framebuffers(m_context.swapchain, m_context.main_renderpass);

    create_command_buffers();

    // Create synchronization objects
    qlogger::Info("Creating synch objects...");
    m_context.image_available_semaphores.resize(m_context.swapchain.max_frames_in_flight);
    m_context.queue_complete_semaphores.resize(m_context.swapchain.max_frames_in_flight);
    m_context.in_flight_fences.resize(m_context.swapchain.max_frames_in_flight);

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
        // m_context.images_in_flight[i] = VK_NULL_HANDLE;
        m_context.images_in_flight[i] = 0;
    }

    if (!m_context.object_shader.Create(m_context)) {
        qlogger::Error("Unable to load built-in object shader");
        return false;
    }

    create_buffers();

    // TEMP TEST GEOMETRY
    constexpr uint32_t vert_count = 4;
    qmath::Vertex3D vertices[vert_count];
    QAllocator::Zero(vertices, sizeof(qmath::Vertex3D) * vert_count); 

    vertices[0].position.x = 0.0;
    vertices[0].position.y = -0.5;
    // vertices[0].position.z = 1.0;
    
    vertices[1].position.x = 0.5;
    vertices[1].position.y = 0.5;
    // vertices[1].position.z = 1.0;
    
    vertices[2].position.x = 0.0;
    vertices[2].position.y = 0.5;
    // vertices[2].position.z = 1.0;
    
    vertices[3].position.x = 0.5;
    vertices[3].position.y = -0.5;

    constexpr uint32_t index_count = 6;
    uint32_t indices[index_count] = {0, 1, 2, 0, 3, 1};

    upload_data_range(
        m_context,
        m_context.device.graphics_command_pool,
        nullptr,
        m_context.device.graphics_queue,
        m_context.object_vertex_buffer,
        0,
        sizeof(qmath::Vertex3D) * vert_count,
        vertices
    );

    upload_data_range(
        m_context,
        m_context.device.graphics_command_pool,
        nullptr,
        m_context.device.graphics_queue,
        m_context.object_index_buffer,
        0,
        sizeof(uint32_t) * index_count,
        indices
    );
    // END TEST CODE

    qlogger::Info("Vulkan Backend initialized successfully.");
    return true;
}

void
VulkanBackend::Shutdown() {
    vkDeviceWaitIdle(m_context.device.logical_device);

    destroy_buffers();

    // Destroy builtin shader modules
    m_context.object_shader.Destroy(m_context);

    // Sync objects
    qlogger::Info("Destroying sync objects... ");
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
    qlogger::Info("Destroyed.");

    // Command Buffers
    qlogger::Info("Freeing command buffers... ");
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
    qlogger::Info("Freed.");

    // Framebuffers
    qlogger::Info("Destroying framebuffers... ");
    for (uint32_t i = 0; i < m_context.swapchain.image_count; i++) {
        destroy_framebuffer(m_context.swapchain.framebuffers[i]);
    }
    qlogger::Info("Destroyed ");

    destroy_renderpass(m_context.main_renderpass);

    destroy_swapchain();

    destroy_device();

    qlogger::Info("Destroying surface... ");
    vkDestroySurfaceKHR(m_context.instance, m_context.surface, m_context.allocator);
    qlogger::Info("Destroyed ");

#if defined(P_DEBUG)
    qlogger::Info("Destroying debug messenger... ");
    PFN_vkDestroyDebugUtilsMessengerEXT func = 
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_context.instance, "vkDestroyDebugUtilsMessengerEXT");
    func(m_context.instance, m_context.debug_messenger, m_context.allocator);
    qlogger::Info("Destroyed");
#endif // P_DEBUG

    qlogger::Info("Destroying instance... ");
    vkDestroyInstance(m_context.instance, m_context.allocator);
    qlogger::Info("Destroyed");
}

void
VulkanBackend::Resized(uint32_t width, uint32_t height) {
    cached_framebuffer_width = width;
    cached_framebuffer_height = height;
    m_context.framebuffer_size_generation++;

    qlogger::Trace("VulkanBackend->resized: w/h/gen: %i/%i/%i", width, height, m_context.framebuffer_size_generation);
}

bool
VulkanBackend::BeginFrame(float delta_time) {
    VKDevice& device = m_context.device;

    if (m_context.recreating_swapchain) {
        VkResult result = vkDeviceWaitIdle(device.logical_device);
        if (!vkresult_is_success(result)) {
            qlogger::Info("VulkanBackend::BeginFrame() vkDeviceWaitIdle(1) failed: %s", vkresult_string(result, true));
            return false;
        }
        qlogger::Info("Recreating swapchain. Booting...");
    }

    // Check if the framebuffer has been resized to 
    // see if we need to recreate the swapchain
    if (m_context.framebuffer_size_generation != m_context.framebuffer_size_last_generation) {
        VkResult result = vkDeviceWaitIdle(device.logical_device);
        if (!vkresult_is_success(result)) {
            qlogger::Info("VulkanBackend::BeginFrame() vkDeviceWaitIdle(2) failed: %s", vkresult_string(result, true));
            return false;
        }

        if (!recreate_swapchain()) {
            return false;
        }

        qlogger::Info("Resized. Booting...");
        return false;
    }

    // Wait for the execution of teh current frame to complete
    // This will move on once the fence is free
    if(!m_context.in_flight_fences[m_context.current_frame].wait(m_context, UINT64_MAX)) {
        qlogger::Info("Warning: In-Flight fence wait failure");
        return false;
    }

    // Acquire the swapchain next image. Pass along the semaphore that shuold be signaled when this completes
    // This same semaphore will be waited on by the queue submission to ensure this image will be available
    if (!m_context.swapchain.acquire_next_image_index(
        m_context, 
        UINT64_MAX, 
        m_context.image_available_semaphores[m_context.current_frame], 
        0, 
        m_context.image_index
    )) {
        return false;
    }

    // Begin recording the command buffers
    VKCommandBuffer& command_buffer = m_context.graphics_command_buffers[m_context.image_index];
    command_buffer.reset();
    command_buffer.begin(false, false, false);

    // Dynamic states
    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = static_cast<float>(m_context.framebuffer_height);
    viewport.width = static_cast<float>(m_context.framebuffer_width);
    viewport.height = -static_cast<float>(m_context.framebuffer_height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor {};
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width = m_context.framebuffer_width;
    scissor.extent.height = m_context.framebuffer_height;

    vkCmdSetViewport(
        command_buffer.handle,
        0,
        1,
        &viewport
    );
    vkCmdSetScissor(
        command_buffer.handle,
        0,
        1,
        &scissor
    );

    m_context.main_renderpass.begin(
        command_buffer,
        m_context.swapchain.framebuffers[m_context.image_index]
    );

    // TODO: TEMP TEST CODE
    m_context.object_shader.Use(m_context);
    VkDeviceSize offsets[1] = {0};
    vkCmdBindVertexBuffers(command_buffer.handle, 0, 1, &m_context.object_vertex_buffer.handle, static_cast<VkDeviceSize*>(offsets));
    vkCmdBindIndexBuffer(command_buffer.handle, m_context.object_index_buffer.handle, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(command_buffer.handle, 6, 1, 0, 0, 0);
    // TODO: END TEST CODE

    return true;
}

bool 
VulkanBackend::EndFrame(float delta_time) {
    VKCommandBuffer& command_buffer = m_context.graphics_command_buffers[m_context.image_index];
    m_context.main_renderpass.end(
        command_buffer
    );

    command_buffer.end();

    // Make sure that the previous frame is not using this image
    if (m_context.images_in_flight[m_context.image_index] != VK_NULL_HANDLE) {
        m_context.images_in_flight[m_context.image_index]->wait(m_context, UINT64_MAX);
    }

    // Mark the image fence as in use by this frame
    m_context.images_in_flight[m_context.image_index] = &m_context.in_flight_fences[m_context.current_frame];

    // Reset the image fence for use by hte next frame
    m_context.in_flight_fences[m_context.current_frame].reset(m_context);

    // Begin queue submission
    VkSubmitInfo submit_info {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer.handle;
    submit_info.signalSemaphoreCount =1;
    submit_info.pSignalSemaphores = &m_context.queue_complete_semaphores[m_context.current_frame];
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &m_context.image_available_semaphores[m_context.current_frame];
    
    VkPipelineStageFlags flags[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.pWaitDstStageMask = flags;

    // submit the commands to be executed
    VkResult result = vkQueueSubmit(
        m_context.device.graphics_queue,
        1,
        &submit_info,
        m_context.in_flight_fences[m_context.current_frame].handle
    );

    if (result != VK_SUCCESS) {
        qlogger::Info("Error: vkQueueSubmit failed with result: %s", vkresult_string(result, true));
        return false;
    }

    command_buffer.update_submitted();
    // End queue submission

    // Presentation
    m_context.swapchain.present(
        m_context,
        m_context.device.graphics_queue,
        m_context.device.present_queue,
        m_context.queue_complete_semaphores[m_context.current_frame],
        m_context.image_index
    );

    return true;
}

// Recreate the swapchain for things like resizing the window
bool
VulkanBackend::recreate_swapchain() {
    if (m_context.recreating_swapchain) {
        qlogger::Info("VulkanBackend::recreate_swapchain() called when already recreating swapchain. Booting...");
        return false;
    }

    if (m_context.framebuffer_height == 0 || m_context.framebuffer_width == 0) {
        qlogger::Info("VulkanBackend::recreate_swapchain() called when windows < 1 in a dimension. Booting...");
        return false;
    }

    // Mark as recreating
    m_context.recreating_swapchain = true;

    // Wait for any operations to complete
    vkDeviceWaitIdle(m_context.device.logical_device);

    // Clear in case
    for (uint32_t i = 0; i < m_context.swapchain.image_count; i++) {
        m_context.images_in_flight[i] = nullptr;
    }

    // Requery support
    vkdevice_query_swapchain_support(
        m_context.device,
        m_context.surface,
        m_context.device.swapchain_support
    );
    vkdevice_detect_depth_format(m_context.device);

    // Recreate the swapchain
    m_context.swapchain.recreate(
        m_context, 
        cached_framebuffer_width, 
        cached_framebuffer_height
    );

    m_context.framebuffer_height = cached_framebuffer_height;
    m_context.framebuffer_width = cached_framebuffer_width;
    m_context.main_renderpass.w = m_context.framebuffer_width;
    m_context.main_renderpass.h = m_context.framebuffer_height;
    cached_framebuffer_height = 0;
    cached_framebuffer_width = 0;

    // Update the framebuffer size generation
    m_context.framebuffer_size_last_generation = m_context.framebuffer_size_generation;

    // Cleanup the swapchain
    for (uint32_t i = 0; i < m_context.swapchain.image_count; i++) {
        m_context.graphics_command_buffers[i].free(m_context, m_context.device.graphics_command_pool);
    }

    for (uint32_t i = 0; i < m_context.swapchain.image_count; i++) {
        destroy_framebuffer(m_context.swapchain.framebuffers[i]);
    }

    m_context.main_renderpass.x = 0;
    m_context.main_renderpass.y = 0;
    m_context.main_renderpass.h = m_context.framebuffer_height;
    m_context.main_renderpass.w = m_context.framebuffer_width;

    regenerate_framebuffers(m_context.swapchain, m_context.main_renderpass);
    create_command_buffers();

    // Clear the recreating flag
    m_context.recreating_swapchain = false;
    return true;
}

// Create the index and vertex buffers
bool
VulkanBackend::create_buffers() {
    VkMemoryPropertyFlagBits memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    const uint64_t vertex_buffer_size = sizeof(qmath::Vertex3D) * 1024 * 1024;
    if (!m_context.object_vertex_buffer.Create(
        m_context,
        vertex_buffer_size,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        memory_property_flags,
        true
    )) {
        qlogger::Error("Failed to create object vertex buffer");
        return false;
    }

    m_context.geometry_vertex_offset = 0;

    const uint64_t index_buffer_size = sizeof(uint32_t) * 1024 * 1024;
    if (!m_context.object_index_buffer.Create(
        m_context,
        index_buffer_size,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        memory_property_flags,
        true
    )) {
        qlogger::Error("Failed to create object index buffer");
        return false;
    }
    m_context.geometry_index_offset = 0;

    return true;
}

void
VulkanBackend::destroy_buffers() {
    m_context.object_index_buffer.Destroy(m_context);
    m_context.object_vertex_buffer.Destroy(m_context);
}