#include "renderer.hh"
#include "renderer/vkcommon.hh"
#include <vulkan/vulkan_core.h>

void
VKModel::_create_index_buffers(const std::vector<uint32_t> &indices) {
    m_indexCount = static_cast<uint32_t>(indices.size());
    m_hasIndexBuffer = m_indexCount > 0; 

    // Check if we have an index buffer
    if (!m_hasIndexBuffer)
        return;

    VkDeviceSize bufferSize = sizeof(indices[0]) * m_indexCount;
    uint32_t indexSize = sizeof(indices[0]);
    
    VKBuffer stagingBuffer {
        m_vkparams,
        indexSize,
        m_indexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };
    stagingBuffer.Map();
    stagingBuffer.WriteToBuffer((void*)indices.data());

    m_ibuffer = std::make_unique<VKBuffer>(
        m_vkparams,
        indexSize,
        m_indexCount,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    VKBuffer::CopyBuffer(m_vkparams, stagingBuffer.GetBuffer(), m_ibuffer->GetBuffer(), bufferSize);

    // Destroy the staging buffer now that we do not need it
    stagingBuffer.Unmap();
    stagingBuffer.Destroy();
    m_ibuffer->Unmap();
}

// Destroy and free the vertex buffer and its bound memory
void
VKModel::Destroy() {
    m_vbuffer->Destroy();

    if (m_hasIndexBuffer)
        m_ibuffer->Destroy();
}


void
VKModel::_create_vertex_buffers(const std::vector<Vertex> &vertices) {
    m_vertexCount = static_cast<uint32_t>(vertices.size());
    
    // The application can copy data to host-visible device memory only using this pointer
    VkDeviceSize bufferSize = sizeof(vertices[0]) * m_vertexCount;

    VKBuffer stagingBuffer {
        m_vkparams,
        bufferSize, 
        m_vertexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };

    stagingBuffer.Map();
    stagingBuffer.WriteToBuffer((void*)vertices.data());

    m_vbuffer = std::make_unique<VKBuffer>(
        m_vkparams,
        bufferSize,
        m_vertexCount,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    VKBuffer::CopyBuffer(m_vkparams, stagingBuffer.GetBuffer(), m_vbuffer->GetBuffer(), bufferSize);

    stagingBuffer.Unmap();
    stagingBuffer.Destroy();
    m_vbuffer->Unmap();
}

//
void
VKModel::Draw(VkCommandBuffer cmdBuffer, uint32_t frameIndex) {
    vkCmdBindDescriptorSets(
            cmdBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS, 
            m_vkparams.PipelineLayout, 
            0, 
            1, 
            &m_vkparams.DescriptorSets[frameIndex], 
            0, 
            nullptr);
    if (m_hasIndexBuffer) {
        vkCmdDrawIndexed(cmdBuffer, m_indexCount, 1, 0, 0, 0);
    } else {
        vkCmdDraw(cmdBuffer, m_vertexCount, 1, 0, 0);
    }
}

void
VKModel::Bind(VkCommandBuffer cmdBuffer) {
    // VkBuffer buffers [] = {m_vertexBuffer};
    VkBuffer buffers [] = {m_vbuffer->GetBuffer()};
    VkDeviceSize offsets[1] = {0};
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, buffers, offsets);

    // If we are using an index buffer then bind it as well
    if (m_hasIndexBuffer) {
        vkCmdBindIndexBuffer(cmdBuffer, m_ibuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
    }
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


