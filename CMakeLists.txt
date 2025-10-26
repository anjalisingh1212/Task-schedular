cmake_minimum_required(VERSION 3.10)
project(TaskScheduler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(include)

# Find and link pthread (used by std::thread)
find_package(Threads REQUIRED)

add_executable(client src/client.cpp)
add_executable(scheduler src/main.cpp src/scheduler.cpp src/task.cpp)
target_link_libraries(scheduler PRIVATE Threads::Threads rt) #link POSIX threads + message queue
target_link_libraries(client PRIVATE rt) #link message queue
