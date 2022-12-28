#define GLFW_INCLUDE_VULKAN
#define NDBUG
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

const std::vector<const char*> validationLayers = 
{
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT( VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger){
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr( instance, "vkCreateDebugUtilsMessengerEXT");

    if (func != nullptr)
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    else{
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT( VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator){
    auto func = ( PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr( instance, "vkDestroyUtilsMessengerEXT");
    if (func != nullptr)
        func( instance, debugMessenger, pAllocator);
}

class HelloTriangleApplication{
    public:
        void Run(){
            InitWindow();
            InitVulkan();
            MainLoop();
            Cleanup();
        }

    private:
        GLFWwindow * _window;
        VkInstance  _instance;
        VkDebugUtilsMessengerEXT _debugMessenger;

        void InitWindow(){
            glfwInit();
            glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE);

            _window = glfwCreateWindow( WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        }

        void InitVulkan(){
            CreateInstance();
            SetupDebugMessenger();
        }

        void MainLoop(){
            while (!glfwWindowShouldClose(_window)){
                glfwPollEvents();
            }
        }

        void Cleanup(){
            if (enableValidationLayers)
                DestroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
            vkDestroyInstance( _instance, nullptr);
            glfwDestroyWindow( _window);
            glfwTerminate();
        }

        void CreateInstance(){

            if (enableValidationLayers && !CheckValidationLayerSupport( ))
                throw std::runtime_error("Validation layers requested, but not available");

            VkApplicationInfo appInfo{};
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pApplicationName = "Hello Triangle";
            appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.pEngineName = "Test";
            appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.apiVersion = VK_API_VERSION_1_0;

            VkInstanceCreateInfo createInfo{ };
            createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            createInfo.pApplicationInfo = &appInfo;

            auto extensions = GetRequiredExtensions();
            createInfo.enabledExtensionCount = static_cast<uint32_t>( extensions.size());
            createInfo.ppEnabledExtensionNames = extensions.data();

            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

            if (enableValidationLayers){
                createInfo.enabledLayerCount = static_cast<unsigned int>( validationLayers.size());
                createInfo.ppEnabledExtensionNames = validationLayers.data();

                PopulateDebugMessengerCreateInfo( debugCreateInfo );
                createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT* )&debugCreateInfo;
            } else {
                createInfo.enabledLayerCount = 0;
                createInfo.pNext = nullptr;
            }
        }

        void PopulateDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT &createInfo){
            createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            createInfo.pfnUserCallback = DebugCallback;
        }

        void SetupDebugMessenger(){
            if (!enableValidationLayers)
                return;
            VkDebugUtilsMessengerCreateInfoEXT createInfo;
            PopulateDebugMessengerCreateInfo( createInfo);
            std::cout << "Struct Type: " << createInfo.sType << std::endl << "Message Severity: " << createInfo.messageSeverity << std::endl << "Message Type: " << createInfo.messageType << std::endl;

            if (CreateDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_debugMessenger) != VK_SUCCESS)
                throw std::runtime_error("Failed to setup debug messenger");
        }

        std::vector<const char*> GetRequiredExtensions(){
            uint32_t glfwExtensionCount = 0;
            const char **glfwExtensions;
            glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount);


            std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
            if (enableValidationLayers)
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            return extensions;
        }

        bool CheckValidationLayerSupport(){
            unsigned int layerCount;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

            std::vector<VkLayerProperties> availableLayers(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        
            for (const char* layerName : validationLayers){
                bool layerFound = false;

                for (const auto &layerProperties : availableLayers){
                    if (std::strcmp(layerName, layerProperties.layerName) == 0){
                        layerFound = true;
                        break;
                    }
                }

                if (layerFound = false)
                    return false;
            }
            return true;
        }

        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData){
            std::cerr << "Validations layer " << pCallbackData->pMessage << std::endl;

            return VK_FALSE;
        }
};

int main(){
    HelloTriangleApplication app;

    try{
        app.Run();
    }
    catch (const std::exception &e){
        std::cerr << e.what() << std::endl;

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}