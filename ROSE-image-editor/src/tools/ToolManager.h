#pragma once

#include "tools/Tool.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

class ToolManager {
private:
    std::unordered_map<std::string, std::unique_ptr<Tool>> tools;
    Tool* activeTool;

public:
    ToolManager();
    ~ToolManager();

    void registerTool(std::unique_ptr<Tool> tool);
    void setActiveTool(const std::string& name);
    Tool* getActiveTool() const;
    std::vector<std::string> getToolList() const;
};
