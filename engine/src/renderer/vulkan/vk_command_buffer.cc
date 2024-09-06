#include "vulkan_backend.hh"
#include "vk_command_buffer.hh"
#include "core/qlogger.hh"

void
VulkanBackend::create_command_buffers() {
    // Create a command buffer for each swapchain image
    // While one image is presenting, we can draw to the 
    // other images in the swapchain
    // We have to use different command buffers for each 
    // image in the swapchain
    if (m_context.graphics_command_buffers.empty()) {
        m_context.graphics_command_buffers.resize(m_context.swapchain.image_count);
        for (uint32_t i = 0; i < m_context.swapchain.image_count; i++) {
            // m_context.graphics_command_buffers[i] = {};
        }
    }

    for (uint32_t i = 0; i < m_context.swapchain.image_count; i++) {
        if (m_context.graphics_command_buffers[i].handle) {
            m_context.graphics_command_buffers[i].free(m_context, m_context.device.graphics_command_pool);
        }

        // m_context.graphics_command_buffers[i] = {};
        m_context.graphics_command_buffers[i].allocate(
            m_context,
            m_context.device.graphics_command_pool,
            true
        );
    }

    qlogger::Info("Graphics command buffers created [%llu]", m_context.graphics_command_buffers.size());
}

void 
VKCommandBuffer::allocate(VKContext& context, VkCommandPool pool, bool is_primary) {
    VkCommandBufferAllocateInfo allocate_info {};
    allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocate_info.commandPool = pool;
    allocate_info.level = (is_primary) ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    allocate_info.commandBufferCount = 1;
    allocate_info.pNext = nullptr;

    this->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
    VK_CHECK(vkAllocateCommandBuffers(
        context.device.logical_device,
        &allocate_info,
        &this->handle
    ));
}

void 
VKCommandBuffer::free(VKContext& context, VkCommandPool pool) {
    vkFreeCommandBuffers(
        context.device.logical_device,
        pool,
        1,
        &this->handle
    );

    this->handle = nullptr;
    this->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
}

void 
VKCommandBuffer::begin(bool is_single_use, bool is_renderpass_continue, bool is_simultaneous_use) {
    VkCommandBufferBeginInfo begin_info  = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.pNext = nullptr;
    begin_info.flags = 0;

    if (is_single_use) {
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    }

    if (is_renderpass_continue) {
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    }

    if (is_simultaneous_use) {
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    }

    VK_CHECK(vkBeginCommandBuffer(
        this->handle,
        &begin_info
    ));

    this->state = COMMAND_BUFFER_STATE_RECORDING;
}

void 
VKCommandBuffer::end() {
    VK_CHECK(vkEndCommandBuffer(
        this->handle
    ));

    this->state = COMMAND_BUFFER_STATE_RECORDING_ENDED;
    // TODO: this should check to see if we are in a state where
    //       ending is valid, but just allow the appliation to crash for now
}

void 
VKCommandBuffer::update_submitted() {
    this->state = COMMAND_BUFFER_STATE_SUBMITTED;
}

void 
VKCommandBuffer::reset() {
    this->state = COMMAND_BUFFER_STATE_READY;
}

void 
VKCommandBuffer::allocate_and_begin_single_use(
    VKContext& context,
    VkCommandPool pool
) {
    this->allocate(context, pool, true);
    this->begin(true, false, false);
}

void VKCommandBuffer::end_single_use(
    VKContext& context,
    VkCommandPool pool,
    VkQueue queue
) {
    this->end();

    VkSubmitInfo submit_info {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &this->handle;

    VK_CHECK(vkQueueSubmit(
        queue,
        1,
        &submit_info,
        nullptr
    ));

    VK_CHECK(vkQueueWaitIdle(queue));

    this->free(context, pool);
}