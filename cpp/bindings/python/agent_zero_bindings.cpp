#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "agent_zero/agent_core.hpp"
#include "agent_zero/task_executor.hpp"
#include "agent_zero/memory_manager.hpp"
#include "agent_zero/tool_interface.hpp"
#include "agent_zero/multi_lang_bridge.hpp"

namespace py = pybind11;
using namespace agent_zero;

PYBIND11_MODULE(agent_zero_native, m) {
    m.doc() = "Agent Zero C++ Backend - Native Python bindings";
    
    // AgentState enum
    py::enum_<AgentState>(m, "AgentState")
        .value("IDLE", AgentState::IDLE)
        .value("PROCESSING", AgentState::PROCESSING)
        .value("WAITING", AgentState::WAITING)
        .value("ERROR", AgentState::ERROR)
        .value("COMPLETED", AgentState::COMPLETED)
        .export_values();
    
    // Agent class
    py::class_<Agent>(m, "Agent")
        .def(py::init<const std::string&, const std::string&>(),
             py::arg("name"), py::arg("model"))
        .def("process_message", &Agent::process_message,
             py::arg("message"),
             "Process a message and return the response")
        .def("set_context", &Agent::set_context,
             py::arg("context"),
             "Set the agent's context")
        .def("get_state", &Agent::get_state,
             "Get the current agent state")
        .def_property("name",
                     &Agent::get_name,
                     &Agent::set_name,
                     "Agent name")
        .def_property("model",
                     &Agent::get_model,
                     &Agent::set_model,
                     "Model name");
    
    // TaskPriority enum
    py::enum_<TaskPriority>(m, "TaskPriority")
        .value("LOW", TaskPriority::LOW)
        .value("NORMAL", TaskPriority::NORMAL)
        .value("HIGH", TaskPriority::HIGH)
        .value("CRITICAL", TaskPriority::CRITICAL)
        .export_values();
    
    // TaskStatus enum
    py::enum_<TaskStatus>(m, "TaskStatus")
        .value("PENDING", TaskStatus::PENDING)
        .value("RUNNING", TaskStatus::RUNNING)
        .value("COMPLETED", TaskStatus::COMPLETED)
        .value("FAILED", TaskStatus::FAILED)
        .value("CANCELLED", TaskStatus::CANCELLED)
        .export_values();
    
    // TaskExecutor class
    py::class_<TaskExecutor>(m, "TaskExecutor")
        .def(py::init<size_t>(),
             py::arg("num_threads") = std::thread::hardware_concurrency(),
             "Create a task executor with specified number of threads")
        .def("submit_task", &TaskExecutor::submit_task,
             py::arg("description"),
             py::arg("task"),
             py::arg("priority") = TaskPriority::NORMAL,
             "Submit a task for execution")
        .def("cancel_task", &TaskExecutor::cancel_task,
             py::arg("task_id"),
             "Cancel a pending or running task")
        .def("get_task_status", &TaskExecutor::get_task_status,
             py::arg("task_id"),
             "Get the status of a task")
        .def("pause", &TaskExecutor::pause,
             "Pause task execution")
        .def("resume", &TaskExecutor::resume,
             "Resume task execution")
        .def("shutdown", &TaskExecutor::shutdown,
             "Shutdown the task executor");
    
    // MemoryEntry struct
    py::class_<MemoryEntry>(m, "MemoryEntry")
        .def(py::init<>())
        .def_readwrite("id", &MemoryEntry::id)
        .def_readwrite("content", &MemoryEntry::content)
        .def_readwrite("metadata", &MemoryEntry::metadata)
        .def_readwrite("relevance_score", &MemoryEntry::relevance_score);
    
    // MemoryQuery struct
    py::class_<MemoryQuery>(m, "MemoryQuery")
        .def(py::init<>())
        .def_readwrite("search_term", &MemoryQuery::search_term)
        .def_readwrite("category", &MemoryQuery::category)
        .def_readwrite("max_results", &MemoryQuery::max_results)
        .def_readwrite("min_relevance", &MemoryQuery::min_relevance);
    
    // MemoryManager class
    py::class_<MemoryManager>(m, "MemoryManager")
        .def(py::init<>())
        .def("save_memory", &MemoryManager::save_memory,
             py::arg("content"),
             py::arg("metadata") = std::map<std::string, std::string>(),
             "Save a memory entry")
        .def("delete_memory", &MemoryManager::delete_memory,
             py::arg("memory_id"),
             "Delete a memory entry")
        .def("load_memory", &MemoryManager::load_memory,
             py::arg("memory_id"),
             "Load a memory entry")
        .def("search_memories", &MemoryManager::search_memories,
             py::arg("query"),
             "Search for memories")
        .def("get_recent_memories", &MemoryManager::get_recent_memories,
             py::arg("count") = 10,
             "Get recent memories")
        .def("consolidate_memories", &MemoryManager::consolidate_memories,
             "Consolidate and optimize memories")
        .def("clear_old_memories", &MemoryManager::clear_old_memories,
             py::arg("days") = 30,
             "Clear old memories")
        .def("get_memory_count", &MemoryManager::get_memory_count,
             "Get total memory count");
    
    // ToolResult struct
    py::class_<ToolResult>(m, "ToolResult")
        .def(py::init<>())
        .def(py::init<const std::string&>())
        .def_readwrite("success", &ToolResult::success)
        .def_readwrite("output", &ToolResult::output)
        .def_readwrite("error", &ToolResult::error)
        .def_readwrite("metadata", &ToolResult::metadata);
    
    // Language enum
    py::enum_<Language>(m, "Language")
        .value("PYTHON", Language::PYTHON)
        .value("JAVASCRIPT", Language::JAVASCRIPT)
        .value("GO", Language::GO)
        .value("RUST", Language::RUST)
        .value("RUBY", Language::RUBY)
        .value("NATIVE_CPP", Language::NATIVE_CPP)
        .export_values();
    
    // BridgeResult struct
    py::class_<BridgeResult>(m, "BridgeResult")
        .def(py::init<>())
        .def_readwrite("success", &BridgeResult::success)
        .def_readwrite("result", &BridgeResult::result)
        .def_readwrite("error", &BridgeResult::error)
        .def_readwrite("language", &BridgeResult::language);
    
    // MultiLangBridge class
    py::class_<MultiLangBridge>(m, "MultiLangBridge")
        .def(py::init<>())
        .def("execute_python", &MultiLangBridge::execute_python,
             py::arg("code"),
             py::arg("vars") = std::map<std::string, std::string>(),
             "Execute Python code")
        .def("execute_javascript", &MultiLangBridge::execute_javascript,
             py::arg("code"),
             py::arg("vars") = std::map<std::string, std::string>(),
             "Execute JavaScript code")
        .def("execute_go", &MultiLangBridge::execute_go,
             py::arg("code"),
             py::arg("vars") = std::map<std::string, std::string>(),
             "Execute Go code")
        .def("execute", &MultiLangBridge::execute,
             py::arg("language"),
             py::arg("code"),
             py::arg("vars") = std::map<std::string, std::string>(),
             "Execute code in specified language")
        .def("is_language_available", &MultiLangBridge::is_language_available,
             py::arg("language"),
             "Check if a language is available")
        .def("get_available_languages", &MultiLangBridge::get_available_languages,
             "Get list of available languages");
    
    // Utility classes
    py::class_<utils::AgentBuilder>(m, "AgentBuilder")
        .def(py::init<>())
        .def("with_name", &utils::AgentBuilder::with_name,
             py::arg("name"),
             "Set agent name")
        .def("with_model", &utils::AgentBuilder::with_model,
             py::arg("model"),
             "Set agent model")
        .def("with_context", &utils::AgentBuilder::with_context,
             py::arg("context"),
             "Set agent context")
        .def("build", &utils::AgentBuilder::build,
             "Build the agent");
}
