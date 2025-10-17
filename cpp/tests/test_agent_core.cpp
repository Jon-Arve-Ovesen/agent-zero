#include <gtest/gtest.h>
#include "agent_zero/agent_core.hpp"

using namespace agent_zero;

class AgentCoreTest : public ::testing::Test {
protected:
    void SetUp() override {
        agent = std::make_unique<Agent>("test_agent", "test_model");
    }
    
    std::unique_ptr<Agent> agent;
};

TEST_F(AgentCoreTest, ConstructorSetsNameAndModel) {
    EXPECT_EQ(agent->get_name(), "test_agent");
    EXPECT_EQ(agent->get_model(), "test_model");
}

TEST_F(AgentCoreTest, InitialStateIsIdle) {
    EXPECT_EQ(agent->get_state(), AgentState::IDLE);
}

TEST_F(AgentCoreTest, ProcessMessageReturnsResponse) {
    std::string message = "Hello, agent!";
    std::string response = agent->process_message(message);
    
    EXPECT_FALSE(response.empty());
    EXPECT_NE(response.find("test_agent"), std::string::npos);
    EXPECT_NE(response.find(message), std::string::npos);
}

TEST_F(AgentCoreTest, StateChangesAfterProcessing) {
    agent->process_message("test");
    EXPECT_EQ(agent->get_state(), AgentState::COMPLETED);
}

TEST_F(AgentCoreTest, ContextIsIncludedInResponse) {
    std::map<std::string, std::string> context = {
        {"key1", "value1"},
        {"key2", "value2"}
    };
    agent->set_context(context);
    
    std::string response = agent->process_message("test");
    EXPECT_NE(response.find("Context"), std::string::npos);
    EXPECT_NE(response.find("key1"), std::string::npos);
    EXPECT_NE(response.find("value1"), std::string::npos);
}

// Test C API
TEST(AgentCoreC_API, CreateAndDestroy) {
    AgentHandle handle = agent_create("c_agent", "c_model");
    EXPECT_NE(handle, nullptr);
    agent_destroy(handle);
}

TEST(AgentCoreC_API, ProcessMessage) {
    AgentHandle handle = agent_create("c_agent", "c_model");
    ASSERT_NE(handle, nullptr);
    
    const char* response = agent_process_message(handle, "test message");
    EXPECT_NE(response, nullptr);
    
    agent_destroy(handle);
}

TEST(AgentCoreC_API, GetState) {
    AgentHandle handle = agent_create("c_agent", "c_model");
    ASSERT_NE(handle, nullptr);
    
    int state = agent_get_state(handle);
    EXPECT_EQ(state, static_cast<int>(AgentState::IDLE));
    
    agent_destroy(handle);
}

// Test functional programming utilities
TEST(FunctionalTest, MapTransformation) {
    std::vector<int> input = {1, 2, 3, 4, 5};
    auto result = functional::map(input, [](int x) { return x * 2; });
    
    EXPECT_EQ(result.size(), 5);
    EXPECT_EQ(result[0], 2);
    EXPECT_EQ(result[4], 10);
}

TEST(FunctionalTest, FilterPredicate) {
    std::vector<int> input = {1, 2, 3, 4, 5, 6};
    auto result = functional::filter(input, [](int x) { return x % 2 == 0; });
    
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], 2);
    EXPECT_EQ(result[1], 4);
    EXPECT_EQ(result[2], 6);
}

TEST(FunctionalTest, ReduceAccumulation) {
    std::vector<int> input = {1, 2, 3, 4, 5};
    int result = functional::reduce(input, 0, [](int acc, int x) { return acc + x; });
    
    EXPECT_EQ(result, 15);
}

// Test builder pattern
TEST(UtilsTest, AgentBuilder) {
    auto agent = utils::AgentBuilder()
        .with_name("builder_agent")
        .with_model("builder_model")
        .build();
    
    EXPECT_NE(agent, nullptr);
    EXPECT_EQ(agent->get_name(), "builder_agent");
    EXPECT_EQ(agent->get_model(), "builder_model");
}

TEST(UtilsTest, AgentBuilderWithContext) {
    std::map<std::string, std::string> context = {{"test", "value"}};
    auto agent = utils::AgentBuilder()
        .with_name("agent")
        .with_model("model")
        .with_context(context)
        .build();
    
    EXPECT_NE(agent, nullptr);
}
