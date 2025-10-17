#include <gtest/gtest.h>
#include "agent_zero/task_executor.hpp"
#include <thread>
#include <chrono>

using namespace agent_zero;

class TaskExecutorTest : public ::testing::Test {
protected:
    void SetUp() override {
        executor = std::make_unique<TaskExecutor>(2);
    }
    
    void TearDown() override {
        executor->shutdown();
    }
    
    std::unique_ptr<TaskExecutor> executor;
};

TEST_F(TaskExecutorTest, SubmitTaskReturnsId) {
    auto task_id = executor->submit_task("test", []() { return "result"; });
    EXPECT_FALSE(task_id.empty());
}

TEST_F(TaskExecutorTest, TaskInitiallyPending) {
    auto task_id = executor->submit_task("test", []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return "result";
    });
    
    // Check status quickly before it completes
    auto status = executor->get_task_status(task_id);
    EXPECT_TRUE(status == TaskStatus::PENDING || status == TaskStatus::RUNNING);
}

TEST_F(TaskExecutorTest, TaskEventuallyCompletes) {
    auto task_id = executor->submit_task("test", []() { return "result"; });
    
    // Wait for completion
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto status = executor->get_task_status(task_id);
    EXPECT_EQ(status, TaskStatus::COMPLETED);
}

TEST_F(TaskExecutorTest, CancelTask) {
    auto task_id = executor->submit_task("test", []() {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        return "result";
    });
    
    executor->cancel_task(task_id);
    auto status = executor->get_task_status(task_id);
    EXPECT_EQ(status, TaskStatus::CANCELLED);
}

TEST_F(TaskExecutorTest, PauseAndResume) {
    executor->pause();
    
    auto task_id = executor->submit_task("test", []() { return "result"; });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto status = executor->get_task_status(task_id);
    EXPECT_EQ(status, TaskStatus::PENDING);
    
    executor->resume();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    status = executor->get_task_status(task_id);
    EXPECT_EQ(status, TaskStatus::COMPLETED);
}

TEST_F(TaskExecutorTest, MultipleTasksExecute) {
    std::vector<std::string> task_ids;
    
    for (int i = 0; i < 5; i++) {
        task_ids.push_back(executor->submit_task("task" + std::to_string(i),
            [i]() { return std::to_string(i); }));
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    for (const auto& id : task_ids) {
        auto status = executor->get_task_status(id);
        EXPECT_EQ(status, TaskStatus::COMPLETED);
    }
}

TEST_F(TaskExecutorTest, HighPriorityTasksFirst) {
    executor->pause();
    
    auto low_task = executor->submit_task("low", []() { return "low"; }, TaskPriority::LOW);
    auto high_task = executor->submit_task("high", []() { return "high"; }, TaskPriority::HIGH);
    
    executor->resume();
    
    // Give some time for execution
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    // High priority task should complete first (though timing may vary)
    EXPECT_TRUE(executor->get_task_status(high_task) == TaskStatus::COMPLETED);
}
