#include "vulkan_backend.hh"
#include "core/qlogger.hh"
#include <vector>

bool
VulkanBackend::create_instance(const char* name) {
    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.apiVersion = VK_API_VERSION_1_2;
    app_info.pApplicationName = name;
    app_info.applicationVersion = VK_MAKE_VERSION(1,0,0);
    app_info.pEngineName = "Pegasus Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1,0,0);

    // Create the vulkan Instance
    VkInstanceCreateInfo instance_info = {};
    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &app_info;

    // TODO: Get platform required extensions
    std::vector<const char*> required_extensions = {};
    required_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    Platform::get_vulkan_extensions(required_extensions);
    
    // If debugging, find the validation layers that we want to use
#if defined(P_DEBUG)
    required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    qlogger::Info("Required extensions: ");
    for (size_t i = 0; i < required_extensions.size(); i++) {
        qlogger::Info("\t%s", required_extensions[i]);
    }
#endif

    instance_info.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
    instance_info.ppEnabledExtensionNames = required_extensions.data();

    std::vector<const char*> required_validation_layer_names;


#if defined(P_DEBUG)
    qlogger::Info("Validation layers enabled. Enumerating...");
    std::vector<const char*> required_validation_layers;
    required_validation_layer_names.push_back("VK_LAYER_KHRONOS_validation");

    // Obtain a list of available validation layers
    uint32_t available_layer_count = 0;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, nullptr));
    std::vector<VkLayerProperties> available_layers(available_layer_count);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers.data()));

    for (size_t i = 0; i < required_validation_layer_names.size(); i++) {
        qlogger::Info("Searching for layer: %s...", required_validation_layer_names[i]);
        bool found = false;
        for (size_t j = 0; j < available_layer_count; j++) {
            if (strcmp(required_validation_layer_names[i], available_layers[j].layerName) == 0) {
                found = true;
                qlogger::Info("Found.");
                break;
            }
        }

        if (!found) {
            qlogger::Info("Required validation layer is missing: %s", required_validation_layer_names[i]);
            return false;
        }
    }

    qlogger::Info("All validation layers found.");
#endif // debug

    instance_info.enabledLayerCount = static_cast<uint32_t>(required_validation_layer_names.size());
    instance_info.ppEnabledLayerNames = required_validation_layer_names.data();

    VK_CHECK(vkCreateInstance(&instance_info, m_context.allocator, &m_context.instance));
    qlogger::Info("Instance created...");

    return true;
}