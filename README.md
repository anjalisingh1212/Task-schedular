# Task Distributor – Multithreaded Scheduler using C++ Threads and POSIX Message Queues

## Overview

This project implements a **multithreaded task distribution system** using **C++ threads** and **POSIX message queues (MQs)**.  
It demonstrates interprocess communication between a client and a scheduler process:

- A **Client** sends computational tasks to a **Scheduler**.
- The **Scheduler** assigns tasks to multiple **worker threads**.
- Results are sent back through **client-specific result queues**.

---

## Features

- **Inter-Process Communication (IPC):** Uses POSIX message queues for safe, fast message passing between processes.
- **Multithreaded Scheduler:** Multiple worker threads process tasks concurrently.
- **Real-Time Dispatching:** Dedicated dispatcher thread routes results back to clients.
- **Thread-Safe Queues:** Protected by `std::mutex` and `std::condition_variable`.
- **Graceful Shutdown:** Uses atomic flags and clean exit handling for threads.
- **Multi-Client Support:** Multiple clients can send tasks simultaneously.
- **Asynchronous Results:** Each client receives its result independently.

## Components

### Scheduler
- Handles incoming tasks via `/task_queue`.
- Maintains:
  - A **task queue**.
  - A **result queue**.
  - Multiple **worker threads**.
  - A **dispatcher thread**.
- Synchronization is done using:
  - `std::mutex`
  - `std::condition_variable`
- Uses an **atomic flag (`running`)** for safe shutdown.

### Client
- Console-based interactive menu.
- Creates tasks and sends them to the scheduler MQ.
- Listens for results on a separate thread through its own **result MQ**.
- Gracefully exits and cleans up MQ resources.

### Multi-Client Support
- All clients share the same **task queue** (`/task_queue`).
- Each client creates a **unique result queue** (`/result_mq_<pid>`).
- The scheduler receives tasks from all clients, assigns them to workers, and sends results to the correct result queue.
- Allows multiple clients to operate simultaneously without interference.

---

## Technologies Used

| Category        | Tools / Libraries                           |
|-----------------|--------------------------------------------|
| Language        | C++17                                      |
| IPC             | POSIX Message Queues (`mqueue.h`)          |
| Multithreading  | `<thread>`, `<mutex>`, `<condition_variable>` |
| Synchronization | `std::mutex`, `std::atomic`, condition variables |
| Build System    | CMake                                      |

---

## Build Instructions

```bash
mkdir build
cd build
cmake ..
make
```
This generates two executables:

  -  **scheduler**
  -  **client**

## Running the Project
Step 1 — Start the Scheduler
```bash
./scheduler
```
Step 2 — Start One or More Clients
In separate terminals:
```bash
./client
```

**Each client will:**
- Create a unique result queue.
- Send a task to the scheduler.
- Wait asynchronously for the result.
- Clean up message queues.

**If execution is stopped abruptly, clean up old queues:**
```bash
ls /dev/mqueue
sudo rm /dev/mqueue/task_queue
sudo rm /dev/mqueue/result_mq_*
```

---

## Concepts Demonstrated

- Inter-Process Communication (IPC) with POSIX message queues.
- Multithreaded programming with C++ threads.
- Thread synchronization using mutexes and condition variables.
- Atomic flags for safe shutdown.
- Multi-client support with isolated result queues.
- Graceful cleanup and resource management.