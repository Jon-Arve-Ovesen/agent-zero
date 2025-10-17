# Agent Zero C++ Backend

A high-performance, multi-paradigm C++ backend for Agent Zero with comprehensive multi-language support.

## Features

- **Multi-Paradigm Programming**: Support for Object-Oriented, Functional, and Procedural programming styles
- **Multi-Language Bindings**: Native bindings for Python and Node.js
- **Cross-Platform**: Works on Linux, macOS, and Windows
- **High Performance**: Native C++ implementation for critical operations
- **Modern C++17**: Uses modern C++ features and best practices
- **Comprehensive Testing**: Full test coverage with Google Test
- **CI/CD Ready**: GitHub Actions workflows for automated builds and releases

## Architecture

### Core Components

1. **Agent Core** (`agent_core.hpp/cpp`)
   - Main agent implementation with OOP interface
   - C-style procedural API for maximum compatibility
   - Builder pattern for flexible agent construction

2. **Task Executor** (`task_executor.hpp/cpp`)
   - Thread pool-based task execution
   - Priority queue for task scheduling
   - Pause/resume capability

3. **Memory Manager** (`memory_manager.hpp/cpp`)
   - Persistent memory storage and retrieval
   - Semantic search capabilities
   - Memory consolidation and cleanup

4. **Tool Interface** (`tool_interface.hpp/cpp`)
   - Plugin system for custom tools
   - Tool registry for dynamic tool management

5. **Multi-Language Bridge** (`multi_lang_bridge.hpp/cpp`)
   - Execute code in multiple programming languages
   - Language abstraction layer
   - Extensible executor registration

## Building

### Prerequisites

- CMake 3.15 or higher
- C++17 compatible compiler (GCC 11+, Clang 14+, MSVC 2019+)
- Python 3.9+ (for Python bindings)
- Node.js 16+ (for Node.js bindings)

### Linux/macOS

```bash
cd cpp
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

### Windows

```bash
cd cpp
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

## Build Options

- `BUILD_SHARED_LIBS`: Build shared libraries (default: ON)
- `BUILD_PYTHON_BINDINGS`: Build Python bindings (default: ON)
- `BUILD_NODE_BINDINGS`: Build Node.js bindings (default: ON)
- `BUILD_TESTS`: Build unit tests (default: ON)
- `BUILD_EXAMPLES`: Build example programs (default: ON)

Example:
```bash
cmake -B build -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF
```

## Running Tests

```bash
cd cpp/build
ctest --output-on-failure
```

## Examples

### C++ Usage

```cpp
#include "agent_zero/agent_core.hpp"

using namespace agent_zero;

int main() {
    // Object-Oriented approach
    auto agent = utils::AgentBuilder()
        .with_name("MyAgent")
        .with_model("gpt-4")
        .build();
    
    std::string response = agent->process_message("Hello!");
    std::cout << response << std::endl;
    
    // Procedural approach
    AgentHandle handle = agent_create("MyAgent", "gpt-4");
    const char* result = agent_process_message(handle, "Hello!");
    agent_destroy(handle);
    
    return 0;
}
```

### Python Usage

```python
from agent_zero_native import Agent, AgentBuilder

# Create agent
agent = Agent("PythonAgent", "gpt-4")

# Process message
response = agent.process_message("Hello from Python!")
print(response)

# Set context
agent.set_context({"user_id": "123", "session": "abc"})
```

### Node.js Usage

```javascript
const { Agent } = require('agent-zero-native');

// Create agent
const agent = new Agent('NodeAgent', 'gpt-4');

// Process message
const response = agent.processMessage('Hello from Node.js!');
console.log(response);

// Set context
agent.setContext({ userId: '123', session: 'abc' });
```

## Programming Paradigms

### Object-Oriented Programming (OOP)

```cpp
class CustomAgent : public Agent {
public:
    CustomAgent(const std::string& name) : Agent(name, "model") {}
    
    std::string process_message(const std::string& message) override {
        return "Custom: " + message;
    }
};
```

### Functional Programming

```cpp
using namespace agent_zero::functional;

std::vector<int> numbers = {1, 2, 3, 4, 5};

// Map, filter, reduce
auto doubled = map(numbers, [](int x) { return x * 2; });
auto evens = filter(doubled, [](int x) { return x % 2 == 0; });
auto sum = reduce(evens, 0, [](int a, int b) { return a + b; });
```

### Procedural Programming

```cpp
AgentHandle agent = agent_create("agent", "model");
agent_process_message(agent, "test");
agent_destroy(agent);
```

## Multi-Language Support

The Multi-Language Bridge allows executing code in different languages:

```cpp
MultiLangBridge bridge;

// Execute Python
auto result = bridge.execute_python("print('Hello from Python!')");

// Execute JavaScript  
auto result = bridge.execute_javascript("console.log('Hello from JS!')");

// Execute Go
auto result = bridge.execute_go("fmt.Println(\"Hello from Go!\")");
```

## CI/CD

The project includes comprehensive GitHub Actions workflows:

- **cpp-build.yml**: Builds and tests on Linux, macOS, and Windows
- **node-bindings.yml**: Builds Node.js bindings for multiple platforms
- **release.yml**: Creates releases with artifacts for all platforms

### Artifacts Generated

- Linux binaries (GCC and Clang)
- macOS binaries (Apple Clang)
- Windows binaries (MSVC)
- Python wheels for Python 3.9-3.12
- Node.js native modules for Node 16, 18, 20

## Contributing

Contributions are welcome! Please ensure:

1. Code follows the existing style
2. All tests pass
3. New features include tests
4. Documentation is updated

## License

See the main repository LICENSE file.

## Integration with Agent Zero

The C++ backend is designed to work seamlessly with the existing Python-based Agent Zero framework. It provides:

- Performance-critical operations in native code
- Language interoperability
- Extensibility through bindings
- Production-ready deployment options
