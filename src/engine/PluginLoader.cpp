// PluginLoader.cpp
#include "PluginLoader.h"
#include <dlfcn.h>
#include <iostream>

// Define function pointer types
typedef IStrategy* (*CreateStrategyFunc)();
typedef void (*DestroyStrategyFunc)(IStrategy*);

PluginHandle LoadStrategyPlugin(const std::string& plugin_path) {
    PluginHandle handle;
    handle.plugin_path = plugin_path;
    
    // Step 1: Load the .so file
    handle.dl_handle = dlopen(plugin_path.c_str(), RTLD_LAZY);
    if (!handle.dl_handle) {
        std::cerr << "Failed to load plugin: " << dlerror() << std::endl;
        handle.strategy = nullptr;
        return handle;
    }
    
    // Step 2: Find the create_strategy function
    void* func_ptr = dlsym(handle.dl_handle, "create_strategy");
    CreateStrategyFunc create_func = (CreateStrategyFunc)func_ptr;
    
    // Step 3: Check for errors
    // TODO: Check if dlerror() returns anything
    if (!func_ptr) {
        std::cerr << "dlsym failed: " << dlerror() << std::endl;
        dlclose(handle.dl_handle);
        handle.strategy = nullptr;
        return handle;
    }
    
    // Step 4: Call the factory function
    handle.strategy = create_func();
    
    return handle;
}

void UnloadStrategyPlugin(PluginHandle& handle) {
    if (handle.strategy && handle.dl_handle) {
        void* destroy_ptr = dlsym(handle.dl_handle, "destroy_strategy");
        if (destroy_ptr) {
            DestroyStrategyFunc destroy_func = (DestroyStrategyFunc)destroy_ptr;
            destroy_func(handle.strategy);
        }
        dlclose(handle.dl_handle);
        handle.strategy = nullptr;
        handle.dl_handle = nullptr;
    }
}