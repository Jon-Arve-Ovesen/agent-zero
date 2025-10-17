#ifndef AGENT_ZERO_TASK_EXECUTOR_HPP
#define AGENT_ZERO_TASK_EXECUTOR_HPP

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace agent_zero {

// Task priority levels
enum class TaskPriority {
    LOW,
    NORMAL,
    HIGH,
    CRITICAL
};

// Task status
enum class TaskStatus {
    PENDING,
    RUNNING,
    COMPLETED,
    FAILED,
    CANCELLED
};

// Task structure
struct Task {
    std::string id;
    std::string description;
    TaskPriority priority;
    TaskStatus status;
    std::function<std::string()> execute;
    
    Task(const std::string& desc, std::function<std::string()> exec, 
         TaskPriority prio = TaskPriority::NORMAL)
        : description(desc), priority(prio), status(TaskStatus::PENDING), execute(exec) {}
};

// Task Executor with thread pool
class TaskExecutor {
public:
    explicit TaskExecutor(size_t num_threads = std::thread::hardware_concurrency());
    ~TaskExecutor();
    
    // Submit tasks
    std::string submit_task(const std::string& description, 
                           std::function<std::string()> task,
                           TaskPriority priority = TaskPriority::NORMAL);
    
    // Task management
    void cancel_task(const std::string& task_id);
    TaskStatus get_task_status(const std::string& task_id);
    std::vector<Task> get_pending_tasks() const;
    
    // Control
    void pause();
    void resume();
    void shutdown();
    
private:
    void worker_thread();
    
    std::vector<std::thread> workers_;
    std::priority_queue<std::shared_ptr<Task>> task_queue_;
    
    mutable std::mutex queue_mutex_;
    std::condition_variable condition_;
    bool stop_;
    bool paused_;
    
    std::map<std::string, std::shared_ptr<Task>> tasks_;
};

} // namespace agent_zero

#endif // AGENT_ZERO_TASK_EXECUTOR_HPP
