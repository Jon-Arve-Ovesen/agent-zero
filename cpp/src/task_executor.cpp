#include "agent_zero/task_executor.hpp"
#include <algorithm>
#include <chrono>

namespace agent_zero {

// Task priority comparison for priority queue
struct TaskComparator {
    bool operator()(const std::shared_ptr<Task>& a, const std::shared_ptr<Task>& b) const {
        return static_cast<int>(a->priority) < static_cast<int>(b->priority);
    }
};

TaskExecutor::TaskExecutor(size_t num_threads)
    : stop_(false)
    , paused_(false)
{
    for (size_t i = 0; i < num_threads; ++i) {
        workers_.emplace_back([this] { worker_thread(); });
    }
}

TaskExecutor::~TaskExecutor() {
    shutdown();
}

std::string TaskExecutor::submit_task(const std::string& description,
                                      std::function<std::string()> task,
                                      TaskPriority priority) {
    auto task_obj = std::make_shared<Task>(description, task, priority);
    
    // Generate unique ID
    static size_t id_counter = 0;
    task_obj->id = "task_" + std::to_string(++id_counter);
    
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        tasks_[task_obj->id] = task_obj;
        task_queue_.push(task_obj);
    }
    
    condition_.notify_one();
    return task_obj->id;
}

void TaskExecutor::cancel_task(const std::string& task_id) {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    auto it = tasks_.find(task_id);
    if (it != tasks_.end()) {
        it->second->status = TaskStatus::CANCELLED;
    }
}

TaskStatus TaskExecutor::get_task_status(const std::string& task_id) {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    auto it = tasks_.find(task_id);
    if (it != tasks_.end()) {
        return it->second->status;
    }
    return TaskStatus::FAILED;
}

std::vector<Task> TaskExecutor::get_pending_tasks() const {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    std::vector<Task> pending;
    for (const auto& [id, task] : tasks_) {
        if (task->status == TaskStatus::PENDING) {
            pending.push_back(*task);
        }
    }
    return pending;
}

void TaskExecutor::pause() {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    paused_ = true;
}

void TaskExecutor::resume() {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    paused_ = false;
    condition_.notify_all();
}

void TaskExecutor::shutdown() {
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        stop_ = true;
    }
    condition_.notify_all();
    
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void TaskExecutor::worker_thread() {
    while (true) {
        std::shared_ptr<Task> task;
        
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            condition_.wait(lock, [this] {
                return stop_ || (!paused_ && !task_queue_.empty());
            });
            
            if (stop_) {
                return;
            }
            
            if (paused_ || task_queue_.empty()) {
                continue;
            }
            
            task = task_queue_.top();
            task_queue_.pop();
        }
        
        if (task && task->status != TaskStatus::CANCELLED) {
            task->status = TaskStatus::RUNNING;
            
            try {
                task->execute();
                task->status = TaskStatus::COMPLETED;
            } catch (...) {
                task->status = TaskStatus::FAILED;
            }
        }
    }
}

} // namespace agent_zero
