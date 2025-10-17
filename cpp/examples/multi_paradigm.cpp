#include <iostream>
#include <vector>
#include "agent_zero/agent_core.hpp"

using namespace agent_zero;

// Demonstrate different programming paradigms

// 1. Object-Oriented Programming
class CustomAgent : public Agent {
public:
    CustomAgent(const std::string& name) : Agent(name, "custom-model") {}
    
    std::string process_message(const std::string& message) override {
        return "Custom processing: " + message;
    }
};

// 2. Procedural Programming
void procedural_example() {
    std::cout << "=== Procedural Programming (C-style) ===" << std::endl;
    
    AgentHandle agent = agent_create("procedural_agent", "model-1");
    const char* response = agent_process_message(agent, "test message");
    
    if (response) {
        std::cout << "Response: " << response << std::endl;
    }
    
    agent_destroy(agent);
}

// 3. Functional Programming
void functional_example() {
    std::cout << "\n=== Functional Programming ===" << std::endl;
    
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // Map: double each number
    auto doubled = functional::map(numbers, [](int x) { return x * 2; });
    
    // Filter: keep only even numbers
    auto evens = functional::filter(doubled, [](int x) { return x % 2 == 0; });
    
    // Reduce: sum all numbers
    auto sum = functional::reduce(evens, 0, [](int acc, int x) { return acc + x; });
    
    std::cout << "Original: ";
    for (auto n : numbers) std::cout << n << " ";
    std::cout << "\nSum of doubled evens: " << sum << std::endl;
    
    // Function composition
    auto add_ten = [](int x) { return x + 10; };
    auto multiply_two = [](int x) { return x * 2; };
    auto composed = functional::compose(add_ten, multiply_two);
    
    std::cout << "Composed function (5): " << composed(5) << std::endl;
}

// 4. Generic Programming with Templates
template<typename T>
void print_vector(const std::vector<T>& vec) {
    for (const auto& item : vec) {
        std::cout << item << " ";
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "=== Multi-Paradigm Programming in Agent Zero ===" << std::endl << std::endl;
    
    // Object-Oriented
    std::cout << "=== Object-Oriented Programming ===" << std::endl;
    CustomAgent custom_agent("OOP_Agent");
    std::cout << custom_agent.process_message("Hello OOP!") << std::endl;
    
    // Procedural
    procedural_example();
    
    // Functional
    functional_example();
    
    // Generic Programming
    std::cout << "\n=== Generic Programming ===" << std::endl;
    std::vector<int> int_vec = {1, 2, 3, 4, 5};
    std::vector<std::string> str_vec = {"hello", "world", "from", "generic"};
    
    std::cout << "Int vector: ";
    print_vector(int_vec);
    
    std::cout << "String vector: ";
    print_vector(str_vec);
    
    std::cout << "\n=== Done ===" << std::endl;
    return 0;
}
