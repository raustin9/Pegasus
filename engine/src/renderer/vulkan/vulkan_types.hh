#pragma once
#include "defines.hh"
#include <vulkan/vulkan.h>
#include <vector>

struct VKContext;

struct VKSwapchainSupportInfo {
    VkSurfaceCapabilitiesKHR capabilities;
    uint32_t format_count;
    std::vector<VkSurfaceFormatKHR> formats{};
    uint32_t present_mode_count;
    std::vector<VkPresentModeKHR> present_modes{};
};

// Information for the logical and physical devices
struct VKDevice {
    VkPhysicalDevice       physical_device;
    VkDevice               logical_device;
    VKSwapchainSupportInfo swapchain_support;

    int32_t graphics_queue_index;
    int32_t present_queue_index;
    int32_t transfer_queue_index;
    // NOTE: put compute index here if needed

    VkQueue graphics_queue;
    VkQueue present_queue;
    VkQueue transfer_queue;

    VkCommandPool graphics_command_pool;

    VkPhysicalDeviceProperties       properties;
    VkPhysicalDeviceFeatures         features;
    VkPhysicalDeviceMemoryProperties memory;

    VkFormat depth_format;
};

struct VKImage {
    VkImage handle;
    VkDeviceMemory memory;
    VkImageView view;
    uint32_t width;
    uint32_t height;
};

struct VKRenderpass {
    VkRenderPass handle;
    float x, y, w, h;
    float r, g, b, a;

    float depth;
    float stencil;
};

enum command_buffer_state : uint32_t {
    COMMAND_BUFFER_STATE_READY,
    COMMAND_BUFFER_STATE_RECORDING,       
    COMMAND_BUFFER_STATE_IN_RENDER_PASS,  
    COMMAND_BUFFER_STATE_RECORDING_ENDED,
    COMMAND_BUFFER_STATE_SUBMITTED,       
    COMMAND_BUFFER_STATE_NOT_ALLOCATED, // cb's start here. Once allocated we switch to READY
};

struct VKCommandBuffer {
    VkCommandBuffer handle;
    command_buffer_state state;

    void allocate(VKContext& context, VkCommandPool pool, bool is_primary);
    void free(VKContext& context, VkCommandPool pool);
    void begin(bool is_single_use, bool is_renderpass_continue, bool is_simultaneous_use);
    void end();
    void update_submitted();
    void reset();
    void allocate_and_begin_single_use(
        VKContext& context,
        VkCommandPool pool
    );
    void end_single_use(
        VKContext& context,
        VkCommandPool pool,
        VkQueue queue
    ); 
};

struct VKFence {
    VkFence handle;
    command_buffer_state state;
};

struct VKFramebuffer {
    VkFramebuffer handle;
    uint32_t attachment_count;
    std::vector<VkImageView> attachments;
    VKRenderpass *renderpass;
};

struct VKSwapchain {

    VkSurfaceFormatKHR image_format;
    uint8_t max_frames_in_flight;
    VkSwapchainKHR handle;
    uint32_t image_count;
    std::vector<VkImage> images;
    std::vector<VkImageView> views;

    VKImage depth_attachment;
    std::vector<VKFramebuffer> framebuffers;
};

// Holds vulkan-specific information
// for the renderer backend
struct VKContext {
    uint32_t image_index;
    uint32_t current_frame;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint32_t framebuffer_size_generation;
    uint64_t framebuffer_size_last_generation;
    bool recreating_swapchain;

    VkInstance             instance;
    VkAllocationCallbacks *allocator;
    VkSurfaceKHR           surface;
    
#if defined(P_DEBUG)
    VkDebugUtilsMessengerEXT debug_messenger;
#endif // P_DEBUG

    VKDevice device;
    VKSwapchain swapchain;
    VKRenderpass main_renderpass;

    std::vector<VKCommandBuffer> graphics_command_buffers;
    std::vector<VkSemaphore> image_available_semaphores;
    std::vector<VkSemaphore> queue_complete_semaphores;

    uint32_t in_flight_fence_count;
    std::vector<VKFence> in_flight_fences;



    int32_t find_memory_index(uint32_t type_filter, uint32_t property_flags);
};