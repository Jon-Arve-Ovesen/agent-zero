#ifndef AGENT_ZERO_MEMORY_MANAGER_HPP
#define AGENT_ZERO_MEMORY_MANAGER_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <optional>

namespace agent_zero {

// Memory entry structure
struct MemoryEntry {
    std::string id;
    std::string content;
    std::map<std::string, std::string> metadata;
    std::chrono::system_clock::time_point timestamp;
    double relevance_score;
    
    MemoryEntry() : relevance_score(0.0) {}
};

// Memory search query
struct MemoryQuery {
    std::string search_term;
    std::optional<std::string> category;
    size_t max_results;
    double min_relevance;
    
    MemoryQuery() : max_results(10), min_relevance(0.0) {}
};

// Memory Manager for persistent storage
class MemoryManager {
public:
    MemoryManager();
    virtual ~MemoryManager();
    
    // Memory operations
    std::string save_memory(const std::string& content, 
                           const std::map<std::string, std::string>& metadata = {});
    
    bool delete_memory(const std::string& memory_id);
    std::optional<MemoryEntry> load_memory(const std::string& memory_id);
    
    // Search and retrieval
    std::vector<MemoryEntry> search_memories(const MemoryQuery& query);
    std::vector<MemoryEntry> get_recent_memories(size_t count = 10);
    
    // Maintenance
    void consolidate_memories();
    void clear_old_memories(int days = 30);
    size_t get_memory_count() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace agent_zero

#endif // AGENT_ZERO_MEMORY_MANAGER_HPP
