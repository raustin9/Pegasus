#include "platform.hh"

#ifdef Q_PLATFORM_WINDOWS

struct PlatformState {
	std::string name;
	HWND hWindow;
	HINSTANCE hInstance;
	bool should_quit;
	uint32_t width = 0;
	uint32_t height = 0;
};

// Global state for windows platform
static PlatformState windows_state = {};

Platform::Platform(std::string name, uint32_t width, uint32_t height) 
	: name{name} , width{width} , height{height} {

	hInstance = GetModuleHandle(0);

	// Setup and register window class
	HICON icon = LoadIcon(hInstance, IDI_APPLICATION);
	WNDCLASSA wc { 0 };
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = Platform::WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = icon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszClassName = "Pegasus Window Class";

	if (!RegisterClassA(&wc)) {
		MessageBoxA(
			0,
			"Window registration failed",
			"Error!",
			MB_ICONEXCLAMATION | MB_OK
		);
		return;
	}
}

// Startup behavior for the platform layer subsystem
bool
Platform::Startup(std::string name, uint32_t width, uint32_t height) {
	windows_state.hInstance = GetModuleHandle(0);
	windows_state.width = width;
	windows_state.height = height;
	windows_state.name = name;

	// Setup and register window class
	HICON icon = LoadIcon(windows_state.hInstance, IDI_APPLICATION);
	WNDCLASSA wc { 0 };
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = Platform::WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = windows_state.hInstance;
	wc.hIcon = icon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszClassName = "Pegasus Window Class";

	if (!RegisterClassA(&wc)) {
		MessageBoxA(
			0,
			"Window registration failed",
			"Error!",
			MB_ICONEXCLAMATION | MB_OK
		);
		return false;
	}

	Platform::create_window();
	std::cout << "Window Created..." << std::endl;

	return true;
}

// Windows implementation for getting the current time
std::chrono::time_point<std::chrono::high_resolution_clock>
Platform::get_current_time() {
	return std::chrono::high_resolution_clock::now();
}

// Set the title of the window
void
Platform::set_title(std::string title) {
	SetWindowTextA(windows_state.hWindow, static_cast<LPCSTR>(title.c_str()));
}

// Create the window for the application
void
Platform::create_window() {
	// Create the window
	uint32_t client_x = 300;
	uint32_t client_y = 100;
	uint32_t client_width = windows_state.width;
	uint32_t client_height = windows_state.height;

	uint32_t window_x = client_x;
	uint32_t window_y = client_y;
  uint32_t window_width = client_width;
  uint32_t window_height = client_height;

	uint32_t window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
	uint32_t window_ex_style = WS_EX_APPWINDOW;

	window_style |= WS_MAXIMIZEBOX;
	window_style |= WS_MINIMIZEBOX;
	window_style |= WS_THICKFRAME;

	// Get size of the border
	RECT border_rect = {0,0,0,0};
	AdjustWindowRectEx(&border_rect, window_style, FALSE, window_ex_style);

	window_x += border_rect.left;
	window_y += border_rect.top;

	// Grow by the size of the OS border
	window_width += border_rect.right - border_rect.left;
	window_height += border_rect.bottom - border_rect.top;

	// Create the window
	windows_state.hWindow = CreateWindowExA(
		window_ex_style,
		"Pegasus Window Class",
		windows_state.name.c_str(),
		window_style,
		window_x,
		window_y,
		window_width,
		window_height,
		nullptr,
		nullptr,
		windows_state.hInstance,
		nullptr
	);

	if (windows_state.hWindow == nullptr) {
		MessageBoxA(NULL, "Window creation failed", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return;
	}

	// Show the window
	bool should_activate = true;
	int32_t show_window_command_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;

	ShowWindow(windows_state.hWindow, show_window_command_flags);
}

// Destroy the window
void
Platform::destroy_window() {

}

// Pump messages from the platform to the application
bool
Platform::pump_messages() {
	MSG message;

	// takes messages from the queue and pumps it to the application
	while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessageA(&message);
	}

	return false;
}

// Windows implementation for getting a vulkan surface
bool
Platform::create_vulkan_surface(VKCommonParameters &params) {
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceCreateInfo.hinstance = windows_state.hInstance;
		surfaceCreateInfo.hwnd = windows_state.hWindow;
		
		VkResult err = vkCreateWin32SurfaceKHR(
			params.Instance, &surfaceCreateInfo, params.Allocator, &params.PresentationSurface);
		
		return (err == VK_SUCCESS) ? true : false;
}

// Callback function for handling messages from the window
LRESULT CALLBACK 
Platform::WindowProc(
	HWND hWnd,
	uint32_t code,
	WPARAM w_param,
	LPARAM l_param
) {
	switch (code) {
		case WM_ERASEBKGND: // notify the OS that erasing the screen will be handled by Application
			return 1;

		case WM_CLOSE: {
			// Fire an event for the application to quit
			EventContext data = {};
			EventHandler::Fire(EVENT_CODE_APPLICATION_QUIT, nullptr, data);
			return true;
		}

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		
		case WM_SIZE: {
			RECT r;
			GetClientRect(hWnd, &r);
			uint32_t width = r.right - r.left;
			uint32_t height = r.bottom - r.top;

			InputHandler::ProcessResize(
					width, 
					height
			);
		} break;

		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP: {
			bool pressed = (code == WM_KEYDOWN || code == WM_SYSKEYDOWN);
			Keys key = static_cast<Keys>(w_param);

			// Pass the input subsystem
			InputHandler::ProcessKey(key, pressed);
		}

		case WM_MOUSEMOVE:
			// Fire an event for mouse movement
			break;
		case WM_MOUSEWHEEL:
			// Fire an event for mouse movement
			break;

		case WM_LBUTTONUP:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDOWN: {
				// TODO: fire events for mouse buttons
		} break;

	}
	
	return DefWindowProc(hWnd, code, w_param, l_param);
}

#endif /* Q_PLATFORM_WINDOWS */