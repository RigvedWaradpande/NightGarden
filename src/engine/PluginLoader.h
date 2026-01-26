#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <string>
#include "../include/IStrategy.h"

struct PluginHandle {
    void* dl_handle;          // From dlopen
    IStrategy* strategy;      // The loaded strategy instance
    std::string plugin_path;
}; 

PluginHandle LoadStrategyPlugin(const std::string& plugin_path);
void UnloadStrategyPlugin(PluginHandle& handle);

#endif