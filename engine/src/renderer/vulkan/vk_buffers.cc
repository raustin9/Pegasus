#include "vulkan_backend.hh"
#include "vk_device.hh"
#include "vk_command_buffer.hh"
#include "vulkan_utils.hh"
#include "core/qlogger.hh"
#include "core/qmemory.hh"
#include "qmath/qmath.hh"


bool 
VKBuffer::Create(
    VKContext& context,
    uint64_t size,
    VkBufferUsageFlags usage,
    uint32_t memory_property_flags,
    bool bind_on_create
) {
    this->total_size = size;
    this->usage = usage;
    this->memory_property_flags = memory_property_flags;

    VkBufferCreateInfo buffer_info {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateBuffer(
        context.device.logical_device,
        &buffer_info,
        context.allocator,
        &this->handle
    ));

    // Memory Requirements
    VkMemoryRequirements requirements {};
    vkGetBufferMemoryRequirements(context.device.logical_device, this->handle, &requirements);
    this->memory_index = context.find_memory_index(requirements.memoryTypeBits, this->memory_property_flags);
    if (this->memory_index == -1) {
        qlogger::Error("Unable to create vulkan buffer because the required memory type index was not found");
        return false;
    }

    VkMemoryAllocateInfo allocate_info {};
    allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocate_info.allocationSize = requirements.size;
    allocate_info.memoryTypeIndex = static_cast<uint32_t>(this->memory_index);

    // Allocate memory
    VkResult result = vkAllocateMemory(
        context.device.logical_device,
        &allocate_info,
        context.allocator,
        &this->memory
    );
    if (!vkresult_is_success(result)) {
        qlogger::Error("Failed to create buffer: vkAllocateMemory failed with %s", vkresult_string(result, true));
        return false;
    }

    if (bind_on_create) {
        this->Bind(context, 0);
    }

    return true;
}
void 
VKBuffer::Destroy(VKContext& context) {
    if (this->memory) {
        vkFreeMemory(context.device.logical_device, this->memory, context.allocator);
        this->memory = nullptr;
    }
    if (this->handle) {
        vkDestroyBuffer(context.device.logical_device, this->handle, context.allocator);
        this->handle = nullptr;
    }
    
    this->total_size = 0;
    this->usage = 0;
    this->is_locked = false;
}

bool 
VKBuffer::Resize(
    VKContext& context,
    uint64_t new_size,
    VkQueue queue,
    VkCommandPool pool
) {
    // Create new buffer
    VkBufferCreateInfo buffer_info {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = new_size;
    buffer_info.usage = this->usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer new_buffer {};
    VK_CHECK(vkCreateBuffer(
        context.device.logical_device,
        &buffer_info,
        context.allocator,
        &new_buffer
    ));

    // Memory Requirements
    VkMemoryRequirements requirements {};
    vkGetBufferMemoryRequirements(context.device.logical_device, new_buffer, &requirements);

    VkMemoryAllocateInfo allocate_info {};
    allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocate_info.allocationSize = requirements.size;
    allocate_info.memoryTypeIndex = static_cast<uint32_t>(this->memory_index);

    // Allocate memory
    VkDeviceMemory new_memory {};
    VkResult result = vkAllocateMemory(
        context.device.logical_device,
        &allocate_info,
        context.allocator,
        &new_memory
    );

    if (!vkresult_is_success(result)) {
        qlogger::Error("Failed to create buffer: vkAllocateMemory failed with %s", vkresult_string(result, true));
        return false;
    }

    VK_CHECK(vkBindBufferMemory(
        context.device.logical_device,
        new_buffer,
        new_memory,
        0
    ));

    // Copy over the data
    this->CopyTo(
        context,
        pool,
        nullptr,
        queue,
        0,
        new_buffer,
        0,
        this->total_size
    );
    vkDeviceWaitIdle(context.device.logical_device);
    if (this->memory) {
        vkFreeMemory(context.device.logical_device, this->memory, context.allocator);
        this->memory = nullptr;
    }
    if (this->handle) {
        vkDestroyBuffer(context.device.logical_device, this->handle, context.allocator);
        this->handle = nullptr;
    }

    // Set the new properties
    this->total_size = new_size;
    this->memory = new_memory;
    this->handle = new_buffer;

    return true;
}

void 
VKBuffer::Bind(VKContext& context, uint64_t offset) {
    VK_CHECK(vkBindBufferMemory(
        context.device.logical_device, this->handle, this->memory, offset
    ));
}

void* 
VKBuffer::LockMemory(VKContext& context, uint64_t offset, uint64_t size, uint32_t flags) {
    void* data;
    VK_CHECK(vkMapMemory(
        context.device.logical_device, 
        this->memory,
        offset,
        size,
        flags,
        &data
    ));
    return data;
}

void 
VKBuffer::UnlockMemory(VKContext& context) {
    vkUnmapMemory(
        context.device.logical_device,
        this->memory
    );
}

void 
VKBuffer::LoadData(VKContext& context, uint64_t offset, uint64_t size, uint32_t flags, const void* data) {
    void* data_ptr;
    VK_CHECK(vkMapMemory(context.device.logical_device, this->memory, offset, size, flags, &data_ptr));
    QAllocator::Copy(data_ptr, data, size);
    vkUnmapMemory(context.device.logical_device, this->memory);
}

void 
VKBuffer::CopyTo(
    VKContext& context,
    VkCommandPool pool,
    VkFence fence,
    VkQueue queue,
    uint64_t source_offset,
    VkBuffer dest,
    uint64_t dest_offset,
    uint64_t size
) {
    vkQueueWaitIdle(queue);

    // Create one-time-use command buffer
    VKCommandBuffer temp_cmd_buffer;
    temp_cmd_buffer.allocate_and_begin_single_use(context, pool);

    // Prepare the copy command and add it to the command buffer
    VkBufferCopy copy_region;
    copy_region.srcOffset = source_offset;
    copy_region.dstOffset = dest_offset;
    copy_region.size = size;

    vkCmdCopyBuffer(temp_cmd_buffer.handle, this->handle, dest, 1, &copy_region);

    temp_cmd_buffer.end_single_use(context, pool, queue);
}