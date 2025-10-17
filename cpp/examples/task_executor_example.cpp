#include <iostream>
#include <thread>
#include <chrono>
#include "agent_zero/task_executor.hpp"

using namespace agent_zero;

int main() {
    std::cout << "=== Task Executor Example ===" << std::endl << std::endl;
    
    // Create task executor with 4 worker threads
    TaskExecutor executor(4);
    
    std::cout << "Submitting tasks..." << std::endl;
    
    // Submit various tasks with different priorities
    auto task1 = executor.submit_task("Low priority task", []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return "Task 1 completed";
    }, TaskPriority::LOW);
    
    auto task2 = executor.submit_task("High priority task", []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return "Task 2 completed";
    }, TaskPriority::HIGH);
    
    auto task3 = executor.submit_task("Normal priority task", []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(75));
        return "Task 3 completed";
    }, TaskPriority::NORMAL);
    
    auto task4 = executor.submit_task("Critical task", []() {
        return "Task 4 completed immediately";
    }, TaskPriority::CRITICAL);
    
    std::cout << "Submitted 4 tasks with different priorities" << std::endl;
    std::cout << "Task IDs: " << task1 << ", " << task2 << ", " 
              << task3 << ", " << task4 << std::endl << std::endl;
    
    // Monitor task status
    std::cout << "Monitoring task status..." << std::endl;
    
    auto print_status = [&](const std::string& id) {
        auto status = executor.get_task_status(id);
        std::cout << "Task " << id << ": ";
        switch (status) {
            case TaskStatus::PENDING: std::cout << "PENDING"; break;
            case TaskStatus::RUNNING: std::cout << "RUNNING"; break;
            case TaskStatus::COMPLETED: std::cout << "COMPLETED"; break;
            case TaskStatus::FAILED: std::cout << "FAILED"; break;
            case TaskStatus::CANCELLED: std::cout << "CANCELLED"; break;
        }
        std::cout << std::endl;
    };
    
    // Check status periodically
    for (int i = 0; i < 5; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        std::cout << "\nStatus check " << (i + 1) << ":" << std::endl;
        print_status(task1);
        print_status(task2);
        print_status(task3);
        print_status(task4);
    }
    
    // Demonstrate pause/resume
    std::cout << "\n=== Testing Pause/Resume ===" << std::endl;
    executor.pause();
    std::cout << "Executor paused" << std::endl;
    
    auto paused_task = executor.submit_task("Task while paused", []() {
        return "Should execute after resume";
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Task submitted while paused: ";
    print_status(paused_task);
    
    executor.resume();
    std::cout << "Executor resumed" << std::endl;
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "After resume: ";
    print_status(paused_task);
    
    std::cout << "\n=== Shutting down ===" << std::endl;
    executor.shutdown();
    
    return 0;
}
