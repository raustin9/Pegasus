#include "vulkan_backend.hh"

bool
VulkanBackend::create_surface() {
  std::cout << "Creating Vulkan surface..." << std::endl;
  if (!Platform::create_vulkan_surface(m_context)) {
      return false;
  }
  
  return true;
}