#ifndef AGENT_ZERO_CORE_HPP
#define AGENT_ZERO_CORE_HPP

#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <map>
#include <optional>
#include <future>

namespace agent_zero {

// Forward declarations
class TaskExecutor;
class MemoryManager;
class ToolInterface;

// Type aliases for functional programming
using TaskCallback = std::function<void(const std::string&)>;
using ErrorHandler = std::function<void(const std::exception&)>;
using AsyncTask = std::future<std::string>;

// Enumeration for agent states
enum class AgentState {
    IDLE,
    PROCESSING,
    WAITING,
    ERROR,
    COMPLETED
};

// Paradigm: Object-Oriented Programming (OOP)
// Core Agent class using OOP principles
class Agent {
public:
    Agent(const std::string& name, const std::string& model);
    virtual ~Agent();
    
    // Prevent copying
    Agent(const Agent&) = delete;
    Agent& operator=(const Agent&) = delete;
    
    // Allow moving
    Agent(Agent&&) noexcept;
    Agent& operator=(Agent&&) noexcept;
    
    // Core methods
    virtual std::string process_message(const std::string& message);
    virtual void set_context(const std::map<std::string, std::string>& context);
    virtual AgentState get_state() const;
    
    // Getters and setters
    std::string get_name() const { return name_; }
    void set_name(const std::string& name) { name_ = name; }
    
    std::string get_model() const { return model_; }
    void set_model(const std::string& model) { model_ = model; }
    
protected:
    std::string name_;
    std::string model_;
    AgentState state_;
    std::map<std::string, std::string> context_;
    
private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

// Paradigm: Functional Programming
// Functional utilities for task composition and transformation
namespace functional {
    
    // Higher-order function: map
    template<typename T, typename Func>
    std::vector<typename std::result_of<Func(T)>::type> 
    map(const std::vector<T>& input, Func func) {
        using ResultType = typename std::result_of<Func(T)>::type;
        std::vector<ResultType> result;
        result.reserve(input.size());
        for (const auto& item : input) {
            result.push_back(func(item));
        }
        return result;
    }
    
    // Higher-order function: filter
    template<typename T, typename Pred>
    std::vector<T> filter(const std::vector<T>& input, Pred predicate) {
        std::vector<T> result;
        for (const auto& item : input) {
            if (predicate(item)) {
                result.push_back(item);
            }
        }
        return result;
    }
    
    // Higher-order function: reduce/fold
    template<typename T, typename Acc, typename Func>
    Acc reduce(const std::vector<T>& input, Acc initial, Func func) {
        Acc result = initial;
        for (const auto& item : input) {
            result = func(result, item);
        }
        return result;
    }
    
    // Function composition
    template<typename F, typename G>
    auto compose(F f, G g) {
        return [=](auto x) { return f(g(x)); };
    }
}

// Paradigm: Procedural Programming
// C-style procedural interface for maximum compatibility
extern "C" {
    
    // Opaque handle types
    typedef void* AgentHandle;
    typedef void* TaskHandle;
    typedef void* MemoryHandle;
    
    // Agent lifecycle functions
    AgentHandle agent_create(const char* name, const char* model);
    void agent_destroy(AgentHandle agent);
    
    // Processing functions
    const char* agent_process_message(AgentHandle agent, const char* message);
    int agent_get_state(AgentHandle agent);
    
    // Context management
    void agent_set_context_value(AgentHandle agent, const char* key, const char* value);
    const char* agent_get_context_value(AgentHandle agent, const char* key);
    
    // Error handling
    const char* agent_get_last_error();
    void agent_clear_error();
}

// Modern C++ utilities
namespace utils {
    
    // RAII wrapper for async operations
    class AsyncOperation {
    public:
        explicit AsyncOperation(std::function<std::string()> task);
        ~AsyncOperation();
        
        std::string wait();
        bool is_ready() const;
        std::optional<std::string> try_get();
        
    private:
        std::future<std::string> future_;
    };
    
    // Builder pattern for agent configuration
    class AgentBuilder {
    public:
        AgentBuilder& with_name(const std::string& name);
        AgentBuilder& with_model(const std::string& model);
        AgentBuilder& with_context(const std::map<std::string, std::string>& context);
        AgentBuilder& with_callback(TaskCallback callback);
        
        std::unique_ptr<Agent> build();
        
    private:
        std::string name_;
        std::string model_;
        std::map<std::string, std::string> context_;
        std::optional<TaskCallback> callback_;
    };
}

} // namespace agent_zero

#endif // AGENT_ZERO_CORE_HPP
