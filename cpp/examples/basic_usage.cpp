#include <iostream>
#include "agent_zero/agent_core.hpp"
#include "agent_zero/memory_manager.hpp"
#include "agent_zero/multi_lang_bridge.hpp"

using namespace agent_zero;

int main() {
    std::cout << "=== Agent Zero C++ Backend - Basic Usage Example ===" << std::endl << std::endl;
    
    // Create an agent using the builder pattern
    auto agent = utils::AgentBuilder()
        .with_name("ExampleAgent")
        .with_model("gpt-4")
        .build();
    
    std::cout << "Created agent: " << agent->get_name() 
              << " using model: " << agent->get_model() << std::endl;
    
    // Process a message
    std::string message = "Hello, I need help with a task!";
    std::cout << "\nSending message: " << message << std::endl;
    
    std::string response = agent->process_message(message);
    std::cout << "Response: " << response << std::endl;
    
    // Set context and process another message
    std::map<std::string, std::string> context = {
        {"user_id", "12345"},
        {"session", "abc-def"}
    };
    agent->set_context(context);
    
    std::cout << "\nProcessing with context..." << std::endl;
    response = agent->process_message("What is my user ID?");
    std::cout << "Response: " << response << std::endl;
    
    // Memory management example
    std::cout << "\n=== Memory Management ===" << std::endl;
    MemoryManager memory;
    
    std::string mem_id = memory.save_memory("Important information to remember");
    std::cout << "Saved memory with ID: " << mem_id << std::endl;
    
    auto loaded = memory.load_memory(mem_id);
    if (loaded) {
        std::cout << "Loaded memory: " << loaded->content << std::endl;
    }
    
    // Multi-language bridge example
    std::cout << "\n=== Multi-Language Bridge ===" << std::endl;
    MultiLangBridge bridge;
    
    auto langs = bridge.get_available_languages();
    std::cout << "Available languages: " << langs.size() << std::endl;
    
    auto result = bridge.execute(Language::NATIVE_CPP, 
                                "std::cout << \"Hello from C++\" << std::endl;");
    if (result.success) {
        std::cout << "Execution result: " << result.result << std::endl;
    }
    
    std::cout << "\n=== Done ===" << std::endl;
    return 0;
}
