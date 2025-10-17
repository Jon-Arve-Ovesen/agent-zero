#include "agent_zero/tool_interface.hpp"
#include <stdexcept>

namespace agent_zero {

// Singleton implementation
ToolRegistry& ToolRegistry::instance() {
    static ToolRegistry instance;
    return instance;
}

void ToolRegistry::register_tool(const std::string& name, 
                                 std::shared_ptr<ToolInterface> tool) {
    if (!tool) {
        throw std::invalid_argument("Cannot register null tool");
    }
    tools_[name] = tool;
}

void ToolRegistry::unregister_tool(const std::string& name) {
    tools_.erase(name);
}

std::shared_ptr<ToolInterface> ToolRegistry::get_tool(const std::string& name) {
    auto it = tools_.find(name);
    if (it != tools_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> ToolRegistry::list_tools() const {
    std::vector<std::string> names;
    names.reserve(tools_.size());
    for (const auto& [name, tool] : tools_) {
        names.push_back(name);
    }
    return names;
}

ToolResult ToolRegistry::execute_tool(const std::string& name, 
                                     const ToolParameters& params) {
    auto tool = get_tool(name);
    if (!tool) {
        ToolResult result;
        result.success = false;
        result.error = "Tool not found: " + name;
        return result;
    }
    
    try {
        return tool->execute(params);
    } catch (const std::exception& e) {
        ToolResult result;
        result.success = false;
        result.error = std::string("Tool execution error: ") + e.what();
        return result;
    }
}

} // namespace agent_zero
