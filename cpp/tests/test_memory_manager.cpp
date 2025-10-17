#include <gtest/gtest.h>
#include "agent_zero/memory_manager.hpp"

using namespace agent_zero;

class MemoryManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        manager = std::make_unique<MemoryManager>();
    }
    
    std::unique_ptr<MemoryManager> manager;
};

TEST_F(MemoryManagerTest, SaveMemoryReturnsId) {
    std::string id = manager->save_memory("test content");
    EXPECT_FALSE(id.empty());
}

TEST_F(MemoryManagerTest, LoadSavedMemory) {
    std::string content = "test memory content";
    std::string id = manager->save_memory(content);
    
    auto memory = manager->load_memory(id);
    ASSERT_TRUE(memory.has_value());
    EXPECT_EQ(memory->content, content);
}

TEST_F(MemoryManagerTest, LoadNonexistentMemory) {
    auto memory = manager->load_memory("nonexistent_id");
    EXPECT_FALSE(memory.has_value());
}

TEST_F(MemoryManagerTest, DeleteMemory) {
    std::string id = manager->save_memory("test");
    EXPECT_TRUE(manager->delete_memory(id));
    
    auto memory = manager->load_memory(id);
    EXPECT_FALSE(memory.has_value());
}

TEST_F(MemoryManagerTest, SaveMemoryWithMetadata) {
    std::map<std::string, std::string> metadata = {
        {"category", "test"},
        {"author", "system"}
    };
    
    std::string id = manager->save_memory("content", metadata);
    auto memory = manager->load_memory(id);
    
    ASSERT_TRUE(memory.has_value());
    EXPECT_EQ(memory->metadata["category"], "test");
    EXPECT_EQ(memory->metadata["author"], "system");
}

TEST_F(MemoryManagerTest, SearchMemories) {
    manager->save_memory("first test memory");
    manager->save_memory("second test memory");
    manager->save_memory("different content");
    
    MemoryQuery query;
    query.search_term = "test";
    query.max_results = 10;
    
    auto results = manager->search_memories(query);
    EXPECT_EQ(results.size(), 2);
}

TEST_F(MemoryManagerTest, SearchWithCategory) {
    std::map<std::string, std::string> meta1 = {{"category", "A"}};
    std::map<std::string, std::string> meta2 = {{"category", "B"}};
    
    manager->save_memory("content A", meta1);
    manager->save_memory("content B", meta2);
    manager->save_memory("content A2", meta1);
    
    MemoryQuery query;
    query.search_term = "content";
    query.category = "A";
    
    auto results = manager->search_memories(query);
    EXPECT_EQ(results.size(), 2);
}

TEST_F(MemoryManagerTest, GetRecentMemories) {
    for (int i = 0; i < 5; i++) {
        manager->save_memory("memory " + std::to_string(i));
    }
    
    auto recent = manager->get_recent_memories(3);
    EXPECT_EQ(recent.size(), 3);
}

TEST_F(MemoryManagerTest, GetMemoryCount) {
    EXPECT_EQ(manager->get_memory_count(), 0);
    
    manager->save_memory("test1");
    manager->save_memory("test2");
    
    EXPECT_EQ(manager->get_memory_count(), 2);
}

TEST_F(MemoryManagerTest, ConsolidateMemories) {
    manager->save_memory("test");
    EXPECT_NO_THROW(manager->consolidate_memories());
}

TEST_F(MemoryManagerTest, ClearOldMemories) {
    manager->save_memory("test");
    EXPECT_NO_THROW(manager->clear_old_memories(0));
    EXPECT_EQ(manager->get_memory_count(), 0);
}
