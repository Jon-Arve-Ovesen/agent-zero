#include "agent_zero/memory_manager.hpp"
#include <algorithm>
#include <random>

namespace agent_zero {

// PIMPL implementation
class MemoryManager::Impl {
public:
    std::map<std::string, MemoryEntry> memories;
    size_t id_counter = 0;
    
    std::string generate_id() {
        return "mem_" + std::to_string(++id_counter);
    }
};

MemoryManager::MemoryManager()
    : pimpl_(std::make_unique<Impl>())
{
}

MemoryManager::~MemoryManager() = default;

std::string MemoryManager::save_memory(const std::string& content,
                                       const std::map<std::string, std::string>& metadata) {
    MemoryEntry entry;
    entry.id = pimpl_->generate_id();
    entry.content = content;
    entry.metadata = metadata;
    entry.timestamp = std::chrono::system_clock::now();
    entry.relevance_score = 1.0;
    
    pimpl_->memories[entry.id] = entry;
    return entry.id;
}

bool MemoryManager::delete_memory(const std::string& memory_id) {
    auto it = pimpl_->memories.find(memory_id);
    if (it != pimpl_->memories.end()) {
        pimpl_->memories.erase(it);
        return true;
    }
    return false;
}

std::optional<MemoryEntry> MemoryManager::load_memory(const std::string& memory_id) {
    auto it = pimpl_->memories.find(memory_id);
    if (it != pimpl_->memories.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<MemoryEntry> MemoryManager::search_memories(const MemoryQuery& query) {
    std::vector<MemoryEntry> results;
    
    for (const auto& [id, entry] : pimpl_->memories) {
        // Simple substring search
        if (entry.content.find(query.search_term) != std::string::npos) {
            if (entry.relevance_score >= query.min_relevance) {
                if (!query.category.has_value() || 
                    entry.metadata.count("category") && 
                    entry.metadata.at("category") == query.category.value()) {
                    results.push_back(entry);
                }
            }
        }
    }
    
    // Sort by relevance
    std::sort(results.begin(), results.end(),
        [](const MemoryEntry& a, const MemoryEntry& b) {
            return a.relevance_score > b.relevance_score;
        });
    
    // Limit results
    if (results.size() > query.max_results) {
        results.resize(query.max_results);
    }
    
    return results;
}

std::vector<MemoryEntry> MemoryManager::get_recent_memories(size_t count) {
    std::vector<MemoryEntry> all_memories;
    for (const auto& [id, entry] : pimpl_->memories) {
        all_memories.push_back(entry);
    }
    
    // Sort by timestamp (most recent first)
    std::sort(all_memories.begin(), all_memories.end(),
        [](const MemoryEntry& a, const MemoryEntry& b) {
            return a.timestamp > b.timestamp;
        });
    
    if (all_memories.size() > count) {
        all_memories.resize(count);
    }
    
    return all_memories;
}

void MemoryManager::consolidate_memories() {
    // Simple consolidation: reduce relevance of old memories
    auto now = std::chrono::system_clock::now();
    for (auto& [id, entry] : pimpl_->memories) {
        auto age = std::chrono::duration_cast<std::chrono::hours>(
            now - entry.timestamp).count();
        
        // Decay relevance based on age (simple exponential decay)
        double decay_factor = std::exp(-age / 168.0); // 1 week half-life
        entry.relevance_score *= decay_factor;
    }
}

void MemoryManager::clear_old_memories(int days) {
    auto cutoff = std::chrono::system_clock::now() - std::chrono::hours(days * 24);
    
    auto it = pimpl_->memories.begin();
    while (it != pimpl_->memories.end()) {
        if (it->second.timestamp < cutoff) {
            it = pimpl_->memories.erase(it);
        } else {
            ++it;
        }
    }
}

size_t MemoryManager::get_memory_count() const {
    return pimpl_->memories.size();
}

} // namespace agent_zero
