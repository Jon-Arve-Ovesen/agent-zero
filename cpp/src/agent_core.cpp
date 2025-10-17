#include "agent_zero/agent_core.hpp"
#include <sstream>
#include <algorithm>
#include <ctime>

namespace agent_zero {

// PIMPL implementation for Agent class
class Agent::Impl {
public:
    std::string last_message;
    std::string last_response;
};

// Constructor
Agent::Agent(const std::string& name, const std::string& model)
    : name_(name)
    , model_(model)
    , state_(AgentState::IDLE)
    , pimpl_(std::make_unique<Impl>())
{
}

// Destructor
Agent::~Agent() = default;

// Move constructor
Agent::Agent(Agent&&) noexcept = default;

// Move assignment
Agent& Agent::operator=(Agent&&) noexcept = default;

// Process message
std::string Agent::process_message(const std::string& message) {
    state_ = AgentState::PROCESSING;
    
    pimpl_->last_message = message;
    
    // Simple echo response with context awareness
    std::ostringstream response;
    response << "Agent '" << name_ << "' using model '" << model_ 
             << "' processed message: " << message;
    
    if (!context_.empty()) {
        response << " [Context: ";
        for (const auto& [key, value] : context_) {
            response << key << "=" << value << " ";
        }
        response << "]";
    }
    
    pimpl_->last_response = response.str();
    state_ = AgentState::COMPLETED;
    
    return pimpl_->last_response;
}

// Set context
void Agent::set_context(const std::map<std::string, std::string>& context) {
    context_ = context;
}

// Get state
AgentState Agent::get_state() const {
    return state_;
}

// C API Implementation
static std::string g_last_error;

extern "C" {

AgentHandle agent_create(const char* name, const char* model) {
    try {
        return new Agent(name ? name : "default", model ? model : "default");
    } catch (const std::exception& e) {
        g_last_error = e.what();
        return nullptr;
    }
}

void agent_destroy(AgentHandle agent) {
    if (agent) {
        delete static_cast<Agent*>(agent);
    }
}

const char* agent_process_message(AgentHandle agent, const char* message) {
    if (!agent || !message) {
        g_last_error = "Invalid agent handle or null message";
        return nullptr;
    }
    
    try {
        Agent* a = static_cast<Agent*>(agent);
        static std::string result;
        result = a->process_message(message);
        return result.c_str();
    } catch (const std::exception& e) {
        g_last_error = e.what();
        return nullptr;
    }
}

int agent_get_state(AgentHandle agent) {
    if (!agent) return -1;
    Agent* a = static_cast<Agent*>(agent);
    return static_cast<int>(a->get_state());
}

void agent_set_context_value(AgentHandle agent, const char* key, const char* value) {
    if (!agent || !key || !value) return;
    
    Agent* a = static_cast<Agent*>(agent);
    std::map<std::string, std::string> ctx;
    ctx[key] = value;
    a->set_context(ctx);
}

const char* agent_get_context_value(AgentHandle agent, const char* key) {
    // Simplified implementation
    return nullptr;
}

const char* agent_get_last_error() {
    return g_last_error.c_str();
}

void agent_clear_error() {
    g_last_error.clear();
}

} // extern "C"

// Utility implementations
namespace utils {

AsyncOperation::AsyncOperation(std::function<std::string()> task)
    : future_(std::async(std::launch::async, task))
{
}

AsyncOperation::~AsyncOperation() {
    if (future_.valid()) {
        try {
            future_.wait();
        } catch (...) {
            // Suppress exceptions in destructor
        }
    }
}

std::string AsyncOperation::wait() {
    return future_.get();
}

bool AsyncOperation::is_ready() const {
    return future_.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

std::optional<std::string> AsyncOperation::try_get() {
    if (is_ready()) {
        return future_.get();
    }
    return std::nullopt;
}

// AgentBuilder implementation
AgentBuilder& AgentBuilder::with_name(const std::string& name) {
    name_ = name;
    return *this;
}

AgentBuilder& AgentBuilder::with_model(const std::string& model) {
    model_ = model;
    return *this;
}

AgentBuilder& AgentBuilder::with_context(const std::map<std::string, std::string>& context) {
    context_ = context;
    return *this;
}

AgentBuilder& AgentBuilder::with_callback(TaskCallback callback) {
    callback_ = callback;
    return *this;
}

std::unique_ptr<Agent> AgentBuilder::build() {
    auto agent = std::make_unique<Agent>(
        name_.empty() ? "default" : name_,
        model_.empty() ? "default" : model_
    );
    
    if (!context_.empty()) {
        agent->set_context(context_);
    }
    
    return agent;
}

} // namespace utils

} // namespace agent_zero
