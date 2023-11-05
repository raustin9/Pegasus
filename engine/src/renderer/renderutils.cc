#include "renderer.hh"
#include <fstream>
#include <vulkan/vulkan_core.h>

VkShaderModule
Renderer::LoadShader(std::string filename) {
    size_t shaderSize;
    char* shaderCode = NULL;

    std::ifstream is(filename, std::ios::binary | std::ios::in | std::ios::ate);

    if (is.is_open()) {
        shaderSize = is.tellg();
        is.seekg(0, std::ios::beg);
        // Copy file content into a buffer
        shaderCode = new char[shaderSize];
        is.read(shaderCode, shaderSize);
        is.close();
        assert(shaderSize > 0);
    }

    if (shaderCode) {
        // Create a new shader module that will be used for pipeline creation
        VkShaderModuleCreateInfo moduleCreateInfo = {};
        moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        moduleCreateInfo.codeSize = shaderSize;
        moduleCreateInfo.pCode = reinterpret_cast<uint32_t*>(shaderCode);

        VkShaderModule shaderModule;
        VK_CHECK(
            vkCreateShaderModule(m_vkparams.Device.Device, &moduleCreateInfo, m_vkparams.Allocator, &shaderModule));

        delete[] shaderCode;
        return shaderModule;
    } else {
        std::cerr << "Error: could not open shader file " << filename << std::endl;
        return VK_NULL_HANDLE;
    }
}


uint32_t 
Renderer::GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags props, VkPhysicalDeviceMemoryProperties deviceMemoryProperties) {
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((typeBits & 1) == 1) {
            if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & props) == props) {
                return i;
            }
        }
        typeBits >>= 1;
    }

    std::cerr << "Error: could not find suitable memory type" << std::endl;
    assert(0);
    return (0);
}
