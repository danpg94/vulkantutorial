#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

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
        void InitWindow(){
            glfwInit();
            glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE);

            _window = glfwCreateWindow( WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        }

        void InitVulkan(){
            CreateInstance();

        }

        void MainLoop(){
            while (!glfwWindowShouldClose(_window)){
                glfwPollEvents();
            }
        }

        void Cleanup(){

            vkDestroyInstance( _instance, nullptr);
            glfwDestroyWindow( _window);
            glfwTerminate();
        }

        void CreateInstance(){
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

            uint32_t glfwExtentionCount = 0;
            const char **glfwExtentions;
            glfwExtentions = glfwGetRequiredInstanceExtensions( &glfwExtentionCount);

            createInfo.enabledExtensionCount = glfwExtentionCount;
            createInfo.ppEnabledExtensionNames = glfwExtentions;

            createInfo.enabledLayerCount = 0;

            if ( vkCreateInstance( &createInfo, nullptr, &_instance ) != VK_SUCCESS){
                throw std::runtime_error( "Failed to create Vulkan Instance!" );
            }

            std::vector<VkExtensionProperties> extensions(glfwExtentionCount);

            vkEnumerateInstanceExtensionProperties( nullptr, &glfwExtentionCount, extensions.data());

            std::cout << "Available Extentions: " << std::endl;

            for (const auto &extenstion : extensions){
                std::cout << '\t' << extenstion.extensionName << std::endl;
            }

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