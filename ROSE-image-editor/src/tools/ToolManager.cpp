#include "tools/ToolManager.h"
#include <stdexcept>

ToolManager::ToolManager() : activeTool(nullptr) {
}

ToolManager::~ToolManager() {
}

void ToolManager::registerTool(std::unique_ptr<Tool> tool) {
    std::string name = tool->getName();
    tools[name] = std::move(tool);
}

void ToolManager::setActiveTool(const std::string& name) {
    auto it = tools.find(name);
    if (it == tools.end()) {
        throw std::runtime_error("Tool not found: " + name);
    }
    if (activeTool) {
        activeTool->setIsActive(false);
    }
    activeTool = it->second.get();
    activeTool->setIsActive(true);
}

Tool* ToolManager::getActiveTool() const {
    return activeTool;
}

std::vector<std::string> ToolManager::getToolList() const {
    std::vector<std::string> list;
    for (const auto& pair : tools) {
        list.push_back(pair.first);
    }
    return list;
}
