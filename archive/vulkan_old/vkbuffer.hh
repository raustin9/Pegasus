#pragma once
#include "vkcommon.hh"
#include "stdafx.hh"
#include <vulkan/vulkan_core.h>

// Used for buffers in vulkan
class VKBuffer {
    public:
        VKBuffer(
            VKCommonParameters& m_vkparams,
            VkDeviceSize instanceSize,
            uint32_t instance_count,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags memoryProps,
            VkDeviceSize minOffsetAllignment = 1);
        ~VKBuffer() {}

        VKBuffer(const VKBuffer&) = delete;
        VKBuffer& operator= (const VKBuffer&) = delete;

        // Map and Unmap the memory in a buffer
        VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        void Unmap();

        void WriteToBuffer(void* data, VkDeviceSize = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        // VkResult Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkDescriptorBufferInfo BufferDescriptor(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        // VkResult Invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        // Indexing
        void WriteToIndex(void* data, int index);
        // VkResult FlushIndex(int index);
        VkDescriptorBufferInfo IndexDescriptor(int index);
        // VkResult InvalidateIndex(int index);

        void Destroy();
        
        static void CreateBuffer(VKCommonParameters& params, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags props, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        static void CopyBuffer(VKCommonParameters& params, VkBuffer src, VkBuffer dst, VkDeviceSize size);

        // Accessors
        VkBuffer GetBuffer() const { return m_buffer; }
        void* GetMappedMemory() const { return m_mapped; }
        uint32_t GetInstanceCount() const { return m_instanceCount; }
        VkDeviceSize GetInstanceSize() const { return m_instanceSize; }
        VkDeviceSize GetAllignmentSize() const { return m_allignmentSize; }
        VkBufferUsageFlags GetUsageFlags() const { return m_usage; }
        VkMemoryPropertyFlags GetMemoryProperties() const { return m_memProps; }
        VkDeviceSize GetBufferSize() const { return m_bufferSize; }
    private:
        static VkDeviceSize GetAllignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAllignment);

        VKCommonParameters& m_vkparams;
        void* m_mapped = nullptr;
        VkBuffer m_buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_memory = VK_NULL_HANDLE;

        VkDeviceSize m_bufferSize;
        VkDeviceSize m_instanceSize;
        uint32_t m_instanceCount;
        VkDeviceSize m_allignmentSize;
        VkBufferUsageFlags m_usage;
        VkMemoryPropertyFlags m_memProps;
};
