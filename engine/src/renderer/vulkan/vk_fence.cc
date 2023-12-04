#include "vulkan_backend.hh"
#include "vk_fence.hh"

void
VKFence::create(VKContext& context, bool create_signaled) {
    // Make sure to signal the fence if it is required
    this->is_signaled = create_signaled;
    VkFenceCreateInfo fence_info {};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if (this->is_signaled) {
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    }

    VK_CHECK(vkCreateFence(
        context.device.logical_device,
        &fence_info,
        context.allocator,
        &this->handle
    ));
    std::cout << "VKFence created..." << std::endl;
}

void 
VKFence::destroy(VKContext& context) {
    if(this->handle) {
        vkDestroyFence(
            context.device.logical_device,
            this->handle,
            context.allocator
        );
        this->handle = nullptr;
    }

    this->is_signaled = false;
}

bool 
VKFence::wait(VKContext& context, uint64_t timeout_ms) {
    if (!this->is_signaled) {
        VkResult result = vkWaitForFences(
            context.device.logical_device,
            1,
            &this->handle,
            true,
            timeout_ms
        );

        switch(result) {
            case VK_SUCCESS:
                this->is_signaled = true;
                return true;
            case VK_TIMEOUT: 
                std::cout << "VKFence.wait(): timed out" << std::endl;
                break;
            case VK_ERROR_DEVICE_LOST: 
                std::cout << "VKFence.wait(): VK_ERROR_DEVICE_LOST" << std::endl;
                break;
            case VK_ERROR_OUT_OF_HOST_MEMORY: 
                std::cout << "VKFence.wait(): VK_ERROR_OUT_OF_HOST_MEMORY" << std::endl;
                break;
            case VK_ERROR_OUT_OF_DEVICE_MEMORY: 
                std::cout << "VKFence.wait(): VK_ERROR_OUT_OF_DEVICE_MEMORY" << std::endl;
                break;
            default:
                std::cout << "VKFence.wait(): An unknown error has occured" << std::endl;
                break;
        }
    } else {
        return true;
    }

    return false;
}

void 
VKFence::reset(VKContext& context) {
    if (this->is_signaled) {
        VK_CHECK(vkResetFences(context.device.logical_device, 1, &this->handle));
        this->is_signaled = false;
    }
}
