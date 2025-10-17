#ifndef AGENT_ZERO_TOOL_INTERFACE_HPP
#define AGENT_ZERO_TOOL_INTERFACE_HPP

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <variant>

namespace agent_zero {

// Tool parameter types
using ToolParameter = std::variant<std::string, int, double, bool>;
using ToolParameters = std::map<std::string, ToolParameter>;

// Tool result
struct ToolResult {
    bool success;
    std::string output;
    std::string error;
    std::map<std::string, std::string> metadata;
    
    ToolResult() : success(false) {}
    explicit ToolResult(const std::string& out) : success(true), output(out) {}
};

// Abstract tool interface
class ToolInterface {
public:
    virtual ~ToolInterface() = default;
    
    virtual std::string get_name() const = 0;
    virtual std::string get_description() const = 0;
    virtual std::vector<std::string> get_required_parameters() const = 0;
    
    virtual ToolResult execute(const ToolParameters& params) = 0;
};

// Tool registry for managing available tools
class ToolRegistry {
public:
    static ToolRegistry& instance();
    
    void register_tool(const std::string& name, std::shared_ptr<ToolInterface> tool);
    void unregister_tool(const std::string& name);
    
    std::shared_ptr<ToolInterface> get_tool(const std::string& name);
    std::vector<std::string> list_tools() const;
    
    ToolResult execute_tool(const std::string& name, const ToolParameters& params);
    
private:
    ToolRegistry() = default;
    std::map<std::string, std::shared_ptr<ToolInterface>> tools_;
};

// Base class for implementing custom tools
class BaseTool : public ToolInterface {
public:
    BaseTool(const std::string& name, const std::string& description)
        : name_(name), description_(description) {}
    
    std::string get_name() const override { return name_; }
    std::string get_description() const override { return description_; }
    
protected:
    std::string name_;
    std::string description_;
};

} // namespace agent_zero

#endif // AGENT_ZERO_TOOL_INTERFACE_HPP
