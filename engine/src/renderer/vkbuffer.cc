#include "vkbuffer.hh"
#include "renderer.hh"
#include <vulkan/vulkan_core.h>

// STATIC
// Returns the minimum instance size required to be compatible with the device's 
// minimum offset allignment
VkDeviceSize
VKBuffer::GetAllignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAllignment) {
    if (minOffsetAllignment > 0) {
        return (instanceSize + minOffsetAllignment-1)
               & ~(minOffsetAllignment-1);
    }

    return instanceSize;
}

// Static function to create a buffer
void 
VKBuffer::CreateBuffer(const VKCommonParameters &params, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags props, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateBuffer(params.Device.Device, &bufferInfo, params.Allocator, &buffer));

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(params.Device.Device, buffer, &memReqs);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = static_cast<uint32_t>(memReqs.size);
    allocInfo.memoryTypeIndex = Renderer::GetMemoryTypeIndex(memReqs.memoryTypeBits, props, params.Device.DeviceMemoryProperties);

    VK_CHECK(vkAllocateMemory(params.Device.Device, &allocInfo, params.Allocator, &bufferMemory));
}

// Copy one buffer to another
void 
CopyBuffer(const VKCommonParameters& params, VkBuffer dst, VkDeviceSize size) {
    // VkCommandBuffer commandBuffer = Renderer::BeginSingleTimeCommands();
}


// Constructor
VKBuffer::VKBuffer(
    VKCommonParameters& params,
    VkDeviceSize instanceSize,
    uint32_t instanceCount,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags memoryProps,
    VkDeviceSize minOffsetAllignment
) : m_vkparams{params},
    m_instanceSize{instanceSize},
    m_instanceCount{instanceCount},
    m_usage{usage},
    m_memProps{memoryProps} {
   
    m_allignmentSize = VKBuffer::GetAllignment(instanceSize, minOffsetAllignment);
    m_bufferSize = m_allignmentSize * instanceCount;
    VKBuffer::CreateBuffer(m_vkparams, m_bufferSize, m_usage, m_memProps, m_buffer, m_memory);
}

//
// PUBLIC
//

// Destroy buffer objects
void
VKBuffer::Destroy() {
    Unmap();
    vkDestroyBuffer(m_vkparams.Device.Device, m_buffer, m_vkparams.Allocator);
    vkFreeMemory(m_vkparams.Device.Device, m_memory, m_vkparams.Allocator);
}

// Map a range of memory to this buffer. If successful then m_mapped 
// points to the specified buffer range
VkResult
VKBuffer::Map(VkDeviceSize size, VkDeviceSize offset) {
    return vkMapMemory(m_vkparams.Device.Device, m_memory, offset, size, 0, &m_mapped);
}   

// Unmap a mapped memory range
// Does not return a VkResult as vkUnmapMemory cannot fail
void
VKBuffer::Unmap() {
    if (m_mapped) {
        vkUnmapMemory(m_vkparams.Device.Device, m_memory);
        m_mapped = nullptr;
    }
}

// Copies the specified data to the mapped buffer
// Default value writes the whole buffer range
void
VKBuffer::WriteToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset) {
    if (size == VK_WHOLE_SIZE) {
        memcpy(m_mapped, data, m_bufferSize);
    } else {
        char* memOffset = static_cast<char*>(m_mapped);
        memOffset += offset;
        memcpy(memOffset, data, size);
    }
}

//TODO: FLUSH

// Create a buffer descriptor info
VkDescriptorBufferInfo
VKBuffer::BufferDescriptor(VkDeviceSize size, VkDeviceSize offset) {
    return VkDescriptorBufferInfo {
        m_buffer,
        offset,
        size
    };
}

// Copies the "m_instanceSize" of data to the mapped buffer at an offset 
// of m_allignmentSize * index
void
VKBuffer::WriteToIndex(void* data, int index) {
    WriteToBuffer(data, m_instanceSize, index * m_allignmentSize);
}

// TODO: flush index

VkDescriptorBufferInfo
VKBuffer::IndexDescriptor(int index) {
    return BufferDescriptor(m_allignmentSize, index * m_allignmentSize);
}

// TODO: invalidate index
