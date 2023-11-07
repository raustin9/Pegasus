#include "vkmodel.hh"
#include "renderer.hh"

void
VKModel::_create_index_buffers(const std::vector<uint32_t> &indices) {
    m_indexCount = static_cast<uint32_t>(indices.size());
    m_hasIndexBuffer = m_indexCount > 0; 
}

// Destroy and free the vertex buffer and its bound memory
void
VKModel::Destroy() {
    vkDestroyBuffer(m_vkparams.Device.Device, m_vertexBuffer, m_vkparams.Allocator);
    vkFreeMemory(m_vkparams.Device.Device, m_vertexBufferMemory, m_vkparams.Allocator);
}

void
VKModel::_create_vertex_buffers(const std::vector<Vertex> &vertices) {
    // m_vertexCount = static_cast<uint32_t>(vertices.size());
    m_vertexCount = 3;
    // m_hasVertexBuffer = m_vertexCount > 0; 

    VkDeviceSize bufferSize = sizeof(vertices[0]) * m_vertexCount;
    
    // Create the vertex buffer in host-visible device memory
    // This is not good because it will lower rendering performance

    // Used to request an allocation of a specific size from a certain memory type
    VkMemoryAllocateInfo memAlloc = {};
    memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    VkMemoryRequirements memReqs;

    // Pointer to map host-visible device memry to the virtual address space of the application
    // The application can copy data to host-visible device memory only using this pointer
    void *data;

    // Create the vertex buffer object
    VkBufferCreateInfo vertexBufferCreateInfo = {};
    vertexBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vertexBufferCreateInfo.size = bufferSize;
    vertexBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    VK_CHECK(
        vkCreateBuffer(m_vkparams.Device.Device, &vertexBufferCreateInfo, m_vkparams.Allocator, &m_vertexBuffer));
    
    // Request a memory allocation from coherent, host-visible device memory that 
    // is large enough to hold the vertex buffer
    // VK_MEMORY_PROPERTY_HOST_COHERENT_BIT makes sure that writes performed
    // by the host (application) will be directly visible to the device without 
    // requiring the explicit flushing of cached memory
    vkGetBufferMemoryRequirements(m_vkparams.Device.Device, m_vertexBuffer, &memReqs);
    memAlloc.allocationSize = static_cast<uint32_t>(memReqs.size);
    memAlloc.memoryTypeIndex = Renderer::GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_vkparams.Device.DeviceMemoryProperties);
    VK_CHECK(
        vkAllocateMemory(m_vkparams.Device.Device, &memAlloc, m_vkparams.Allocator, &m_vertexBufferMemory));

    // Map the host-visible device memory and copy the vertex data
    // Once finished, we can unmap it since we no longer need to access
    // the vertex buffer from the application
    VK_CHECK(
        vkMapMemory(m_vkparams.Device.Device, m_vertexBufferMemory, 0, memAlloc.allocationSize, 0, &data)
    );
    memcpy(data, vertices.data(), bufferSize);
    vkUnmapMemory(m_vkparams.Device.Device, m_vertexBufferMemory);
    
    // Bind the vertex buffer object to the backing host-visible device memory
    // we just allocated
    VK_CHECK(
        vkBindBufferMemory(m_vkparams.Device.Device, m_vertexBuffer, m_vertexBufferMemory, 0)
    );
}

void
VKModel::Draw(VkCommandBuffer cmdBuffer) {
    vkCmdDraw(cmdBuffer, m_vertexCount, 1, 0, 0);
}

void
VKModel::Bind(VkCommandBuffer cmdBuffer) {
    VkBuffer buffers [] = {m_vertexBuffer};
    VkDeviceSize offsets[1] = {0};
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, buffers, offsets);
}

// Vertex Structure IMPL
std::vector <VkVertexInputBindingDescription>
VKModel::Vertex::GetBindingDesc() {
    std::vector <VkVertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions[0].binding = 0;
    bindingDescriptions[0].stride = sizeof(Vertex);
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescriptions;
}

std::vector <VkVertexInputAttributeDescription>
VKModel::Vertex::GetAttribDesc() {
    std::vector <VkVertexInputAttributeDescription> attribDescriptions(2);
    attribDescriptions[0].binding = 0;
    attribDescriptions[0].location = 0; 
    attribDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribDescriptions[0].offset = offsetof(Vertex, position);

    attribDescriptions[1].binding = 0;
    attribDescriptions[1].location = 1; 
    attribDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribDescriptions[1].offset = offsetof(Vertex, color);
    return attribDescriptions;
}


