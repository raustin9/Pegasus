#pragma once
#include "defines.hh"
#include "renderer/render_types.hh"
#include <vulkan/vulkan.h>
#include <vector>

struct VKContext;
struct VKCommandBuffer;
struct VKFramebuffer;

struct VKBuffer {
    uint64_t total_size;
    VkBuffer handle;
    VkBufferUsageFlags usage;
    bool is_locked;
    VkDeviceMemory memory;
    int32_t memory_index;
    uint32_t memory_property_flags; 

    bool Create(
        VKContext& context,
        uint64_t size,
        VkBufferUsageFlags usage,
        uint32_t memory_property_flags,
        bool bind_on_create
    );
    void Destroy(VKContext& context);
    bool Resize(
        VKContext& context,
        uint64_t new_size,
        VkQueue queue,
        VkCommandPool pool
    );
    void Bind(VKContext& context, uint64_t offset);
    void* LockMemory(VKContext& context, uint64_t offset, uint64_t size, uint32_t flags);
    void UnlockMemory(VKContext& context);
    void LoadData(VKContext& context, uint64_t offset, uint64_t size, uint32_t flags, const void* data);

    void CopyTo(
        VKContext& context,
        VkCommandPool pool,
        VkFence fence,
        VkQueue queue,
        uint64_t source_offset,
        VkBuffer dest,
        uint64_t dest_offset,
        uint64_t size
    );
};

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

    void end(VKCommandBuffer& command_buffer);
    void begin(VKCommandBuffer& command_buffer, VKFramebuffer& framebuffer);
};

struct VKShaderStage {
    VkShaderModuleCreateInfo create_info;
    VkShaderModule handle;
    VkPipelineShaderStageCreateInfo shader_stage_create_info;
};

struct VKPipeline {
    VkPipeline handle;
    VkPipelineLayout layout;

    bool Create(
        VKContext& context,
        VKRenderpass& renderpass,
        uint32_t attribute_count,
        VkVertexInputAttributeDescription* attributes,
        uint32_t descriptor_set_layout_count,
        VkDescriptorSetLayout* descriptor_set_layouts,
        uint32_t stage_count,
        VkPipelineShaderStageCreateInfo* stages,
        VkViewport viewport,
        VkRect2D scissor,
        bool is_wireframe
    );

    void Destroy(VKContext& context);
    void Bind(VKCommandBuffer& command_buffer, VkPipelineBindPoint bind_point);
};

constexpr uint64_t OBJECT_SHADER_STAGE_COUNT = 2; // Vert/Frag
struct VKObjShader {
    VKShaderStage stages[OBJECT_SHADER_STAGE_COUNT];

    global_uniform_object global_ubo;

    VKBuffer global_uniform_buffer;

    VkDescriptorPool global_descriptor_pool;
    VkDescriptorSetLayout global_descriptor_set_layout;

    // One descriptor set per frame -- max of 3 for triple buffering
    VkDescriptorSet global_descriptor_sets[3];
    
    VKPipeline pipeline;
    

    bool Create(VKContext& context);
    void Destroy(VKContext& context);
    void Use(VKContext& context);
    void UpdateGlobalState(VKContext& context);
    void Object(VKContext& context, qmath::Mat4<float> model);
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
    bool is_signaled;
    bool is_in_use;

    void create(VKContext& context, bool create_signaled);
    void destroy(VKContext& context);
    bool wait(VKContext& context, uint64_t timeout_ms);
    void reset(VKContext& context);
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

    void create(VKContext& context, uint32_t width, uint32_t height);
    void destroy(VKContext& context);
    bool recreate(VKContext& context, uint32_t width, uint32_t height);
    bool acquire_next_image_index(
        VKContext& context, 
        uint64_t timeout_ms, 
        VkSemaphore image_available_semaphore, 
        VkFence fence, 
        uint32_t& present_image_index
    );
    bool present(
        VKContext& context,
        VkQueue graphics_queue,
        VkQueue present_queue,
        VkSemaphore render_complete_semaphore,
        uint32_t present_image_index
    );

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

    VKBuffer object_vertex_buffer;
    VKBuffer object_index_buffer;

    std::vector<VKCommandBuffer> graphics_command_buffers;
    std::vector<VkSemaphore> image_available_semaphores;
    std::vector<VkSemaphore> queue_complete_semaphores;

    uint32_t in_flight_fence_count;
    std::vector<VKFence> in_flight_fences;
    std::vector<VKFence*> images_in_flight;

    VKObjShader object_shader;

    uint64_t geometry_vertex_offset = 0;
    uint64_t geometry_index_offset = 0;
    int32_t find_memory_index(uint32_t type_filter, uint32_t property_flags);
};